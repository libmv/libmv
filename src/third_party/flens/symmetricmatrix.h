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

#ifndef FLENS_SYMMETRICMATRIX_H
#define FLENS_SYMMETRICMATRIX_H 1

#include <flens/array.h>
#include <flens/densevector.h>
#include <flens/matvec.h>
#include <flens/storage.h>
#include <flens/triangularmatrix.h>

namespace flens {

// == SyMatrix =================================================================

template <typename FS>
class SyMatrix
    : public SymmetricMatrix<SyMatrix<FS> >
{
    public:
        typedef typename SyMatrix<FS>::ElementType T;

        SyMatrix();

        SyMatrix(int dim, StorageUpLo upLo, int firstIndex=1);

        SyMatrix(const FS &fs, StorageUpLo upLo);

        SyMatrix(const SyMatrix<FS> &rhs);

        template <typename RHS>
            SyMatrix(const SyMatrix<RHS> &rhs);

        SyMatrix(const TrMatrix<FS> &rhs);

        template <typename RHS>
            SyMatrix(const TrMatrix<RHS> &rhs);

        // -- operators --------------------------------------------------------

        SyMatrix &
        operator*=(T alpha);

        SyMatrix &
        operator/=(T alpha);

        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        StorageUpLo
        upLo() const;

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
};

template <typename FS>
struct TypeInfo<SyMatrix<FS> >
{
    typedef SyMatrix<FS> Impl;
    typedef typename FS::ElementType ElementType;
};

// == SbMatrix =================================================================

template <typename BS>
class SbMatrix
    : public SymmetricMatrix<SbMatrix<BS> >
{
    public:
        typedef typename SbMatrix<BS>::ElementType  T;


        // view types from BS
        typedef typename BS::ConstView          ConstBSView;
        typedef typename BS::View               BSView;
        typedef typename BS::View               BSNoView;

        typedef typename BS::ConstVectorView    ConstBSVectorView;
        typedef typename BS::VectorView         BSVectorView;
        typedef typename BS::VectorNoView       BSVectorNoView;

        // view types for SbMatrix
        typedef DenseVector<ConstBSVectorView>  ConstVectorView;
        typedef DenseVector<BSVectorView>       VectorView;
        typedef DenseVector<BSVectorNoView>     VectorNoView;

        typedef TbMatrix<ConstBSView>           ConstTriangularView;
        typedef TbMatrix<BSView>                TriangularView;
        typedef TbMatrix<BSNoView>              TriangularNoView;

        SbMatrix();

        SbMatrix(int dim, StorageUpLo upLo, int numOffDiags, int firstIndex=1);

        SbMatrix(const BS &bs, StorageUpLo upLo);

        SbMatrix(const SbMatrix<BS> &rhs);

        template <typename RHS>
            SbMatrix(const SbMatrix<RHS> &rhs);

        SbMatrix(const TbMatrix<BS> &rhs);

        template <typename RHS>
            SbMatrix(const TbMatrix<RHS> &rhs);

        // -- operators --------------------------------------------------------

        SbMatrix<BS> &
        operator*=(T alpha);

        SbMatrix<BS> &
        operator/=(T alpha);

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

        // for element access
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
};

template <typename BS>
struct TypeInfo<SbMatrix<BS> >
{
    typedef SbMatrix<BS> Impl;
    typedef typename BS::ElementType ElementType;
};

// == SpMatrix =================================================================

template <typename PS>
class SpMatrix
    : public SymmetricMatrix<SpMatrix<PS> >
{
    public:
        typedef typename SpMatrix<PS>::ElementType  T;
        typedef DenseVector<ConstArrayView<T> >     ConstVectorView;
        typedef DenseVector<ArrayView<T> >          VectorView;

        SpMatrix();

        SpMatrix(int dim, int firstIndex=1);

        SpMatrix(const PS &ps);

        // -- operators --------------------------------------------------------
        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        StorageUpLo
        upLo() const;

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
};

template <typename PS>
struct TypeInfo<SpMatrix<PS> >
{
    typedef SpMatrix<PS> Impl;
    typedef typename PS::ElementType ElementType;
};

} // namespace flens

#include <flens/symmetricmatrix.tcc>

#endif // FLENS_SYMMETRIC_MATRIX_H
