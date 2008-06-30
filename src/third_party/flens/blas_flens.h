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

#ifndef FLENS_BLAS_FLENS_H
#define FLENS_BLAS_FLENS_H 1

#include <flens/densevector.h>
#include <flens/fullstorage.h>
#include <flens/generalmatrix.h>
#include <flens/sparsematrix.h>
#include <flens/id.h>
#include <flens/symmetricmatrix.h>
#include <flens/triangularmatrix.h>

namespace flens {

template <typename I>
    class GbMatrix;

template <typename I>
    class SbMatrix;

template <typename I>
    class SyMatrix;

//- Level 1 --------------------------------------------------------------------

template <typename X, typename Y>
    typename DenseVector<X>::ElementType
    dot(const DenseVector<X> &x, const DenseVector<Y> &y);

template <typename X, typename Y>
    void
    copy(const DenseVector<X> &x, DenseVector<Y> &y);

template <typename X>
    void
    scal(typename DenseVector<X>::ElementType alpha, DenseVector<X> &x);

template <typename I>
    typename DenseVector<I>::ElementType
    asum(const DenseVector<I> &x);

template <typename I>
    int
    amax(const DenseVector<I> &x);

template <typename I>
    int
    amin(const DenseVector<I> &x);

template <typename X, typename Y>
    void
    axpy(typename DenseVector<X>::ElementType alpha,
         const DenseVector<X> &x,
         DenseVector<Y> &y);

template <typename X>
    typename DenseVector<X>::ElementType
    nrm2(const DenseVector<X> &x);

// for matrices
template <typename X>
    void
    scal(typename GeMatrix<X>::ElementType alpha, GeMatrix<X> &x);

template <typename X>
    void
    scal(typename GbMatrix<X>::ElementType alpha, GbMatrix<X> &x);

template <typename X>
    void
    scal(typename SyMatrix<X>::ElementType alpha, SyMatrix<X> &x);

template <typename X>
    void
    scal(typename SbMatrix<X>::ElementType alpha, SbMatrix<X> &x);

template <typename X, typename Y>
    void
    copy(const GeMatrix<X> &x, GeMatrix<Y> &y);

template <typename X, typename Y>
    void
    copy(const GbMatrix<X> &x, GbMatrix<Y> &y);

template <typename X, typename Y>
    void
    copy(const SparseGeMatrix<X> &x, GeMatrix<Y> &y);

template <typename X, typename Y>
    void
    copy(const GeMatrix<X> &x, SparseGeMatrix<Y> &y, double eps=0.);

template <typename X, typename Y>
    void
    copyTrans(const GeMatrix<X> &x, GeMatrix<Y> &y);

template <typename X, typename Y>
    void
    copyTrans(const GbMatrix<X> &x, GbMatrix<Y> &y);

template <typename X, typename Y>
    void
    copyConjugateTrans(const GeMatrix<X> &x, GeMatrix<Y> &y);

template <typename Y>
    void
    copy(const Id &A, GeMatrix<Y> &B);

template <typename X, typename Y>
    void
    axpy(typename GeMatrix<X>::ElementType alpha,
         const GeMatrix<X> &x,
         GeMatrix<Y> &y);

template <typename X, typename Y>
    void
    axpy(typename GbMatrix<X>::ElementType alpha,
         const GbMatrix<X> &x,
         GbMatrix<Y> &y);

//- Level 2 --------------------------------------------------------------------

// gemv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
    void
    mv(Transpose trans,
       ALPHA alpha, const GeMatrix<MA> &A, const DenseVector<VX> &x,
       BETA beta, DenseVector<VY> &y);

// gbmv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
    void
    mv(Transpose trans,
       ALPHA alpha, const GbMatrix<MA> &A, const DenseVector<VX> &x,
       BETA beta, DenseVector<VY> &y);

// trmv
template <typename MA, typename VX>
    void
    mv(Transpose trans, const TrMatrix<MA> &A, DenseVector<VX> &x);

// tbmv
template <typename MA, typename VX>
    void
    mv(Transpose trans, const TbMatrix<MA> &A, DenseVector<VX> &x);

// tpmv
template <typename MA, typename VX>
    void
    mv(Transpose trans, const TpMatrix<MA> &A, DenseVector<VX> &x);

// symv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
    void
    mv(ALPHA alpha, const SyMatrix<MA> &A, const DenseVector<VX> &x,
       BETA beta, DenseVector<VY> &y);

// sbmv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
    void
    mv(ALPHA alpha, const SbMatrix<MA> &A, const DenseVector<VX> &x,
       BETA beta, DenseVector<VY> &y);

// spmv
template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
    void
    mv(ALPHA alpha, const SpMatrix<MA> &A, const DenseVector<VX> &x,
       BETA beta, DenseVector<VY> &y);


//- Level 3 --------------------------------------------------------------------

// gemm
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB,
       ALPHA alpha, const GeMatrix<MA> &A, const GeMatrix<MB> &B,
       BETA beta, GeMatrix<MC> &C);

// symm
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
    void
    mm(BlasSide side,
       ALPHA alpha, const SyMatrix<MA> A, const GeMatrix<MB> &B,
       BETA beta, GeMatrix<MC> &C);

// trmm
template <typename ALPHA, typename MA, typename MB>
    void
    mm(BlasSide side,
       Transpose transA, ALPHA alpha, const TrMatrix<MA> &A,
       GeMatrix<MB> &B);

template <typename E, typename S>
    void
    trsv(Transpose transpose,
         const TrMatrix<S> &A,
         DenseVector<E> &xb);

// trsm
template <typename ALPHA, typename MA, typename MB>
    void
    trsm(BlasSide side,
         Transpose transA, ALPHA alpha, const TrMatrix<MA> &A,
         GeMatrix<MB> &B);

} // namespace flens

#include <flens/blas_flens.tcc>

#endif // FLENS_BLAS_FLENS_H
