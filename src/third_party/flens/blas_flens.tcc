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

#include <numeric>

#include <flens/macros.h>
#include <flens/blas.h>

namespace flens {

//- Level 1 --------------------------------------------------------------------

template <typename X, typename Y>
typename DenseVector<X>::ElementType
dot(const DenseVector<X> &x, const DenseVector<Y> &y)
{
    return dot(x.impl().length(), x.impl().data(), x.impl().stride(),
               y.impl().data(), y.impl().stride());
}

template <typename X, typename Y>
void
copy(const DenseVector<X> &x, DenseVector<Y> &y)
{
//    assert(&x!=&y);
    if (y.length()!=x.length()) {
        y.engine().resize(x.length(), x.firstIndex());
    }
    copy(x.length(), x.data(), x.stride(), y.data(), y.stride());
}

template <typename X>
void
scal(typename DenseVector<X>::ElementType alpha, DenseVector<X> &x)
{
    scal(x.length(), alpha, x.data(), x.stride());
}

template <typename I>
typename DenseVector<I>::ElementType
asum(const DenseVector<I> &x)
{
    return asum(x.length(), x.data(), x.stride());
}

template <typename I>
int
amax(const DenseVector<I> &x)
{
    return amax(x.length(), x.data(), x.stride()) + x.firstIndex();
}

template <typename I>
int
amin(const DenseVector<I> &x)
{
    return amin(x.length(), x.data(), x.stride()) + x.firstIndex();
}

template <typename X, typename Y>
void
axpy(typename DenseVector<X>::ElementType alpha,
     const DenseVector<X> &x,
     DenseVector<Y> &y)
{
    assert(y.length()==x.length());

    axpy(x.length(), alpha, x.data(), x.stride(), y.data(), y.stride());
}

template <typename X>
typename DenseVector<X>::ElementType
nrm2(const DenseVector<X> &x)
{
    return nrm2(x.length(), x.data(), x.stride());
}

// for matrices
template <typename X>
void
scal(typename GeMatrix<X>::ElementType alpha, GeMatrix<X> &x)
{
    for (int i=x.firstRow(); i<=x.lastRow(); ++i) {
        x(i,_) *= alpha;
    }
}

template <typename X>
void
scal(typename GbMatrix<X>::ElementType alpha, GbMatrix<X> &x)
{
    for (int k=-x.numSubDiags(); k<=x.numSuperDiags(); ++k) {
        x.diag(k) *= alpha;
    }
}

template <typename X>
void
scal(typename SyMatrix<X>::ElementType alpha, SyMatrix<X> &x)
{
    if (x.upLo()==Upper) {
        int offset = x.firstCol() - x.firstRow();
        for (int i=x.firstRow(); i<=x.lastRow(); ++i) {
            for (int j=i+offset; j<=x.lastCol(); ++j) {
                x(i,j) *= alpha;
            }
        }
    } else {
        int offset = x.lastCol() - x.lastRow();
        for (int i=x.firstRow(); i<=x.lastRow(); ++i) {
            for (int j=x.firstCol; j<=i+offset; ++j) {
                x(i,j) *= alpha;
            }
        }
    }
}

template <typename X>
void
scal(typename SbMatrix<X>::ElementType alpha, SbMatrix<X> &x)
{
    Range r = x.diags();
    for (int i=r.firstIndex(); i<=r.lastIndex(); ++i) {
        x.diag(i) *= alpha;
    }
}

template <typename X, typename Y>
void
copy(const GeMatrix<X> &x, GeMatrix<Y> &y)
{
    if ((y.numRows()!=x.numRows()) || (y.numCols()!=x.numCols())) {
        y.resize(x.numRows(), x.numCols(),
                 x.firstRow(), x.firstCol());
    }

    for (int i=x.firstRow(), I=y.firstRow(); i<=x.lastRow(); ++i, ++I) {
        y(I,_) = x(i,_);
    }
}

template <typename X, typename Y>
void
copy(const GbMatrix<X> &x, GbMatrix<Y> &y)
{
    if ((y.numRows()!=x.numRows())
     || (y.numCols()!=x.numCols())
     || (y.numSubDiags()!=x.numSubDiags())
     || (y.numSuperDiags()!=x.numSuperDiags()))
    {
        y.engine().resize(x.numRows(), x.numCols(),
                          x.numSubDiags(), x.numSuperDiags(),
                          x.firstRow());
    }

    for (int k=-x.numSubDiags(); k<=x.numSuperDiags(); ++k) {
        y.diag(k) = x.diag(k);
    }
}

template <typename X, typename Y>
void
copy(const SparseGeMatrix<X> &x, GeMatrix<Y> &y)
{
    y.resizeOrClear( x.numRows(), x.numCols() );

    typedef typename SparseGeMatrix<X>::const_iterator It;
    for (It it=x.begin(); it!=x.end(); ++it) {
        y(it->first.first, it->first.second) = it->second;
    }
}

template <typename X, typename Y>
void
copy(const GeMatrix<X> &x, SparseGeMatrix<Y> &y, double eps)
{
    // NOTE we have to resize the sparse matrix here, also if they
    //      have the same size, because then it is already initialized
    //      and we cannot call finalize !
    y.resize(x.numRows(), x.numCols());

    for (int i=x.firstRow(); i<=x.lastRow(); ++i) {
        for (int j=x.firstCol(); j<=x.lastCol(); ++j) {
            if( std::abs( x(i,j) ) > eps ) y(i,j) = x(i,j);
        }
    }

    y.finalize();
}

template <typename X, typename Y>
void
copyTrans(const GeMatrix<X> &x, GeMatrix<Y> &y)
{
    if ((y.numRows()!=x.numCols()) || (y.numCols()!=x.numRows())) {
        y.resize(x.numCols(), x.numRows(),
                 x.firstCol(), x.firstRow());
    }

    for (int i=x.firstRow(), I=y.firstCol(); i<=x.lastRow(); ++i, ++I) {
        y(_,I) = x(i,_);
    }
}

template <typename X, typename Y>
void
copyTrans(const GbMatrix<X> &x, GbMatrix<Y> &y)
{
    if ((y.numRows()!=x.numCols())
     || (y.numCols()!=x.numRows())
     || (y.numSubDiags()!=x.numSuperDiags())
     || (y.numSuperDiags()!=x.numSubDiags()))
    {
        y.resize(x.numCols(), x.numRows(),
                 x.numSuperDiags(), x.numSubDiags(),
                 x.firstIndex());
    }

    for (int k=-x.numSubDiags(); k<=x.numSuperDiags(); ++k) {
        y.diag(k) = x.diag(k);
    }
}

template <typename X, typename Y>
void
copyConjugateTrans(const GeMatrix<X> &x, GeMatrix<Y> &y)
{
    if ((y.numRows()!=x.numCols()) || (y.numCols()!=x.numRows())) {
        y.resize(x.numCols(), x.numRows(),
                 x.firstCol(), x.firstRow());
    }
    // there is no blas function to calculate the conjugate transpose ...
    for (int i=x.firstRow(), I=y.firstCol(); i<=x.lastRow(); ++i, ++I) {
        for(int jj = x.firstCol(), JJ = y.firstRow(); jj <= x.lastCol(); jj++, JJ++) {
            y(JJ,I) = conjugate(x(i,jj));
        }
    }
}

template <typename Y>
void copy(const Id &A, GeMatrix<Y> &B)
{
    if (A.dim()!=B.numRows()) {
        B.resize(A.dim(),A.dim(),1,1);
    } else {
        B = typename Y::ElementType(0.);
    }
    B.diag(0) = typename Y::ElementType(1.);
}

template <typename X, typename Y>
void
axpy(typename GeMatrix<X>::ElementType alpha,
     const GeMatrix<X> &x,
     GeMatrix<Y> &y)
{
    assert(y.numRows()==x.numRows());
    assert(y.numCols()==x.numCols());

    int I = y.firstRow();
    for (int i=x.firstRow(); i<=x.lastRow(); ++i, ++I) {
        y(I,_) += alpha*x(i,_);
    }

}

template <typename X, typename Y>
void
axpy(typename GbMatrix<X>::ElementType alpha,
     const GbMatrix<X> &x,
     GbMatrix<Y> &y)
{
    assert(y.numRows()==x.numRows());
    assert(y.numCols()==x.numCols());
    assert(y.numSubDiags()==x.numSubDiags());
    assert(y.numSuperDiags()==x.numSuperDiags());

    for (int k=-x.numSubDiags(); k<=x.numSuperDiags(); ++k) {
        y.diag(k) += alpha*x.diag(k);
    }
}

//- Level 2 --------------------------------------------------------------------

// gemv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans,
   ALPHA alpha, const GeMatrix<MA> &A, const DenseVector<VX> &x,
   BETA beta, DenseVector<VY> &y)
{
    assert(ADDRESS(y)!=ADDRESS(x));
    assert(x.length()==((trans==NoTrans) ? A.numCols()
                                         : A.numRows()));

    int yLength = (trans==NoTrans) ? A.numRows()
                                   : A.numCols();

    assert((beta==0) || (y.length()==yLength));

    if (y.length()!=yLength) {
        y.resize(yLength);
    }

    gemv(StorageInfo<MA>::order,
         trans, A.numRows(), A.numCols(),
         alpha,
         A.data(), A.leadingDimension(),
         x.data(), x.stride(),
         beta,
         y.data(), y.stride());
}

