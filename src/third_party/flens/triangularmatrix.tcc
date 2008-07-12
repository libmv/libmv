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

#include <flens/underscore.h>

namespace flens {

// == TrMatrix =================================================================

template <typename FS>
TrMatrix<FS>::TrMatrix()
{
}

template <typename FS>
TrMatrix<FS>::TrMatrix(int dim, StorageUpLo upLo, UnitDiag unitDiag,
                       int firstIndex)
    : _fs(dim, dim, firstIndex, firstIndex), _upLo(upLo), _unitDiag(unitDiag)
{
}

template <typename FS>
TrMatrix<FS>::TrMatrix(const FS &fs, StorageUpLo upLo, UnitDiag unitDiag)
    : _fs(fs), _upLo(upLo), _unitDiag(unitDiag)
{
}

template <typename FS>
TrMatrix<FS>::TrMatrix(const TrMatrix<FS> &rhs)
    : TriangularMatrix<TrMatrix<FS> >(),
      _fs(rhs.engine()), _upLo(rhs.upLo()), _unitDiag(rhs.unitDiag())
{
}

template <typename FS>
template <typename RHS>
TrMatrix<FS>::TrMatrix(const TrMatrix<RHS> &rhs)
    : _fs(rhs.engine()), _upLo(rhs.upLo()), _unitDiag(rhs.unitDiag())
{
}

// -- operators ----------------------------------------------------------------

template <typename FS>
const typename TrMatrix<FS>::T &
TrMatrix<FS>::operator()(int row, int col) const
{
    assert((_upLo==Upper) ? (col>=row) : (col<=row));
    assert(!((_unitDiag==Unit) && (col==row)));

    return _fs(row, col);
}

template <typename FS>
typename TrMatrix<FS>::T &
TrMatrix<FS>::operator()(int row, int col)
{
    assert((_upLo==Upper) ? (col>=row) : (col<=row));
    assert(!((_unitDiag==Unit) && (col==row)));

    return _fs(row, col);
}

// -- methods ------------------------------------------------------------------

// for BLAS/LAPACK
template <typename FS>
StorageUpLo
TrMatrix<FS>::upLo() const
{
    return _upLo;
}

template <typename FS>
UnitDiag
TrMatrix<FS>::unitDiag() const
{
    return _unitDiag;
}

template <typename FS>
int
TrMatrix<FS>::dim() const
{
    assert(_fs.numRows()==_fs.numCols());

    return _fs.numRows();
}

template <typename FS>
int
TrMatrix<FS>::leadingDimension() const
{
    return engine().leadingDimension();
}

template <typename FS>
const typename TrMatrix<FS>::T *
TrMatrix<FS>::data() const
{
    return engine().data();
}

template <typename FS>
typename TrMatrix<FS>::T *
TrMatrix<FS>::data()
{
    return engine().data();
}
// for element access
template <typename FS>
int
TrMatrix<FS>::firstRow() const
{
    return _fs.firstRow();
}

template <typename FS>
int
TrMatrix<FS>::lastRow() const
{
    return _fs.lastRow();
}

template <typename FS>
int
TrMatrix<FS>::firstCol() const
{
    return _fs.firstCol();
}

template <typename FS>
int
TrMatrix<FS>::lastCol() const
{
    return _fs.lastCol();
}

template <typename FS>
Range
TrMatrix<FS>::rows() const
{
    return Range(firstRow(), lastRow());
}

template <typename FS>
Range
TrMatrix<FS>::cols() const
{
    return Range(firstCol(), lastCol());
}

// -- implementation -----------------------------------------------------------
template <typename FS>
const FS &
TrMatrix<FS>::engine() const
{
    return _fs;
}

template <typename FS>
FS &
TrMatrix<FS>::engine()
{
    return _fs;
}

// == TbMatrix =================================================================

template <typename BS>
TbMatrix<BS>::TbMatrix()
{
}

template <typename BS>
TbMatrix<BS>::TbMatrix(int dim, StorageUpLo upLo, int numDiags,
                       UnitDiag unitDiag, int firstIndex)
    : _bs(dim, dim,
          (upLo==Upper) ? 0        : -numDiags,
          (upLo==Upper) ? numDiags :  0,
          firstIndex),
      _upLo(upLo), _unitDiag(unitDiag)
{
}

template <typename BS>
TbMatrix<BS>::TbMatrix(const BS &bs, StorageUpLo upLo, UnitDiag unitDiag)
    : _bs(bs), _upLo(upLo), _unitDiag(unitDiag)
{
}

template <typename BS>
TbMatrix<BS>::TbMatrix(const TbMatrix<BS> &rhs)
    : TriangularMatrix<TbMatrix<BS> >(),
      _bs(rhs.engine()), _upLo(rhs.upLo()), _unitDiag(rhs.unitDiag())
{
}

template <typename BS>
template <typename RHS>
TbMatrix<BS>::TbMatrix(const TbMatrix<RHS> &rhs)
    : _bs(rhs.engine()), _upLo(rhs.upLo()), _unitDiag(rhs.unitDiag())
{
}

// -- operators ----------------------------------------------------------------

template <typename BS>
const typename TbMatrix<BS>::T &
TbMatrix<BS>::operator()(int row, int col) const
{
#ifndef NDEBUG
    if (_upLo==Upper) {
        assert(col>=row);
        assert(col-row<=numOffDiags());
    } else {
        assert(col<=row);
        assert(row-col<=numOffDiags());
    }
#endif
    return _bs(row, col);
}

template <typename BS>
typename TbMatrix<BS>::T &
TbMatrix<BS>::operator()(int row, int col)
{
#ifndef NDEBUG
    if (_upLo==Upper) {
        assert(col>=row);
        assert(col-row<=numOffDiags());
    } else {
        assert(col<=row);
        assert(row-col<=numOffDiags());
    }
#endif
    return _bs(row, col);
}

// -- views --------------------------------------------------------------------

template <typename BS>
typename TbMatrix<BS>::ConstVectorView
TbMatrix<BS>::diag(int d) const
{
    return _bs.viewDiag(d);
}

template <typename BS>
typename TbMatrix<BS>::VectorView
TbMatrix<BS>::diag(int d)
{
    return _bs.viewDiag(d);
}

// -- methods ------------------------------------------------------------------

// for BLAS/LAPACK
template <typename BS>
StorageUpLo
TbMatrix<BS>::upLo() const
{
    return _upLo;
}

template <typename BS>
UnitDiag
TbMatrix<BS>::unitDiag() const
{
    return _unitDiag;
}

template <typename BS>
int
TbMatrix<BS>::dim() const
{
    return _bs.numRows();
}

template <typename BS>
int
TbMatrix<BS>::numOffDiags() const
{
    return (_upLo==Upper) ? _bs.numSuperDiags()
                          : _bs.numSubDiags();
}

template <typename BS>
int
TbMatrix<BS>::leadingDimension() const
{
    return _bs.leadingDimension();
}

template <typename BS>
const typename TbMatrix<BS>::T *
TbMatrix<BS>::data() const
{
    return _bs.data();
}

template <typename BS>
typename TbMatrix<BS>::T *
TbMatrix<BS>::data()
{
    return _bs.data();
}

// for element access
template <typename BS>
int
TbMatrix<BS>::firstIndex() const
{
    return _bs.firstRow();
}

template <typename BS>
int
TbMatrix<BS>::lastIndex() const
{
    assert(_bs.lastRow()==_bs.lastCol());
    return _bs.lastRow();
}

template <typename BS>
Range
TbMatrix<BS>::indices() const
{
    return Raange(_bs.firstIndex(), _bs.lastIndex());
}

template <typename BS>
Range
TbMatrix<BS>::diags() const
{
    return (_upLo==Upper) ? Range(0, numOffDiags())
                          : Range(-numOffDiags(),0);
}

// -- implementation -----------------------------------------------------------

template <typename BS>
const BS &
TbMatrix<BS>::engine() const
{
    return _bs;
}

template <typename BS>
BS &
TbMatrix<BS>::engine()
{
    return _bs;
}

// == TpMatrix =================================================================

template <typename PS>
TpMatrix<PS>::TpMatrix()
{
}

template <typename PS>
TpMatrix<PS>::TpMatrix(int dim, UnitDiag unitDiag, int firstIndex)
    : _ps(dim, firstIndex), _unitDiag(unitDiag)
{
}

template <typename PS>
TpMatrix<PS>::TpMatrix(const PS &ps, UnitDiag unitDiag)
    : _ps(ps), _unitDiag(unitDiag)
{
}

// -- operators ----------------------------------------------------------------

template <typename PS>
const typename TpMatrix<PS>::T &
TpMatrix<PS>::operator()(int row, int col) const
{
#ifndef NDEBUG
    if (upLo()==Upper) {
        assert(col>=row);
    } else {
        assert(col<=row);
    }
#endif
    return _ps(row, col);
}

template <typename PS>
typename TpMatrix<PS>::T &
TpMatrix<PS>::operator()(int row, int col)
{
#ifndef NDEBUG
    if (upLo()==Upper) {
        assert(col>=row);
    } else {
        assert(col<=row);
    }
#endif
    return _ps(row, col);
}

// -- methods ------------------------------------------------------------------

// for BLAS/LAPACK
template <typename PS>
StorageUpLo
TpMatrix<PS>::upLo() const
{
    return StorageInfo<PS>::upLo;
}

template <typename PS>
UnitDiag
TpMatrix<PS>::unitDiag() const
{
    return _unitDiag;
}

template <typename PS>
int
TpMatrix<PS>::dim() const
{
    return _ps.dim();
}

template <typename PS>
const typename TpMatrix<PS>::T *
TpMatrix<PS>::data() const
{
    return _ps.data();
}

template <typename PS>
typename TpMatrix<PS>::T *
TpMatrix<PS>::data()
{
    return _ps.data();
}

// for element access
template <typename PS>
int
TpMatrix<PS>::firstIndex() const
{
    return _ps.firstIndex();
}

template <typename PS>
int
TpMatrix<PS>::lastIndex() const
{
    return _ps.lastIndex();
}

template <typename PS>
Range
TpMatrix<PS>::indices() const
{
    return Range(firstIndex(), lastIndex());
}

// -- implementation -----------------------------------------------------------

template <typename PS>
const PS &
TpMatrix<PS>::engine() const
{
    return _ps;
}

template <typename PS>
PS &
TpMatrix<PS>::engine()
{
    return _ps;
}

} // namespace flens
