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

namespace flens {

template <typename T, StorageOrder Order>
ConstBandStorageView<T,Order>::ConstBandStorageView(
                                  const ConstFullStorageView<T, ColMajor> &data,
                                  int numRows, int numCols, int indexBase)
    : _numRows(numRows), _numCols(numCols), _indexBase(indexBase), _data(data)
{
}

template <typename T, StorageOrder Order>
ConstBandStorageView<T,Order>::ConstBandStorageView(const ConstView &rhs)
    : _numRows(rhs._numRows), _numCols(rhs._numCols),
      _indexBase(rhs._indexBase), _data(rhs._data)
{
}

template <typename T, StorageOrder Order>
ConstBandStorageView<T,Order>::~ConstBandStorageView()
{
}

template <typename T, StorageOrder Order>
const T &
ConstBandStorageView<T,Order>::operator()(int row, int col) const
{
    if (Order==RowMajor) {
        return _data(col-row, row);
    }
    return _data(row-col, col);
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::firstRow() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::lastRow() const
{
    return _indexBase+_numRows-1;
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::firstCol() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::lastCol() const
{
    return _indexBase+_numCols-1;
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::numRows() const
{
    return _numRows;
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::numCols() const
{
    return _numCols;
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::numSubDiags() const
{
    if (Order==RowMajor) {
        return -_data.firstRow();
    }
    return _data.lastRow();
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::numSuperDiags() const
{
    if (Order==RowMajor) {
        return _data.lastRow();
    }
    return -_data.firstRow();
}

template <typename T, StorageOrder Order>
int
ConstBandStorageView<T,Order>::leadingDimension() const
{
    return _data.leadingDimension();
}

template <typename T, StorageOrder Order>
const T *
ConstBandStorageView<T,Order>::data() const
{
    return _data.data();
}

template <typename T, StorageOrder Order>
ConstArrayView<T>
ConstBandStorageView<T,Order>::viewDiag(int diag, int viewIndexBase) const
{
    if (Order==RowMajor) {
        int from = std::max(0,diag) + _data.firstCol();
        int length = (diag<=0) ? std::min(_numCols, _numRows+diag)
                               : std::min(_numCols-diag, _numRows);

        return _data.viewRow(diag, from, from+length-1, viewIndexBase);
    }

    int from = std::max(0,diag) + _data.firstCol();
    int length = (diag>=0) ? std::min(_numRows, _numCols-diag)
                           : std::min(_numRows+diag, _numCols);

    return _data.viewRow(-diag, from, from+length-1, viewIndexBase);
}

template <typename T, StorageOrder Order>
ConstBandStorageView<T, Order>
ConstBandStorageView<T, Order>::viewDiags(int fromDiag, int toDiag,
                                          int viewIndexBase) const
{
    assert(fromDiag<=0);
    assert(toDiag>=0);

    if (Order==RowMajor) {
        return ConstBandStorageView<T, Order>(
                          _data.view(fromDiag, _data.firstCol(),
                                     toDiag, _data.lastCol(),
                                     fromDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
    }
    return ConstBandStorageView<T, Order>(
                          _data.view(-toDiag, _data.firstCol(),
                                     -fromDiag, _data.lastCol(),
                                     -toDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
}

//==============================================================================

template <typename T, StorageOrder Order>
BandStorageView<T,Order>::BandStorageView(
                                      const FullStorageView<T, ColMajor> &data,
                                      int numRows, int numCols, int indexBase)
    : _numRows(numRows), _numCols(numCols), _indexBase(indexBase), _data(data)
{
}

template <typename T, StorageOrder Order>
BandStorageView<T,Order>::BandStorageView(const BandStorageView<T, Order> &rhs)
    : _numRows(rhs._numRows), _numCols(rhs._numCols),
      _indexBase(rhs._indexBase), _data(rhs._data)
{
}

template <typename T, StorageOrder Order>
BandStorageView<T,Order>::~BandStorageView()
{
}

template <typename T, StorageOrder Order>
BandStorageView<T, Order> &
BandStorageView<T,Order>::operator=(const BandStorage<T, Order> &rhs)
{
    assert(_numRows==rhs._numRows);
    assert(_numCols==rhs._numCols);
    assert(numSubDiags()==rhs.numSubDiags());
    assert(numSuperDiags()==rhs.numSuperDiags());

    _data = rhs.data;
    return *this;
}

template <typename T, StorageOrder Order>
BandStorageView<T, Order> &
BandStorageView<T,Order>::operator=(const BandStorageView<T, Order> &rhs)
{
    assert(_numRows==rhs._numRows);
    assert(_numCols==rhs._numCols);
    assert(numSubDiags()==rhs.numSubDiags());
    assert(numSuperDiags()==rhs.numSuperDiags());

    if (this!=&rhs) {
        _data = rhs._data;
    }
    return *this;
}

template <typename T, StorageOrder Order>
BandStorageView<T, Order> &
BandStorageView<T,Order>::operator=(const ConstBandStorageView<T, Order> &rhs)
{
    assert(_numRows==rhs._numRows);
    assert(_numCols==rhs._numCols);
    assert(numSubDiags()==rhs.numSubDiags());
    assert(numSuperDiags()==rhs.numSuperDiags());

    _data = rhs.data;
    return *this;
}

template <typename T, StorageOrder Order>
const T &
BandStorageView<T,Order>::operator()(int row, int col) const
{
    if (Order==RowMajor) {
        return _data(col-row, row);
    }
    return _data(row-col, col);
}

template <typename T, StorageOrder Order>
T &
BandStorageView<T,Order>::operator()(int row, int col)
{
    if (Order==RowMajor) {
        return _data(col-row, row);
    }
    return _data(row-col, col);
}

template <typename T, StorageOrder Order>
BandStorageView<T,Order>::operator ConstView() const
{
    int fromDiag = -numSubDiags(),
        toDiag = numSuperDiags(),
        viewIndexBase = _indexBase;

    if (Order==RowMajor) {
        return ConstBandStorageView<T, Order>(
                          _data.view(fromDiag, _data.firstCol(),
                                     toDiag, _data.lastCol(),
                                     fromDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
    }
    return ConstBandStorageView<T, Order>(
                          _data.view(-toDiag, _data.firstCol(),
                                     -fromDiag, _data.lastCol(),
                                     -toDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::firstRow() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::lastRow() const
{
    return _indexBase+_numRows-1;
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::firstCol() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::lastCol() const
{
    return _indexBase+_numCols-1;
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::numRows() const
{
    return _numRows;
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::numCols() const
{
    return _numCols;
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::numSubDiags() const
{
    if (Order==RowMajor) {
        return -_data.firstRow();
    }
    return _data.lastRow();
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::numSuperDiags() const
{
    if (Order==RowMajor) {
        return _data.lastRow();
    }
    return -_data.firstRow();
}

template <typename T, StorageOrder Order>
int
BandStorageView<T,Order>::leadingDimension() const
{
    return _data.leadingDimension();
}

template <typename T, StorageOrder Order>
const T *
BandStorageView<T,Order>::data() const
{
    return _data.data();
}

template <typename T, StorageOrder Order>
T *
BandStorageView<T,Order>::data()
{
    return _data.data();
}

template <typename T, StorageOrder Order>
void
BandStorageView<T,Order>::resize(int numRows, int numCols,
                                 int numSubDiags, int numSuperDiags,
                                 int indexBase)
{
    assert(numRows==this->numRows());
    assert(numCols==this->numCols());
    assert(numSubDiags==this->numSubDiags());
    assert(numSuperDiags==this->numSuperDiags());
    _indexBase = indexBase;
}

template <typename T, StorageOrder Order>
ConstArrayView<T>
BandStorageView<T,Order>::viewDiag(int diag, int viewIndexBase) const
{
    if (Order==RowMajor) {
        int from = std::max(0,diag) + _data.firstCol();
        int length = (diag<=0) ? std::min(_numCols, _numRows+diag)
                               : std::min(_numCols-diag, _numRows);

        return _data.viewRow(diag, from, from+length-1, viewIndexBase);
    }

    int from = std::max(0,diag) + _data.firstCol();
    int length = (diag>=0) ? std::min(_numRows, _numCols-diag)
                           : std::min(_numRows+diag, _numCols);

    return _data.viewRow(-diag, from, from+length-1, viewIndexBase);
}

template <typename T, StorageOrder Order>
ArrayView<T>
BandStorageView<T,Order>::viewDiag(int diag, int viewIndexBase)
{
    if (Order==RowMajor) {
        int from = std::max(0,diag) + _data.firstCol();
        int length = (diag<=0) ? std::min(_numCols, _numRows+diag)
                               : std::min(_numCols-diag, _numRows);

        return _data.viewRow(diag, from, from+length-1, viewIndexBase);
    }

    int from = std::max(0,diag) + _data.firstCol();
    int length = (diag>=0) ? std::min(_numRows, _numCols-diag)
                           : std::min(_numRows+diag, _numCols);

    return _data.viewRow(-diag, from, from+length-1, viewIndexBase);
}

template <typename T, StorageOrder Order>
ConstBandStorageView<T, Order>
BandStorageView<T, Order>::viewDiags(int fromDiag, int toDiag,
                                     int viewIndexBase) const
{
    assert(fromDiag<=0);
    assert(toDiag>=0);

    if (Order==RowMajor) {
        return ConstBandStorageView<T, Order>(
                          _data.view(fromDiag, _data.firstCol(),
                                     toDiag, _data.lastCol(),
                                     fromDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
    }
    return ConstBandStorageView<T, Order>(
                          _data.view(-toDiag, _data.firstCol(),
                                     -fromDiag, _data.lastCol(),
                                     -toDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
}

template <typename T, StorageOrder Order>
BandStorageView<T, Order>
BandStorageView<T, Order>::viewDiags(int fromDiag, int toDiag,
                                     int viewIndexBase)
{
    assert(fromDiag<=0);
    assert(toDiag>=0);

    if (Order==RowMajor) {
        return BandStorageView<T, Order>(
                          _data.view(fromDiag, _data.firstCol(),
                                     toDiag, _data.lastCol(),
                                     fromDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
    }
    return BandStorageView<T, Order>(
                          _data.view(-toDiag, _data.firstCol(),
                                     -fromDiag, _data.lastCol(),
                                     -toDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
}

//==============================================================================

template <typename T, StorageOrder Order>
BandStorage<T, Order>::BandStorage()
    : _numRows(0), _numCols(0), _indexBase(1)
{
}

template <typename T, StorageOrder Order>
BandStorage<T, Order>::BandStorage(int numRows, int numCols,
                                   int numSubDiags, int numSuperDiags,
                                   int indexBase)
    : _numRows(numRows), _numCols(numCols), _indexBase(indexBase),
      _data(numSubDiags+numSuperDiags+1,
            (Order==ColMajor) ? numCols : numRows,
            (Order==ColMajor) ? -numSuperDiags : -numSubDiags,
            indexBase)
{
}

template <typename T, StorageOrder Order>
BandStorage<T, Order>::BandStorage(const BandStorage<T, Order> &rhs)
    : _numRows(rhs._numRows), _numCols(rhs._numCols),_indexBase(rhs._indexBase),
      _data(rhs._data)
{
}

template <typename T, StorageOrder Order>
BandStorage<T, Order>::~BandStorage()
{
}

template <typename T, StorageOrder Order>
BandStorage<T, Order> &
BandStorage<T, Order>::operator=(const BandStorage<T, Order> &rhs)
{
    _numRows = rhs._numRows;
    _numCols = rhs._numCols;
    _indexBase = rhs._indexBase;
    _data = rhs._data;
}

template <typename T, StorageOrder Order>
const T &
BandStorage<T, Order>::operator()(int row, int col) const
{
    if (Order==RowMajor) {
        return _data(col-row, row);
    }
    return _data(row-col, col);
}

template <typename T, StorageOrder Order>
T &
BandStorage<T, Order>::operator()(int row, int col)
{
    if (Order==RowMajor) {
        return _data(col-row, row);
    }
    return _data(row-col, col);
}

template <typename T, StorageOrder Order>
int
BandStorage<T,Order>::firstRow() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order>
int
BandStorage<T,Order>::lastRow() const
{
    return _indexBase+_numRows-1;
}

template <typename T, StorageOrder Order>
int
BandStorage<T,Order>::firstCol() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order>
int
BandStorage<T,Order>::lastCol() const
{
    return _indexBase+_numCols-1;
}

template <typename T, StorageOrder Order>
int
BandStorage<T, Order>::numRows() const
{
    return _numRows;
}

template <typename T, StorageOrder Order>
int
BandStorage<T, Order>::numCols() const
{
    return _numCols;
}

template <typename T, StorageOrder Order>
int
BandStorage<T, Order>::numSubDiags() const
{
    if (Order==RowMajor) {
        return -_data.firstRow();
    }
    return _data.lastRow();
}

template <typename T, StorageOrder Order>
int
BandStorage<T, Order>::numSuperDiags() const
{
    if (Order==RowMajor) {
        return _data.lastRow();
    }
    return -_data.firstRow();
}

template <typename T, StorageOrder Order>
int
BandStorage<T, Order>::leadingDimension() const
{
    return _data.leadingDimension();
}

template <typename T, StorageOrder Order>
const T *
BandStorage<T, Order>::data() const
{
    return _data.data();
}

template <typename T, StorageOrder Order>
T *
BandStorage<T, Order>::data()
{
    return _data.data();
}

template <typename T, StorageOrder Order>
void
BandStorage<T, Order>::resize(int numRows, int numCols,
                              int numSubDiags, int numSuperDiags,
                                 int indexBase)
{
    _numRows = numRows;
    _numCols = numCols;
    _indexBase = indexBase;
    if (Order==RowMajor) {
        return _data.resize(numSubDiags+numSuperDiags+1, numRows,
                            -numSubDiags, indexBase);
    }
    return _data.resize(numSubDiags+numSuperDiags+1, numCols,
                        -numSuperDiags, indexBase);
}

template <typename T, StorageOrder Order>
ConstArrayView<T>
BandStorage<T, Order>::viewDiag(int diag, int viewIndexBase) const
{
    if (Order==RowMajor) {
        int from = std::max(0,-diag) + _data.firstCol();
        int length = (diag<=0) ? std::min(_numCols, _numRows+diag)
                               : std::min(_numCols-diag, _numRows);

        return _data.viewRow(diag, from, from+length-1, viewIndexBase);
    }

    int from = -std::min(0,-diag) + _data.firstCol();
    int length = (diag>=0) ? std::min(_numRows, _numCols-diag)
                           : std::min(_numRows+diag, _numCols);
    return _data.viewRow(-diag, from, from+length-1, viewIndexBase);
}

template <typename T, StorageOrder Order>
ArrayView<T>
BandStorage<T, Order>::viewDiag(int diag, int viewIndexBase)
{
    if (Order==RowMajor) {
        int from = std::max(0,-diag) + _data.firstCol();
        int length = (diag<=0) ? std::min(_numCols, _numRows+diag)
                               : std::min(_numCols-diag, _numRows);

        return _data.viewRow(diag, from, from+length-1, viewIndexBase);
    }

    int from = -std::min(0,-diag) + _data.firstCol();
    int length = (diag>=0) ? std::min(_numRows, _numCols-diag)
                           : std::min(_numRows+diag, _numCols);
    return _data.viewRow(-diag, from, from+length-1, viewIndexBase);
}

template <typename T, StorageOrder Order>
ConstBandStorageView<T, Order>
BandStorage<T, Order>::viewDiags(int fromDiag, int toDiag,
                                 int viewIndexBase) const
{
    assert(fromDiag<=0);
    assert(toDiag>=0);

    if (Order==RowMajor) {
        return ConstBandStorageView<T, Order>(
                          _data.view(fromDiag, _data.firstCol(),
                                     toDiag, _data.lastCol(),
                                     fromDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
    }
    return ConstBandStorageView<T, Order>(
                          _data.view(-toDiag, _data.firstCol(),
                                     -fromDiag, _data.lastCol(),
                                     -toDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
}

template <typename T, StorageOrder Order>
BandStorageView<T, Order>
BandStorage<T, Order>::viewDiags(int fromDiag, int toDiag, int viewIndexBase)
{
    assert(fromDiag<=0);
    assert(toDiag>=0);

    if (Order==RowMajor) {
        return BandStorageView<T, Order>(
                          _data.view(fromDiag, _data.firstCol(),
                                     toDiag, _data.lastCol(),
                                     fromDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
    }
    return BandStorageView<T, Order>(
                          _data.view(-toDiag, _data.firstCol(),
                                     -fromDiag, _data.lastCol(),
                                     -toDiag, viewIndexBase),
                          _numRows, _numCols, viewIndexBase);
}

} // namespace flens
