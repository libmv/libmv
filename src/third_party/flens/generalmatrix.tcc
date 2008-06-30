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

#include <flens/blas_flens.h>

namespace flens {

// == GeMatrix =================================================================

template <typename FS>
GeMatrix<FS>::GeMatrix()
{
}

template <typename FS>
GeMatrix<FS>::GeMatrix(int numRows, int numCols, int firstRow, int firstCol)
    : _fs(numRows, numCols, firstRow, firstCol)
{
}

template <typename FS>
GeMatrix<FS>::GeMatrix(const Range &rows, const Range &cols)
    : _fs(rows.length(), cols.length(), rows.firstIndex(), cols.firstIndex())
{
    assert(rows.stride()==1);
    assert(cols.stride()==1);
}

template <typename FS>
GeMatrix<FS>::GeMatrix(const FS &fs)
    : _fs(fs)
{
}

template <typename FS>
GeMatrix<FS>::GeMatrix(const GeMatrix<FS> &rhs)
    : GeneralMatrix<GeMatrix<FS> >(),
      _fs(rhs._fs)
{
}

template <typename FS>
template <typename RHS>
GeMatrix<FS>::GeMatrix(const GeMatrix<RHS> &rhs)
    : _fs(rhs.engine())
{
}

// -- operators ----------------------------------------------------------------

template <typename FS>
ListInitializerSwitch<GeMatrix<FS> >
GeMatrix<FS>::operator=(const T &value)
{
    return ListInitializerSwitch<GeMatrix<FS> >(*this,
                                                firstRow(),
                                                firstCol(),
                                                value);
}

template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator=(const GeMatrix<FS> &rhs)
{
    copy(rhs, *this);
    return *this;
}

template <typename FS>
template <typename RHS>
GeMatrix<FS> &
GeMatrix<FS>::operator=(const Matrix<RHS> &rhs)
{
    copy(rhs.impl(), *this);
    return *this;
}

template <typename FS>
template <typename SP>
GeMatrix<FS> &
GeMatrix<FS>::operator=(const SparseGeMatrix<SP> &sp)
{
    copy(sp.impl(), *this);
    return *this;
}


template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator+=(const GeMatrix<FS> &rhs)
{
    axpy(T(1), rhs, *this);
    return *this;
}

template <typename FS>
template <typename RHS>
GeMatrix<FS> &
GeMatrix<FS>::operator+=(const Matrix<RHS> &rhs)
{
    axpy(T(1), rhs.impl(), *this);
    return *this;
}

template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator-=(const GeMatrix<FS> &rhs)
{
    axpy(T(-1), rhs, *this);
    return *this;
}

template <typename FS>
template <typename RHS>
GeMatrix<FS> &
GeMatrix<FS>::operator-=(const Matrix<RHS> &rhs)
{
    axpy(T(-1), rhs.impl(), *this);
    return *this;
}

template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator+=(T alpha)
{
    for (int i=0; i < ( numRows()*numCols() ); ++i) {
        this->data()[i] += alpha;
    }
    return *this;
}

template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator-=(T alpha)
{
    for (int i=0; i < ( numRows()*numCols() ); ++i) {
        this->data()[i] -= alpha;
    }
    return *this;
}

template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator*=(T alpha)
{
    scal(alpha, *this);
    return *this;
}

template <typename FS>
GeMatrix<FS> &
GeMatrix<FS>::operator/=(T alpha)
{
    scal(T(1)/alpha, *this);
    return *this;
}

template <typename FS>
inline
const typename GeMatrix<FS>::T &
GeMatrix<FS>::operator()(int row, int col) const
{
    return _fs(row, col);
}

template <typename FS>
inline
typename GeMatrix<FS>::T &
GeMatrix<FS>::operator()(int row, int col)
{
    return _fs(row, col);
}

// -- views --------------------------------------------------------------------

// rectangular view

template <typename FS>
const typename GeMatrix<FS>::ConstView
GeMatrix<FS>::operator()(const Range &rows, const Range &cols,
                         int firstViewRow, int firstViewCol) const
{
    assert(rows.stride()==1);
    assert(cols.stride()==1);

    return _fs.view(rows.firstIndex(), cols.firstIndex(),
                    rows.lastIndex(), cols.lastIndex(),
                    firstViewRow, firstViewCol);
}

template <typename FS>
typename GeMatrix<FS>::View
GeMatrix<FS>::operator()(const Range &rows, const Range &cols,
                         int firstViewRow, int firstViewCol)
{
    assert(rows.stride()==1);
    assert(cols.stride()==1);

    return _fs.view(rows.firstIndex(), cols.firstIndex(),
                    rows.lastIndex(), cols.lastIndex(),
                    firstViewRow, firstViewCol);
}

template <typename FS>
const typename GeMatrix<FS>::ConstView
GeMatrix<FS>::operator()(const Range &rows, const Underscore &allCols,
                         int firstViewRow, int firstViewCol) const
{
    assert(rows.stride()==1);

    return _fs.view(rows.firstIndex(), this->firstCol(),
                    rows.lastIndex(), this->lastCol(),
                    firstViewRow, firstViewCol);
}

template <typename FS>
typename GeMatrix<FS>::View
GeMatrix<FS>::operator()(const Range &rows, const Underscore &allCols,
                         int firstViewRow, int firstViewCol)
{
    assert(rows.stride()==1);

    return _fs.view(rows.firstIndex(), this->firstCol(),
                    rows.lastIndex(), this->lastCol(),
                    firstViewRow, firstViewCol);
}

template <typename FS>
const typename GeMatrix<FS>::ConstView
GeMatrix<FS>::operator()(const Underscore &allRows, const Range &cols,
                         int firstViewRow, int firstViewCol) const
{
    assert(cols.stride()==1);

    return _fs.view(this->firstRow(), cols.firstIndex(),
                    this->lastRow(), cols.lastIndex(),
                    firstViewRow, firstViewCol);
}

template <typename FS>
typename GeMatrix<FS>::View
GeMatrix<FS>::operator()(const Underscore &allRows, const Range &cols,
                         int firstViewRow, int firstViewCol)
{
    assert(cols.stride()==1);

    return _fs.view(this->firstRow(), cols.firstIndex(),
                    this->lastRow(), cols.lastIndex(),
                    firstViewRow, firstViewCol);
}

// row views
template <typename FS>
const typename GeMatrix<FS>::ConstVectorView
GeMatrix<FS>::operator()(int row, const Range &cols,
                         int firstViewIndex) const
{
    return _fs.viewRow(row,
                       cols.firstIndex(), cols.lastIndex(),
                       firstViewIndex);
}

template <typename FS>
typename GeMatrix<FS>::VectorView
GeMatrix<FS>::operator()(int row, const Range &cols,
                         int firstViewIndex)
{
    return _fs.viewRow(row,
                       cols.firstIndex(), cols.lastIndex(),
                       firstViewIndex);
}

template <typename FS>
const typename GeMatrix<FS>::ConstVectorView
GeMatrix<FS>::operator()(int row, const Underscore &allCols,
                         int firstViewIndex) const
{
    return _fs.viewRow(row, firstViewIndex);
}

template <typename FS>
typename GeMatrix<FS>::VectorView
GeMatrix<FS>::operator()(int row, const Underscore &allCols,
                         int firstViewIndex)
{
    return _fs.viewRow(row, firstViewIndex);
}

// col views
template <typename FS>
const typename GeMatrix<FS>::ConstVectorView
GeMatrix<FS>::operator()(const Range &rows, int col,
                         int firstViewIndex) const
{
    return _fs.viewCol(col,
                       rows.firstIndex(), rows.lastIndex(),
                       firstViewIndex);
}

template <typename FS>
typename GeMatrix<FS>::VectorView
GeMatrix<FS>::operator()(const Range &rows, int col,
                         int firstViewIndex)
{
    return _fs.viewCol(col,
                       rows.firstIndex(), rows.lastIndex(),
                       firstViewIndex);
}

template <typename FS>
const typename GeMatrix<FS>::ConstVectorView
GeMatrix<FS>::operator()(const Underscore &allRows, int col,
                         int firstViewIndex) const
{
    return _fs.viewCol(col, firstViewIndex);
}

template <typename FS>
typename GeMatrix<FS>::VectorView
GeMatrix<FS>::operator()(const Underscore &allRows, int col,
                         int firstViewIndex)
{
    return _fs.viewCol(col, firstViewIndex);
}

// diag views
template <typename FS>
const typename GeMatrix<FS>::ConstVectorView
GeMatrix<FS>::diag(int d) const
{
    return _fs.viewDiag(d);
}

template <typename FS>
typename GeMatrix<FS>::VectorView
GeMatrix<FS>::diag(int d)
{
    return _fs.viewDiag(d);
}

// triangular views
template <typename FS>
typename GeMatrix<FS>::ConstTriangularView
GeMatrix<FS>::upper(int firstViewRow, int firstViewCol) const
{
    assert(numRows()==numCols());

    ConstFSView view = _fs.view(firstRow(), firstCol(),
                                lastRow(), lastCol(),
                                firstViewRow, firstViewCol);
    return ConstTriangularView(view, Upper, NonUnit);
}

template <typename FS>
typename GeMatrix<FS>::TriangularView
GeMatrix<FS>::upper(int firstViewRow, int firstViewCol)
{
    assert(numRows()==numCols());

    FSView view = _fs.view(firstRow(), firstCol(),
                           lastRow(), lastCol(),
                           firstViewRow, firstViewCol);
    return TriangularView(view, Upper, NonUnit);
}

template <typename FS>
typename GeMatrix<FS>::ConstTriangularView
GeMatrix<FS>::upperUnit(int firstViewRow, int firstViewCol) const
{
    assert(numRows()==numCols());

    ConstFSView view = _fs.view(firstRow(), firstCol(),
                                lastRow(), lastCol(),
                                firstViewRow, firstViewCol);
    return ConstTriangularView(view, Upper, Unit);
}

template <typename FS>
typename GeMatrix<FS>::TriangularView
GeMatrix<FS>::upperUnit(int firstViewRow, int firstViewCol)
{
    assert(numRows()==numCols());

    FSView view = _fs.view(firstRow(), firstCol(),
                           lastRow(), lastCol(),
                           firstViewRow, firstViewCol);
    return TriangularView(view, Upper, NonUnit);
}

template <typename FS>
typename GeMatrix<FS>::ConstTriangularView
GeMatrix<FS>::lower(int firstViewRow, int firstViewCol) const
{
    assert(numRows()==numCols());

    ConstFSView view = _fs.view(firstRow(), firstCol(),
                                lastRow(), lastCol(),
                                firstViewRow, firstViewCol);
    return ConstTriangularView(view, Lower, NonUnit);
}

template <typename FS>
typename GeMatrix<FS>::TriangularView
GeMatrix<FS>::lower(int firstViewRow, int firstViewCol)
{
    assert(numRows()==numCols());

    FSView view = _fs.view(firstRow(), firstCol(),
                           lastRow(), lastCol(),
                           firstViewRow, firstViewCol);
    return TriangularView(view, Lower, NonUnit);
}

template <typename FS>
typename GeMatrix<FS>::ConstTriangularView
GeMatrix<FS>::lowerUnit(int firstViewRow, int firstViewCol) const
{
    assert(numRows()==numCols());

    ConstFSView view = _fs.view(firstRow(), firstCol(),
                                lastRow(), lastCol(),
                                firstViewRow, firstViewCol);
    return ConstTriangularView(view, Lower, Unit);
}

template <typename FS>
typename GeMatrix<FS>::TriangularView
GeMatrix<FS>::lowerUnit(int firstViewRow, int firstViewCol)
{
    assert(numRows()==numCols());

    FSView view = _fs.view(firstRow(), firstCol(),
                           lastRow(), lastCol(),
                           firstViewRow, firstViewCol);
    return TriangularView(view, Lower, Unit);
}

// -- methods ------------------------------------------------------------------

// for BLAS/LAPACK
template <typename FS>
int
GeMatrix<FS>::numRows() const
{
    return _fs.numRows();
}

template <typename FS>
int
GeMatrix<FS>::numCols() const
{
    return _fs.numCols();
}

template <typename FS>
int
GeMatrix<FS>::leadingDimension() const
{
    return _fs.leadingDimension();
}

template <typename FS>
const typename GeMatrix<FS>::T *
GeMatrix<FS>::data() const
{
    return _fs.data();
}

template <typename FS>
typename GeMatrix<FS>::T *
GeMatrix<FS>::data()
{
    return _fs.data();
}

//for element access
template <typename FS>
int
GeMatrix<FS>::firstRow() const
{
    return _fs.firstRow();
}

template <typename FS>
int
GeMatrix<FS>::lastRow() const
{
    return _fs.lastRow();
}

template <typename FS>
int
GeMatrix<FS>::firstCol() const
{
    return _fs.firstCol();
}

template <typename FS>
int
GeMatrix<FS>::lastCol() const
{
    return _fs.lastCol();
}

template <typename FS>
Range
GeMatrix<FS>::rows()  const
{
    return _(firstRow(), lastRow());
}

template <typename FS>
Range
GeMatrix<FS>::cols() const
{
    return _(firstCol(), lastCol());
}

template <typename FS>
void
GeMatrix<FS>::resize(int numRows, int numCols, int firstRow, int firstCol)
{
    _fs.resize(numRows, numCols, firstRow, firstCol);
}

template <typename FS>
void
GeMatrix<FS>::resize(const Range &rows, const Range &cols)
{
    _fs.resize(rows.length(), cols.length(),
               rows.firstIndex(), cols.firstIndex());
}

template <typename FS>
void
GeMatrix<FS>::resizeOrClear(int numRows, int numCols, int firstRow, int firstCol)
{
    _fs.resizeOrClear(numRows, numCols, firstRow, firstCol);
}

template <typename FS>
void
GeMatrix<FS>::shiftIndex(int rowAmount, int colAmount)
{
    _fs.shiftIndexTo(firstRow()+rowAmount, firstCol()+colAmount);
}

template <typename FS>
void
GeMatrix<FS>::shiftIndexTo(int firstRow, int firstCol)
{
    _fs.shiftIndexTo(firstRow, firstCol);
}

// -- implementation -----------------------------------------------------------

template <typename FS>
const FS &
GeMatrix<FS>::engine() const
{
    return _fs;
}

template <typename FS>
FS &
GeMatrix<FS>::engine()
{
    return _fs;
}

// == GbMatrix =================================================================

template <typename BS>
GbMatrix<BS>::GbMatrix()
{
}

template <typename BS>
GbMatrix<BS>::GbMatrix(int numRows, int numCols,
                       int numSubDiags, int numSuperDiags,
                       int indexBase)
    : _bs(numRows, numCols, numSubDiags, numSuperDiags, indexBase)
{
}

template <typename BS>
GbMatrix<BS>::GbMatrix(const BS &bs)
    : _bs(bs)
{
}

template <typename BS>
GbMatrix<BS>::GbMatrix(const GbMatrix<BS> &rhs)
    : _bs(rhs._bs)
{
}

template <typename BS>
template <typename RHS>
GbMatrix<BS>::GbMatrix(const GbMatrix<RHS> &rhs)
    : _bs(rhs.engine())
{
}

// -- operators ----------------------------------------------------------------

template <typename BS>
GbMatrix<BS> &
GbMatrix<BS>::operator=(const GbMatrix<BS> &rhs)
{
    copy(rhs, *this);
    return *this;
}

template <typename BS>
template <typename RHS>
GbMatrix<BS> &
GbMatrix<BS>::operator=(const Matrix<RHS> &rhs)
{
    copy(rhs.impl(), *this);
    return *this;
}

template <typename BS>
GbMatrix<BS> &
GbMatrix<BS>::operator+=(const GbMatrix<BS> &rhs)
{
    axpy(T(1), rhs, *this);
    return *this;
}

template <typename BS>
template <typename RHS>
GbMatrix<BS> &
GbMatrix<BS>::operator+=(const Matrix<RHS> &rhs)
{
    axpy(T(1), rhs.impl(), *this);
    return *this;
}

template <typename BS>
GbMatrix<BS> &
GbMatrix<BS>::operator-=(const GbMatrix<BS> &rhs)
{
    axpy(T(-1), rhs, *this);
    return *this;
}

template <typename BS>
template <typename RHS>
GbMatrix<BS> &
GbMatrix<BS>::operator-=(const Matrix<RHS> &rhs)
{
    axpy(T(-1), rhs.impl(), *this);
    return *this;
}

template <typename BS>
GbMatrix<BS> &
GbMatrix<BS>::operator*=(T alpha)
{
    scal(alpha, *this);
    return *this;
}

template <typename BS>
GbMatrix<BS> &
GbMatrix<BS>::operator/=(T alpha)
{
    scal(T(1)/alpha, *this);
    return *this;
}

template <typename BS>
inline
const typename GbMatrix<BS>::T &
GbMatrix<BS>::operator()(int row, int col) const
{
    return _bs(row, col);
}

template <typename BS>
inline
typename GbMatrix<BS>::T &
GbMatrix<BS>::operator()(int row, int col)
{
    return _bs(row, col);
}

// -- views --------------------------------------------------------------------

// view of one diagonal
template <typename BS>
typename GbMatrix<BS>::ConstVectorView
GbMatrix<BS>::diag(int d) const
{
    return _bs.viewDiag(d);
}

template <typename BS>
typename GbMatrix<BS>::VectorView
GbMatrix<BS>::diag(int d)
{
    return _bs.viewDiag(d);
}

// view of bands of diagonal
template <typename BS>
typename GbMatrix<BS>::ConstView
GbMatrix<BS>::diags(int fromDiag, int toDiag) const
{
    return _bs.viewDiags(fromDiag, toDiag);
}

template <typename BS>
typename GbMatrix<BS>::View
GbMatrix<BS>::diags(int fromDiag, int toDiag)
{
    return _bs.viewDiags(fromDiag, toDiag);
}

template <typename BS>
typename GbMatrix<BS>::ConstView
GbMatrix<BS>::diags(const Range &range) const
{
    assert(range.stride()==1);

    return _bs.viewDiags(range.firstIndex(), range.lastIndex());
}

template <typename BS>
typename GbMatrix<BS>::View
GbMatrix<BS>::diags(const Range &range)
{
    assert(range.stride()==1);

    return _bs.viewDiags(range.firstIndex(), range.lastIndex());
}

// triangular views
template <typename BS>
typename GbMatrix<BS>::ConstTriangularView
GbMatrix<BS>::upper(int viewIndex) const
{
    assert(numRows()==numCols());

    ConstBSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(),
                                     viewIndex);
    return ConstTriangularView(view, Upper, NonUnit);
}

