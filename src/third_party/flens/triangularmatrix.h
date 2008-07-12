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

#ifndef FLENS_TRIANGULARMATRIX_H
#define FLENS_TRIANGULARMATRIX_H 1

#include <flens/matvec.h>
#include <flens/storage.h>

namespace flens {

// == TrMatrix =================================================================

template <typename FS>
class TrMatrix
    : public TriangularMatrix<TrMatrix<FS> >
{
    public:
        typedef typename TrMatrix<FS>::ElementType T;

        TrMatrix();

        TrMatrix(int dim, StorageUpLo upLo, UnitDiag unitDiag=NonUnit,
                 int firstIndex=0);

        TrMatrix(const FS &fs, StorageUpLo upLo, UnitDiag unitDiag=NonUnit);

        TrMatrix(const TrMatrix<FS> &rhs);

        template <typename RHS>
            TrMatrix(const TrMatrix<RHS> &rhs);

        // -- operators --------------------------------------------------------
        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        StorageUpLo
        upLo() const;

        UnitDiag
        unitDiag() const;

        int
        dim() const;

        int
        leadingDimension() const;

        const T *
        data() const;

        T *
        data();

        // for element access
        int
        firstRow() const;

        int
        lastRow() const;

        int
        firstCol() const;

        int
        lastCol() const;

        Range
        rows() const;

        Range
        cols() const;

        // -- implementation ---------------------------------------------------
        const FS &
        engine() const;

        FS &
        engine();

    private:
        FS _fs;
        StorageUpLo _upLo;
        UnitDiag _unitDiag;
};

template <typename FS>
struct TypeInfo<TrMatrix<FS> >
{
    typedef TrMatrix<FS> Impl;
    typedef typename FS::ElementType ElementType;
};

// == TbMatrix =================================================================

template <typename BS>
class TbMatrix
    : public TriangularMatrix<TbMatrix<BS> >
{
    public:
        typedef typename TbMatrix<BS>::ElementType  T;

        // view types from BS
        typedef typename BS::ConstView          ConstBSView;
        typedef typename BS::View               BSView;
        typedef typename BS::View               BSNoView;

        typedef typename BS::ConstVectorView    ConstBSVectorView;
        typedef typename BS::VectorView         BSVectorView;
        typedef typename BS::VectorNoView       BSVectorNoView;

        // view types for TbMatrix
        typedef DenseVector<ConstBSVectorView>  ConstVectorView;
        typedef DenseVector<BSVectorView>       VectorView;
        typedef DenseVector<BSVectorNoView>     VectorNoView;

        TbMatrix();

        TbMatrix(int dim, StorageUpLo upLo, int numDiags,
                 UnitDiag unitDiag=NonUnit, int firstIndex=0);

        TbMatrix(const BS &bs, StorageUpLo upLo, UnitDiag unitDiag=NonUnit);

        TbMatrix(const TbMatrix<BS> &rhs);

        template <typename RHS>
            TbMatrix(const TbMatrix<RHS> &rhs);

        // -- operators --------------------------------------------------------
        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- views ------------------------------------------------------------
        ConstVectorView
        diag(int d) const;

        VectorView
        diag(int d);

        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        StorageUpLo
        upLo() const;

        UnitDiag
        unitDiag() const;

        int
        dim() const;

        int
        numOffDiags() const;

        int
        leadingDimension() const;

        const T *
        data() const;

        T *
        data();

        // for element accesss
        int
        firstIndex() const;

        int
        lastIndex() const;

        Range
        indices() const;

        Range
        diags() const;

        // -- implementation ---------------------------------------------------
        const BS &
        engine() const;

        BS &
        engine();

    private:
        BS _bs;
        StorageUpLo _upLo;
        UnitDiag _unitDiag;
};

template <typename BS>
struct TypeInfo<TbMatrix<BS> >
{
    typedef TbMatrix<BS> Impl;
    typedef typename BS::ElementType ElementType;
};

// == TpMatrix =================================================================

template <typename PS>
class TpMatrix
    : public TriangularMatrix<TpMatrix<PS> >
{
    public:
        typedef typename TpMatrix<PS>::ElementType T;

        TpMatrix();

        TpMatrix(int dim, UnitDiag unitDiag=NonUnit, int firstIndex=0);

        TpMatrix(const PS &ps, UnitDiag unitDiag=NonUnit);

        // -- operators --------------------------------------------------------
        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        StorageUpLo
        upLo() const;

        UnitDiag
        unitDiag() const;

        int
        dim() const;

        const T *
        data() const;

        T *
        data();

        // for element access
        int
        firstIndex() const;

        int
        lastIndex() const;

        Range
        indices() const;

        // -- implementation ---------------------------------------------------
        const PS &
        engine() const;

        PS &
        engine();

    private:
        PS _ps;
        UnitDiag _unitDiag;
};

template <typename PS>
struct TypeInfo<TpMatrix<PS> >
{
    typedef TpMatrix<PS> Impl;
    typedef typename PS::ElementType ElementType;
};


} // namespace flens

#include <flens/triangularmatrix.tcc>

#endif // FLENS_TRIANGULARMATRIX_H