// gbmv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans,
   ALPHA alpha, const GbMatrix<MA> &A, const DenseVector<VX> &x,
   BETA beta, DenseVector<VY> &y)
{
    assert(A.numSubDiags()>=0);            // kl >= 0
    assert(A.numSuperDiags()>=0);            // ku >= 0

    assert(ADDRESS(y)!=ADDRESS(x));
    assert(x.length()==((trans==NoTrans) ? A.numCols()
                                         : A.numRows()));
    int yLength = (trans==NoTrans) ? A.numRows()
                                   : A.numCols();

    assert((beta==0) || (y.length()==yLength));

    if (y.length()!=yLength) {
        y.resize(yLength);
    }
    gbmv(StorageInfo<MA>::order,
         trans, A.numRows(), A.numCols(),
         A.numSubDiags(), A.numSuperDiags(),
         alpha,
         A.data(), A.leadingDimension(),
         x.data(), x.stride(),
         beta,
         y.data(), y.stride());
}

// trmv
template <typename MA, typename VX>
void
mv(Transpose trans, const TrMatrix<MA> &A, DenseVector<VX> &x)
{
    assert(x.length()==A.dim());

    trmv(StorageInfo<MA>::order,
         A.upLo(), trans, A.unitDiag(), A.dim(),
         A.data(), A.leadingDimension(),
         x.data(), x.stride());
}