template <typename BS>
typename GbMatrix<BS>::TriangularView
GbMatrix<BS>::upper(int viewIndex)
{
    assert(numRows()==numCols());

    BSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(), viewIndex);
    return TriangularView(view, Upper, NonUnit);
}

template <typename BS>
typename GbMatrix<BS>::ConstTriangularView
GbMatrix<BS>::upperUnit(int viewIndex) const
{
    assert(numRows()==numCols());

    ConstBSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(),
                                     viewIndex);
    return ConstTriangularView(view, Upper, Unit);
}

template <typename BS>
typename GbMatrix<BS>::TriangularView
GbMatrix<BS>::upperUnit(int viewIndex)
{
    assert(numRows()==numCols());

    BSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(), viewIndex);
    return TriangularView(view, Upper, Unit);
}

template <typename BS>
typename GbMatrix<BS>::ConstTriangularView
GbMatrix<BS>::lower(int viewIndex) const
{
    assert(numRows()==numCols());

    ConstBSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(),
                                     viewIndex);
    return ConstTriangularView(view, Lower, NonUnit);
}

template <typename BS>
typename GbMatrix<BS>::TriangularView
GbMatrix<BS>::lower(int viewIndex)
{
    assert(numRows()==numCols());

    BSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(), viewIndex);
    return TriangularView(view, Lower, NonUnit);
}

