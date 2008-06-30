/*
 *   Copyright (c) 2007, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cassert>

namespace flens {

//-- CRS -----------------------------------------------------------------------

// -- constructors -------------------------------------------------------------
template <typename T, CRS_Storage Storage>
CRS<T, Storage>::CRS()
    : _numRows(0)
{
}

template <typename T, CRS_Storage Storage>
CRS<T, Storage>::CRS(int numRows, int numCols, int approxBandwidth)
    : rows(numRows+1), _numRows(numRows), _numCols(numCols), _k(approxBandwidth)
{
}

template <typename T, CRS_Storage Storage>
CRS<T, Storage>::~CRS()
{
}

// -- operators ----------------------------------------------------------------
template <typename T, CRS_Storage Storage>
CRS<T, Storage> &
CRS<T, Storage>::operator*=(T alpha)
{
    values *= alpha;
    return *this;
}

template <typename T, CRS_Storage Storage>
CRS<T, Storage> &
CRS<T, Storage>::operator/=(T alpha)
{
    values /= alpha;
    return *this;
}

// -- methods ------------------------------------------------------------------
template <typename T, CRS_Storage Storage>
typename CRS<T, Storage>::Initializer *
CRS<T, Storage>::initializer()
{
    return new Initializer(*this, _k);
}

template <typename T, CRS_Storage Storage>
void
CRS<T, Storage>::allocate(int numNonZeros)
{
    values.resize(numNonZeros);
    columns.resize(numNonZeros);

    rows(1) = 1;
    rows(_numRows+1) = numNonZeros+1;
}

template <typename T, CRS_Storage Storage>
int
CRS<T, Storage>::numRows() const
{
    return _numRows;
}

template <typename T, CRS_Storage Storage>
int
CRS<T, Storage>::numCols() const
{
    return _numCols;
}

template <typename T, CRS_Storage Storage>
int
CRS<T, Storage>::numNonZeros() const
{
    return rows(_numRows+1)-rows(1);
}

template <typename T, CRS_Storage Storage>
typename CRS<T, Storage>::const_iterator
CRS<T, Storage>::begin() const
{
    return const_iterator(*this, 1);
}

template <typename T, CRS_Storage Storage>
typename CRS<T, Storage>::iterator
CRS<T, Storage>::begin()
{
    return iterator(*this, 1);
}

template <typename T, CRS_Storage Storage>
typename CRS<T, Storage>::const_iterator
CRS<T, Storage>::end() const
{
    return const_iterator(*this, numNonZeros()+1);
}

template <typename T, CRS_Storage Storage>
typename CRS<T, Storage>::iterator
CRS<T, Storage>::end()
{
    return iterator(*this, numNonZeros()+1);
}

//-- CRS_Coordinate ------------------------------------------------------------

template <typename T>
CRS_Coordinate<T>::CRS_Coordinate(int _row, int _col, T _value)
    : row(_row), col(_col), value(_value)
{
}

//-- CRS_CoordinateCmp ---------------------------------------------------------

template <typename T>
bool
CRS_CoordinateCmp::operator()(const CRS_Coordinate<T> &a,
                              const CRS_Coordinate<T> &b) const
{
    if (a.row<b.row) {
        return true;
    }
    if ((a.row==b.row) && (a.col<b.col)) {
        return true;
    }
    return false;
}

//-- CRS_Initializer -----------------------------------------------------------

template <typename T, CRS_Storage Storage>
CRS_Initializer<T, Storage>::CRS_Initializer(CRS<T, Storage> &crs, int k)
    : _lastSortedCoord(0), _isSorted(true), _crs(crs)
{
    _coordinates.reserve(k*crs.numRows());
}

template <typename T, CRS_Storage Storage>
CRS_Initializer<T, Storage>::~CRS_Initializer()
{
    sort();

    // check for empty rows and insert 0 on diagonal if needed
    std::vector<int> fillIn;
    int row = 0;
    for (size_t k=0; k<_coordinates.size(); ++k) {
        int rowDiff = _coordinates[k].row - row;
        for (int r=1; r<rowDiff; ++r) {
            fillIn.push_back(row+r);
        }
        row += rowDiff;
        if ((Storage==CRS_UpperTriangular) && (rowDiff>0)) {
            if (_coordinates[k].row!=_coordinates[k].col) {
                fillIn.push_back(_coordinates[k].row);
            }
        }
        if ((Storage==CRS_LowerTriangular)
         && (k<_coordinates.size()-1)
         && (_coordinates[k].row<_coordinates[k+1].row)) {
            if (_coordinates[k].row!=_coordinates[k].col) {
                fillIn.push_back(_coordinates[k].row);
            }
        }
    }
    if (Storage==CRS_LowerTriangular) {
        int k = _coordinates.size();
        if (_coordinates[k-1].row!=_coordinates[k-1].col) {
            fillIn.push_back(_coordinates[k-1].row);
        }
    }
    for (; row<_crs.numRows(); ++row) {
            fillIn.push_back(row+1);
    }
    // add fillins
    if (fillIn.size()>0) {
        for (size_t k=0; k<fillIn.size(); ++k) {
            operator()(fillIn[k], // fillIn[k]
                       _crs.numCols()) = T(0);
        }
        sort();
    }
    
    // allocate memory for the crs format
    _crs.allocate(_coordinates.size());

    // convert coordinate format to crs
    row = 1;
    for (size_t k=0; k<_coordinates.size(); ++k) {
        if (_coordinates[k].row>row) {
            assert(_coordinates[k].row-row==1);
            _crs.rows(row+1) = k+1;
            row = _coordinates[k].row;
        }
        _crs.columns(k+1) = _coordinates[k].col;
        _crs.values(k+1) = _coordinates[k].value;
    }
    _crs.rows(row+1) = _coordinates.size()+1;

#ifndef NDEBUG
    if (Storage==CRS_UpperTriangular) {
        for (int i=1; i<=_crs.numRows(); ++i) {
            if (_crs.columns(_crs.rows(i))!=i) {
                assert(0);
            }
        }
    }
    if (Storage==CRS_LowerTriangular) {
        for (int i=1; i<=_crs.numRows(); ++i) {
            assert(_crs.rows(i+1)-1>0);
            if (_crs.columns(_crs.rows(i+1)-1)!=i) {
                assert(0);
            }
        }
    }
#endif // NDEBUG
}

template <typename T, CRS_Storage Storage>
void
CRS_Initializer<T, Storage>::sort()
{
    // sort
    if (!_isSorted) {
        std::sort(_coordinates.begin()+_lastSortedCoord,
                  _coordinates.end(),
                  _less);
    }
    if ((_lastSortedCoord>0) && (_lastSortedCoord<_coordinates.size())) {
        std::inplace_merge(_coordinates.begin(),
                           _coordinates.begin() + _lastSortedCoord,
                           _coordinates.end(),
                           _less);
    }
    _isSorted = true;

    // eliminate duplicates
    size_t i, I;
    for (i=0, I=1; I<_coordinates.size(); ++i, ++I) {
        while ((!_less(_coordinates[i], _coordinates[I]))
            && (I<_coordinates.size()))
        {
            _coordinates[i].value += _coordinates[I].value;
            _coordinates[I].value = T(0);
            ++I;
        }
        if (I<_coordinates.size()) {
            _coordinates[i+1] = _coordinates[I];
        }
    }
    if ((i<_coordinates.size()) && (I-i-1>0)) {
        _coordinates.erase(_coordinates.end()-(I-i-1), _coordinates.end());
    }
    _lastSortedCoord = _coordinates.size();
}

template <typename T, CRS_Storage Storage>
T &
CRS_Initializer<T, Storage>::operator()(int row, int col)
{
    assert(row>=1);
    assert(row<=_crs.numRows());
    assert(col>=1);
    assert(col<=_crs.numCols());

    int r = -1,
        c = -1;
    if (Storage==CRS_General) {
        r = row;
        c = col;
    }
    if (Storage==CRS_UpperTriangular) {
        r = std::min(row, col);
        c = std::max(row, col);
    }
    if (Storage==CRS_LowerTriangular) {
        r = std::max(row, col);
        c = std::min(row, col);
    }
    assert((r>0) && (c>0));

    if (_coordinates.size()>=_coordinates.capacity()) {
        sort();
        _coordinates.reserve(_coordinates.capacity() + _crs.numRows());
    }

    // add new coordinate, check if coord.-list is still sorted
    _coordinates.push_back(CRS_Coordinate<T>(r, c, T(0)));
    size_t lastIndex = _coordinates.size()-1;
    if ((lastIndex>0) && _isSorted) {
        if (_less(_coordinates[lastIndex-1], _coordinates[lastIndex])) {
            _lastSortedCoord = lastIndex;
        } else {
            _isSorted = false;
        }
    }
    return _coordinates[lastIndex].value;
}

//-- CRS_ConstIterator ---------------------------------------------------------

template <typename T, CRS_Storage Storage>
CRS_ConstIterator<T, Storage>::CRS_ConstIterator(const CRS_ConstIterator &rhs)
    : _crs(rhs._crs), _pos(rhs._pos)
{
    if (_pos<=_crs.numNonZeros()) {
        int col = _crs.columns(_pos);
        int row = _crs.numRows()+1;
        for (int i=1; i<=_crs.numRows(); ++i) {
            if ((_pos>=_crs.rows(i)) && (_pos<_crs.rows(i+1))) {
                row = i;
                break;
            }
        }
        _value.first = key_type(row, col);
        _value.second = _crs.values(_pos);
    }
}

template <typename T, CRS_Storage Storage>
CRS_ConstIterator<T, Storage>::CRS_ConstIterator(const
                                                 CRS_Iterator<T, Storage> &rhs)
    : _crs(rhs._crs), _pos(rhs._pos)
{
    if (_pos<=_crs.numNonZeros()) {
        int col = _crs.columns(_pos);
        int row = _crs.numRows()+1;
        for (int i=1; i<=_crs.numRows(); ++i) {
            if ((_pos>=_crs.rows(i)) && (_pos<_crs.rows(i+1))) {
                row = i;
                break;
            }
        }
        _value.first = key_type(row, col);
        _value.second = _crs.values(_pos);
    }
}

template <typename T, CRS_Storage Storage>
CRS_ConstIterator<T, Storage>::CRS_ConstIterator(const CRS<T, Storage> &crs,
                                                 int pos)
    : _crs(crs), _pos(pos)
{
    if (pos<=_crs.numNonZeros()) {
        int col = _crs.columns(_pos);
        int row = _crs.numRows()+1;
        for (int i=1; i<=_crs.numRows(); ++i) {
            if ((pos>=_crs.rows(i)) && (pos<_crs.rows(i+1))) {
                row = i;
                break;
            }
        }
        _value.first = key_type(row, col);
        _value.second = _crs.values(_pos);
    }
}

template <typename T, CRS_Storage Storage>
bool
CRS_ConstIterator<T, Storage>::operator==(const CRS_ConstIterator &rhs) const
{
    return ((&_crs==&rhs._crs) && (_pos==rhs._pos));
}

template <typename T, CRS_Storage Storage>
bool
CRS_ConstIterator<T, Storage>::operator!=(const CRS_ConstIterator &rhs) const
{
    return !(*this==rhs);
}


template <typename T, CRS_Storage Storage>
CRS_ConstIterator<T, Storage> &
CRS_ConstIterator<T, Storage>::operator++()
{
    ++_pos;

    if (_pos<=_crs.numNonZeros()) {
        if (_pos>=_crs.rows(_value.first.first+1)) {
            ++_value.first.first;
        }
        _value.first.second = _crs.columns(_pos);
        _value.second = _crs.values(_pos);
    }
    return *this;
}

template <typename T, CRS_Storage Storage>
const typename CRS_ConstIterator<T, Storage>::value_type &
CRS_ConstIterator<T, Storage>::operator*() const
{
    return _value;
}

template <typename T, CRS_Storage Storage>
const typename CRS_ConstIterator<T, Storage>::value_type *
CRS_ConstIterator<T, Storage>::operator->() const
{
    return &_value;
}

//-- CRS_Iterator --------------------------------------------------------------

template <typename T, CRS_Storage Storage>
CRS_Iterator<T, Storage>::CRS_Iterator(const CRS_Iterator &rhs)
    : _crs(rhs._crs), _pos(rhs._pos)
{
    if (_pos<=_crs.numNonZeros()) {
        int col = _crs.columns(_pos);
        int row = _crs.numRows()+1;
        for (int i=1; i<=_crs.numRows(); ++i) {
            if ((_pos>=_crs.rows(i)) && (_pos<_crs.rows(i+1))) {
                row = i;
                break;
            }
        }
        _value.first = key_type(row, col);
        _value.second = _crs.values(_pos);
    }
}

template <typename T, CRS_Storage Storage>
CRS_Iterator<T, Storage>::CRS_Iterator(const CRS<T, Storage> &crs, int pos)
    : _crs(crs), _pos(pos)
{
    if (pos<=_crs.numNonZeros()) {
        int col = _crs.columns(_pos);
        int row = _crs.numRows()+1;
        for (int i=1; i<=_crs.numRows(); ++i) {
            if ((pos>=_crs.rows(i)) && (pos<_crs.rows(i+1))) {
                row = i;
                break;
            }
        }
        _value.first = key_type(row, col);
        _value.second = _crs.values(_pos);
    }
}

template <typename T, CRS_Storage Storage>
bool
CRS_Iterator<T, Storage>::operator==(const CRS_Iterator &rhs) const
{
    return ((&_crs==&rhs._crs) && (_pos==rhs._pos));
}

template <typename T, CRS_Storage Storage>
bool
CRS_Iterator<T, Storage>::operator!=(const CRS_Iterator &rhs) const
{
    return !(*this==rhs);
}

template <typename T, CRS_Storage Storage>
CRS_Iterator<T, Storage> &
CRS_Iterator<T, Storage>::operator++()
{
    ++_pos;

    if (_pos<=_crs.numNonZeros()) {
        if (_pos>=_crs.rows(_value.first.first+1)) {
            ++_value.first.first;
        }
        _value.first.second = _crs.columns(_pos);
        _value.second = _crs.values(_pos);
    }
    return *this;
}

template <typename T, CRS_Storage Storage>
typename CRS_Iterator<T, Storage>::value_type &
CRS_Iterator<T, Storage>::operator*()
{
    return _value;
}

template <typename T, CRS_Storage Storage>
typename CRS_Iterator<T, Storage>::value_type *
CRS_Iterator<T, Storage>::operator->()
{
    return &_value;
}

} // namespace flens
