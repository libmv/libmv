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

#ifndef FLENS_GENERALMATRIX_H
#define FLENS_GENERALMATRIX_H 1

#include <flens/array.h>
#include <flens/densevector.h>
#include <flens/listinitializer.h>
#include <flens/matvec.h>
#include <flens/range.h>
#include <flens/symmetricmatrix.h>
#include <flens/triangularmatrix.h>
#include <flens/sparsematrix.h>
#include <flens/underscore.h>

namespace flens {

// == GeMatrix =================================================================

template <typename FS>
class GeMatrix
    : public GeneralMatrix<GeMatrix<FS> >
{
    public:
        // shortcut for element type
        typedef typename GeMatrix<FS>::ElementType  T;

        // view types from FS
        typedef typename FS::ConstView          ConstFSView;
        typedef typename FS::View               FSView;
        typedef typename FS::NoView             FSNoView;

        typedef typename FS::ConstVectorView    ConstFSVectorView;
        typedef typename FS::VectorView         FSVectorView;
        typedef typename FS::VectorNoView       FSVectorNoView;

        // view types for GeMatrix
        typedef DenseVector<ConstFSVectorView>  ConstVectorView;
        typedef DenseVector<FSVectorView>       VectorView;
        typedef DenseVector<FSVectorNoView>     VectorNoView;

        typedef GeMatrix<ConstFSView>           ConstView;
        typedef GeMatrix<FSView>                View;
        typedef GeMatrix<FSNoView>              NoView;

        typedef TrMatrix<ConstFSView>           ConstTriangularView;
        typedef TrMatrix<FSView>                TriangularView;
        typedef TrMatrix<FSNoView>              TriangularNoView;

        typedef SyMatrix<ConstFSView>           ConstSymmetricView;
        typedef SyMatrix<FSView>                SymmetricView;
        typedef SyMatrix<FSNoView>              SymmetricNoView;

        // -- constructors -----------------------------------------------------
        GeMatrix();

        GeMatrix(int numRows, int numCols, int firstRow=0, int firstCol=0);

        GeMatrix(const Range &rows, const Range &cols);

        GeMatrix(const FS &fs);

        GeMatrix(const GeMatrix<FS> &rhs);

        template <typename RHS>
            GeMatrix(const GeMatrix<RHS> &fs);

        // -- operators --------------------------------------------------------
        ListInitializerSwitch<GeMatrix<FS> >
        operator=(const T &value);

        GeMatrix<FS> &
        operator=(const GeMatrix<FS> &rhs);

        template <typename RHS>
            GeMatrix<FS> &
            operator=(const Matrix<RHS> &rhs);

        template <typename SP>
            GeMatrix<FS> &
            operator=(const SparseGeMatrix<SP> &sp);

        GeMatrix<FS> &
        operator+=(const GeMatrix<FS> &rhs);

        template <typename RHS>
            GeMatrix<FS> &
            operator+=(const Matrix<RHS> &rhs);

        GeMatrix<FS> &
        operator-=(const GeMatrix<FS> &rhs);

        template <typename RHS>
            GeMatrix<FS> &
            operator-=(const Matrix<RHS> &rhs);

        GeMatrix<FS> &
        operator+=(T alpha);

        GeMatrix<FS> &
        operator-=(T alpha);

        GeMatrix<FS> &
        operator*=(T alpha);

        GeMatrix<FS> &
        operator/=(T alpha);

        bool
        operator==(const GeMatrix<FS> &rhs) const;

        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- views ------------------------------------------------------------

        // rectangular view
        const ConstView
        operator()(const Range &rows, const Range &cols,
                        int firstViewRow=0, int firstViewCol=0) const;

        View
        operator()(const Range &rows, const Range &cols,
                   int firstViewRow=0, int firstViewCol=0);

        const ConstView
        operator()(const Range &rows, const Underscore &allCols,
                   int firstViewRow=0, int firstViewCol=0) const;

        View
        operator()(const Range &rows, const Underscore &allCols,
                   int firstViewRow=0, int firstViewCol=0);

        const ConstView
        operator()(const Underscore &allRows, const Range &cols,
                   int firstViewRow=0, int firstViewCol=0) const;

        View
        operator()(const Underscore &allRows, const Range &cols,
                   int firstViewRow=0, int firstViewCol=0);

        // row views
        const ConstVectorView
        operator()(int row, const Range &cols,
                   int firstViewIndex=0) const;

        VectorView
        operator()(int row, const Range &cols,
                   int firstViewIndex=0);

        const ConstVectorView
        operator()(int row, const Underscore &allCols,
                   int firstViewIndex=0) const;

        VectorView
        operator()(int row, const Underscore &allCols,
                   int firstViewIndex=0);

        // col views
        const ConstVectorView
        operator()(const Range &rows, int col,
                   int firstViewIndex=0) const;

        VectorView
        operator()(const Range &rows, int col,
                   int firstViewIndex=0);

        const ConstVectorView
        operator()(const Underscore &allRows, int col,
                   int firstViewIndex=0) const;

        VectorView
        operator()(const Underscore &allRows, int col,
                   int firstViewIndex=0);

        // diag views
        const ConstVectorView
        diag(int d) const;

        VectorView
        diag(int d);

        // triangular views
        ConstTriangularView
        upper(int firstViewRow=0, int firstViewCol=0) const;

        TriangularView
        upper(int firstViewRow=0, int firstViewCol=0);

        ConstTriangularView
        upperUnit(int firstViewRow=0, int firstViewCol=0) const;

        TriangularView
        upperUnit(int firstViewRow=0, int firstViewCol=0);

        ConstTriangularView
        lower(int firstViewRow=0, int firstViewCol=0) const;

        TriangularView
        lower(int firstViewRow=0, int firstViewCol=0);

        ConstTriangularView
        lowerUnit(int firstViewRow=0, int firstViewCol=0) const;

        TriangularView
        lowerUnit(int firstViewRow=0, int firstViewCol=0);

        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        int
        numRows() const;

        int
        numCols() const;

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

        void
        resize(int numRows, int numCols);

        void
        resize(int numRows, int numCols, int firstRow, int firstCol);

        void
        resize(const Range &rows, const Range &cols);

        void
        resizeOrClear(int numRows, int numCols, int firstRow=0, int firstCol=0);

    	void
    	shiftIndex(int rowAmount, int colAmount);

        void
        shiftIndexTo(int firstRow, int firstCol);

        // -- implementation ---------------------------------------------------

        const FS &
        engine() const;

        FS &
        engine();

    private:
        FS _fs;
};

template <typename FS>
struct TypeInfo<GeMatrix<FS> >
{
    typedef GeMatrix<FS> Impl;
    typedef typename FS::ElementType ElementType;
};

// == GbMatrix =================================================================

template <typename BS>
class GbMatrix
    : public GeneralMatrix<GbMatrix<BS> >
{
    public:
        // shortcut for element type
        typedef typename GbMatrix<BS>::ElementType  T;

        // view types from BS
        typedef typename BS::ConstView          ConstBSView;
        typedef typename BS::View               BSView;
        typedef typename BS::View               BSNoView;

        typedef typename BS::ConstVectorView    ConstBSVectorView;
        typedef typename BS::VectorView         BSVectorView;
        typedef typename BS::VectorNoView       BSVectorNoView;

        // view types for GeMatrix
        typedef DenseVector<ConstBSVectorView>  ConstVectorView;
        typedef DenseVector<BSVectorView>       VectorView;
        typedef DenseVector<BSVectorNoView>     VectorNoView;

        typedef GbMatrix<ConstBSView>           ConstView;
        typedef GbMatrix<BSView>                View;
        typedef GbMatrix<BSNoView>              NoView;

        typedef TbMatrix<ConstBSView>           ConstTriangularView;
        typedef TbMatrix<BSView>                TriangularView;
        typedef TbMatrix<BSNoView>              TriangularNoView;

        typedef SbMatrix<ConstBSView>           ConstSymmetricView;
        typedef SbMatrix<BSView>                SymmetricView;
        typedef SbMatrix<BSNoView>              SymmetricNoView;

        // -- constructors -----------------------------------------------------
        GbMatrix();

        GbMatrix(int numRows, int numCols, int numSubDiags, int numSuperDiags,
                 int indexBase=1);

        GbMatrix(const BS &bs);

        GbMatrix(const GbMatrix<BS> &rhs);

        template <typename RHS>
            GbMatrix(const GbMatrix<RHS> &rhs);

        // -- operators --------------------------------------------------------
        GbMatrix<BS> &
        operator=(const GbMatrix<BS> &rhs);

        template <typename RHS>
            GbMatrix<BS> &
            operator=(const Matrix<RHS> &rhs);

        GbMatrix<BS> &
        operator+=(const GbMatrix<BS> &rhs);

        template <typename RHS>
            GbMatrix<BS> &
            operator+=(const Matrix<RHS> &rhs);

        GbMatrix<BS> &
        operator-=(const GbMatrix<BS> &rhs);

        template <typename RHS>
            GbMatrix<BS> &
            operator-=(const Matrix<RHS> &rhs);

        GbMatrix<BS> &
        operator*=(T alpha);

        GbMatrix<BS> &
        operator/=(T alpha);

        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- views ------------------------------------------------------------

        // view of one diagonal
        ConstVectorView
        diag(int d) const;

        VectorView
        diag(int d);

        // view of bands of diagonal
        ConstView
        diags(int fromDiag, int toDiag) const;

        View
        diags(int fromDiag, int toDiag);

        ConstView
        diags(const Range &range) const;

        View
        diags(const Range &range);

        // triangular views
        ConstTriangularView
        upper(int viewIndex=1) const;

        TriangularView
        upper(int viewIndex=1);

        ConstTriangularView
        upperUnit(int viewIndex=1) const;

        TriangularView
        upperUnit(int viewIndex=1);

        ConstTriangularView
        lower(int viewIndex=1) const;

        TriangularView
        lower(int viewIndex=1);

        ConstTriangularView
        lowerUnit(int viewIndex=1) const;

        TriangularView
        lowerUnit(int viewIndex=1);


        // -- methods ----------------------------------------------------------

        // for BLAS/LAPACK
        int
        numRows() const;

        int
        numCols() const;

        int
        numSubDiags() const;

        int
        numSuperDiags() const;

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
        rowIndices() const;

        Range
        colIndices() const;

        Range
        diags() const;

        // -- implementation ---------------------------------------------------
        const BS &
        engine() const;

        BS &
        engine();

    private:
        BS _bs;
};

template <typename BS>
struct TypeInfo<GbMatrix<BS> >
{
    typedef GbMatrix<BS> Impl;
    typedef typename BS::ElementType ElementType;
};

} // namespace flens

#include <flens/generalmatrix.tcc>

#endif // FLENS_GENERALMATRIX_H