// tbmv
template <typename MA, typename VX>
void
mv(Transpose trans, const TbMatrix<MA> &A, DenseVector<VX> &x)
{
    assert(x.length()==A.dim());

    tbmv(StorageInfo<MA>::order,
         A.upLo(), trans, A.unitDiag(), A.dim(), A.numOffDiags(),
         A.data(), A.leadingDimension(),
         x.data(), x.stride());
}

// tpmv
template <typename MA, typename VX>
void
mv(Transpose trans, const TpMatrix<MA> &A, DenseVector<VX> &x)
{
    assert(x.length()==A.dim());

    tpmv(StorageInfo<MA>::order,
         A.upLo(), trans, A.unitDiag(), A.dim(),
         A.data(), x.data(), x.stride());
}

// symv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(ALPHA alpha, const SyMatrix<MA> &A, const DenseVector<VX> &x,
   BETA beta, DenseVector<VY> &y)
{
    assert(x.length()==A.dim());
    assert((beta==0) || (y.length()==A.dim()));

    if (y.length()!=A.dim()) {
        y.resize(A.dim());
    }

    symv(StorageInfo<MA>::order,
         A.upLo(), A.dim(),
         alpha,
         A.data(), A.leadingDimension(),
         x.data(), x.stride(),
         beta,
         y.data(), y.stride());
}

// sbmv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(ALPHA alpha, const SbMatrix<MA> &A, const DenseVector<VX> &x,
   BETA beta, DenseVector<VY> &y)
{
    assert(x.length()==A.dim());
    assert((beta==0) || (y.length()==A.dim()));

    if (y.length()!=A.dim()) {
        y.resize(A.dim());
    }

    sbmv(StorageInfo<MA>::order,
         A.upLo(), A.dim(), A.numOffDiags(),
         alpha,
         A.data(), A.leadingDimension(),
         x.data(), x.stride(),
         beta,
         y.data(), y.stride());
}

// spmv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(ALPHA alpha, const SpMatrix<MA> &A, const DenseVector<VX> &x,
   BETA beta, DenseVector<VY> &y)
{
    assert((beta==0) || (y.length()==A.dim()));

    if (y.length()!=A.dim()) {
        y.resize(A.dim());
    }

    spmv(StorageInfo<MA>::order,
         A.upLo(), A.dim(), alpha, A.data(),
         x.data(), x.stride(),
         beta,
         y.data(), y.stride());
}

//- Level 3 --------------------------------------------------------------------