template <typename BS>
typename GbMatrix<BS>::ConstTriangularView
GbMatrix<BS>::lowerUnit(int viewIndex) const
{
    assert(numRows()==numCols());

    ConstBSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(),
                                     viewIndex);
    return ConstTriangularView(view, Lower, Unit);
}

template <typename BS>
typename GbMatrix<BS>::TriangularView
GbMatrix<BS>::lowerUnit(int viewIndex)
{
    assert(numRows()==numCols());

    BSView view = _bs.viewDiags(-numSubDiags(), numSuperDiags(), viewIndex);
    return TriangularView(view, Lower, Unit);
}

// -- methods ------------------------------------------------------------------

// for BLAS/LAPCK
template <typename BS>
int
GbMatrix<BS>::numRows() const
{
    return _bs.numRows();
}

template <typename BS>
int
GbMatrix<BS>::numCols() const
{
    return _bs.numCols();
}

template <typename BS>
int
GbMatrix<BS>::numSubDiags() const
{
    return _bs.numSubDiags();
}

template <typename BS>
int
GbMatrix<BS>::numSuperDiags() const
{
    return _bs.numSuperDiags();
}

template <typename BS>
int
GbMatrix<BS>::leadingDimension() const
{
    return _bs.leadingDimension();
}

template <typename BS>
const typename GbMatrix<BS>::T *
GbMatrix<BS>::data() const
{
    return _bs.data();
}

template <typename BS>
typename GbMatrix<BS>::T *
GbMatrix<BS>::data()
{
    return _bs.data();
}

// for element access
template <typename BS>
int
GbMatrix<BS>::firstRow() const
{
    return _bs.firstRow();
}

template <typename BS>
int
GbMatrix<BS>::lastRow() const
{
    return _bs.lastRow();
}

template <typename BS>
int
GbMatrix<BS>::firstCol() const
{
    return _bs.firstCol();
}

template <typename BS>
int
GbMatrix<BS>::lastCol() const
{
    return _bs.lastCol();
}

template <typename BS>
Range
GbMatrix<BS>::rowIndices()  const
{
    return _(firstRow(), lastRow());
}

template <typename BS>
Range
GbMatrix<BS>::colIndices()  const
{
    return _(firstCol(), lastCol());
}

template <typename BS>
Range
GbMatrix<BS>::diags() const
{
    return _(-numSubDiags(), numSuperDiags());
}

// -- implementation -----------------------------------------------------------

template <typename BS>
const BS &
GbMatrix<BS>::engine() const
{
    return _bs;
}

template <typename BS>
BS &
GbMatrix<BS>::engine()
{
    return _bs;
}

} // namespace flens
