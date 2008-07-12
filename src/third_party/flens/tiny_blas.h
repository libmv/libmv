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

#ifndef FLENS_TINY_BLAS_H
#define FLENS_TINY_BLAS_H 1

namespace flens {

//== Low-Level BLAS: Fixed Dimensions ==========================================

//-- Level 1 -------------------------------------------------------------------

template <int N, typename TX, int incX, typename TY, int incY>
    void
    copy(const TX *x, TY *y);
    
template <int M, int N, typename TX, typename TY>
    void
    copy(const TX x[M][N], TY y[M][N]);

template <int N, typename T, typename TX, int incX, typename TY, int incY>
    void
    axpy(T alpha, const TX *x, TY *y);

template <int M, int N, typename T, typename TX, typename TY>
    void
    axpy(T alpha, const TX x[M][N], TY y[M][N]);

template <int N, typename TX, int incX, typename TY, int incY>
    typename Promotion<TX, TY>::Type
    dot(const TX *x, const TY *y);

//== High-Level BLAS: Tiny Types ===============================================

template <typename I>
    class TinyVector;

template <typename I>
    class TinyGeMatrix;

template <typename I>
    class DenseVector;

template <typename I>
    class GeMatrix;

//-- Level 1 -------------------------------------------------------------------

template <typename X, typename Y>
    typename Promotion<typename TypeInfo<X>::ElementType,
                       typename TypeInfo<Y>::ElementType>::Type
    dot(const TinyVector<X> &x, const TinyVector<Y> &y);

template <typename X, typename Y>
    void
    copy(const TinyVector<X> &x, TinyVector<Y> &y);

template <typename X, typename Y>
    void
    copy(const TinyVector<X> &x, DenseVector<Y> &y);

template <typename X, typename Y>
    void
    copy(const DenseVector<X> &x, TinyVector<Y> &y);

template <typename X, typename Y>
    void
    copy(const TinyGeMatrix<X> &x, TinyGeMatrix<Y> &y);

template <typename X, typename Y>
    void
    copy(const TinyGeMatrix<X> &x, GeMatrix<Y> &y);

template <typename X, typename Y>
    void
    copy(const GeMatrix<X> &x, TinyGeMatrix<Y> &y);

template <typename T, typename X, typename Y>
    void
    axpy(T alpha, const TinyVector<X> &x, TinyVector<Y> &y);

template <typename T, typename X, typename Y>
    void
    axpy(T alpha, const TinyGeMatrix<X> &x, TinyGeMatrix<Y> &y);

} // namespace flens

#include <flens/tiny_blas.tcc>

#endif // FLENS_TINY_BLAS_H