// gemm
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB,
   ALPHA alpha, const GeMatrix<MA> &A, const GeMatrix<MB> &B,
   BETA beta, GeMatrix<MC> &C)
{
    assert(StorageInfo<MA>::order==StorageInfo<MB>::order);
    assert(StorageInfo<MA>::order==StorageInfo<MC>::order);

    // M: op(A) - M x K
    // N: op(B) - K x N
#ifndef NDEBUG
    int K_A = (transA==NoTrans) ? A.numCols() : A.numRows();
    int K_B = (transB==NoTrans) ? B.numRows() : B.numCols();
    assert(K_A==K_B);
#endif

    int m = (transA==NoTrans) ? A.numRows() : A.numCols();
    int n = (transB==NoTrans) ? B.numCols() : B.numRows();

    assert((beta==0) || (C.numRows()==m));
    assert((beta==0) || (C.numCols()==n));

    if ((C.numRows()!=m) || (C.numCols()!=n)) {
        C.resize(m,n);
    }

    gemm(StorageInfo<MA>::order,
         transA, transB,
         C.numRows(),
         C.numCols(),
         (transA==NoTrans) ? A.numCols() : A.numRows(),   // K
         alpha,
         A.data(), A.leadingDimension(),
         B.data(), B.leadingDimension(),
         beta,
         C.data(), C.leadingDimension());
}

// symm
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
void
mm(BlasSide side,
   ALPHA alpha, const SyMatrix<MA> A, const GeMatrix<MB> &B,
   BETA beta, GeMatrix<MC> &C)
{
    assert(StorageInfo<MA>::order==StorageInfo<MB>::order);
    assert(StorageInfo<MA>::order==StorageInfo<MC>::order);
#ifndef NDEBUG
    if (side==Left) {
        assert(A.dim()==B.numRows());
    } else {
        assert(B.numCols()==A.dim());
    }
#endif

    int m = (side==Left) ? A.dim() : B.numRows();
    int n = (side==Left) ? B.numCols() : A.dim();

    assert((beta==0) || (C.numRows()==m));
    assert((beta==0) || (C.numCols()==n));

    if ((C.numRows()!=m) || (C.numCols()!=n)) {
        C.resize(m,n);
    }

    symm(StorageInfo<MA>::order,
         side, A.upLo(),
         C.numRows(), C.numCols(),
         alpha,
         A.data(), A.leadingDimension(),
         B.data(), B.leadingDimension(),
         beta, C.data(), C.leadingDimension());
}

// trmm
template <typename ALPHA, typename MA, typename MB>
void
mm(BlasSide side,
   Transpose transA, ALPHA alpha, const TrMatrix<MA> &A,
   GeMatrix<MB> &B)
{
    assert(StorageInfo<MA>::order==StorageInfo<MB>::order);
#ifndef NDEBUG
    if (side==Left) {
        assert(A.dim()==B.numRows());
    } else {
        assert(B.numCols()==A.dim());
    }
#endif

    trmm(StorageInfo<MA>::order,
         side, A.upLo(), transA,
         A.unitDiag(),
         B.numRows(), B.numCols(),
         alpha,
         A.data(), A.leadingDimension(),
         B.data(), B.leadingDimension());

}

template <typename E, typename S>
void
trsv(Transpose transpose,
     const TrMatrix<S> &A,
     DenseVector<E> &xb)
{
        CBLAS_ORDER order = CblasColMajor;
//    CBLAS_ORDER order = (Order==ColMajor) ? CblasColMajor:CblasRowMajor;
    CBLAS_UPLO upLo = (A.upLo()==Lower) ? CblasLower:CblasUpper;
    CBLAS_DIAG diag = (A.unitDiag()==Unit) ? CblasUnit:CblasNonUnit;
    CBLAS_TRANSPOSE transA;

    switch (transpose) {
        case NoTrans:
            transA = CblasNoTrans;
            break;
        case Trans:
            transA = CblasTrans;
            break;
        case ConjTrans:
            transA = CblasConjTrans;
            break;
        default:
            assert(0);
    }
    assert(xb.length()==A.dim());

    cblas_dtrsv(order,                      // ORDER
                upLo,                       // UPLO
                transA,                     // TRANSA
                diag,                       // DIAG
                A.dim(),                    // N
                A.data(),                   // A
                A.leadingDimension(),       // LDA
                xb.data(),                  // X
                xb.stride());               // INCX
}

// trsm
template <typename ALPHA, typename MA, typename MB>
void
trsm(BlasSide side,
     Transpose transA, ALPHA alpha, const TrMatrix<MA> &A,
     GeMatrix<MB> &B)
{
    assert(StorageInfo<MA>::order==StorageInfo<MB>::order);
#ifndef NDEBUG
    if (side==Left) {
        assert(A.dim()==B.numRows());
    } else {
        assert(B.numCols()==A.dim());
    }
#endif

    trsm(StorageInfo<MA>::order,
         side, A.upLo(), transA,
         A.unitDiag(),
         B.numRows(), B.numCols(),
         alpha,
         A.data(), A.leadingDimension(),
         B.data(), B.leadingDimension());

}

} // namespace flens
