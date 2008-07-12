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

#include <flens/densevector.h>

namespace flens {

//== Low-Level BLAS: Fixed Dimensions ==========================================

//-- Level 1 -------------------------------------------------------------------

template <int N, typename TX, int incX, typename TY, int incY>
void
copy(const TX *x, TY *y)
{
    for (int i=0, j=0; i<N; i+=incX, j+=incY) {
        y[j] = x[i];
    }
}

template <int M, int N, typename TX, typename TY>
void
copy(const TX x[M][N], TY y[M][N])
{
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            y[i][j] = x[i][j];
        }
    }
}

template <int N, typename T, typename TX, int incX, typename TY, int incY>
void
axpy(T alpha, const TX *x, TY *y)
{
    for (int i=0, j=0; i<N; i+=incX, j+=incY) {
        y[j] += alpha*x[i];
    }
}

template <int M, int N, typename T, typename TX, typename TY>
void
axpy(T alpha, const TX x[M][N], TY y[M][N])
{
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            y[i][j] += alpha*x[i][j];
        }
    }
}

template <int N, typename TX, int incX, typename TY, int incY>
typename Promotion<TX, TY>::Type
dot(const TX *x, const TY *y)
{
    typedef typename Promotion<TX, TY>::Type Result;
    
    Result result = Result(0);
    for (int i=0, j=0; i<N; i+=incX, j+=incY) {
        result += x[i]*y[j];
    }
    return result;
}

//-- Level 2 -------------------------------------------------------------------

template <typename ALPHA, typename TA, int M, int N, typename TX, int NX,
          typename BETA, typename TY, int NY>
void
gemv(Transpose trans, ALPHA alpha, const TA A[M][N], const TX x[NX],
     BETA beta, TY y[NY])
{
    assert(((trans==NoTrans) && (N==NX) && (M==NY))
        || ((trans==Trans) && (N==NY) && (M==NX)));
        
    if (trans==NoTrans) {
        for (int i=0; i<M; ++i) {
            TY sum = beta*y[i];
            for (int j=0; j<N; ++j) {
                sum += alpha * A[i][j] * x[j];
            }
            y[i] = sum;
        }
    } else {
        for (int j=0; j<N; ++j) {
            TY sum = beta*y[j];
            for (int i=0; i<M; ++i) {
                sum += alpha * A[i][j] * x[i];
            }
            y[j] = sum;
        }
    }
}

//== High-Level BLAS: Tiny Types ===============================================

//-- Level 1 -------------------------------------------------------------------

template <typename X, typename Y>
typename Promotion<typename TypeInfo<X>::ElementType,
                   typename TypeInfo<Y>::ElementType>::Type
dot(const TinyVector<X> &x, const TinyVector<Y> &y)
{
    assert(TypeInfo<X>::length==TypeInfo<Y>::length);

    typedef typename TypeInfo<X>::ElementType TX;
    typedef typename TypeInfo<Y>::ElementType TY;

    return dot<TypeInfo<X>::length,
              TX, TypeInfo<X>::stride,
              TY, TypeInfo<Y>::stride>(x.engine()._data, y.engine()._data);
}

template <typename X, typename Y>
void
copy(const TinyVector<X> &x, TinyVector<Y> &y)
{
    assert(TypeInfo<X>::length==TypeInfo<Y>::length);

    typedef typename TypeInfo<X>::ElementType TX;
    typedef typename TypeInfo<Y>::ElementType TY;

    copy<TypeInfo<X>::length,
         TX, TypeInfo<X>::stride,
         TY, TypeInfo<Y>::stride>(x.engine()._data, y.engine()._data);
}

template <typename X, typename Y>
void
copy(const TinyVector<X> &x, DenseVector<Y> &y)
{
    if (x.length()!=y.length()) {
        y.resize(x.length());
    }

    copy(x.length(), x.data(), x.stride(), y.data(), y.stride());
}

template <typename X, typename Y>
void
copy(const DenseVector<X> &x, TinyVector<Y> &y)
{
    if (x.length()!=y.length()) {
        y.resize(x.length());
    }

    copy(x.length(), x.data(), x.stride(), y.data(), y.stride());
}

template <typename X, typename Y>
void
copy(const TinyGeMatrix<X> &x, TinyGeMatrix<Y> &y)
{
    assert(TypeInfo<X>::numRows==TypeInfo<Y>::numRows);
    assert(TypeInfo<X>::numCols==TypeInfo<Y>::numCols);

    typedef typename TypeInfo<X>::ElementType TX;
    typedef typename TypeInfo<Y>::ElementType TY;

    static const int M = TypeInfo<X>::numRows;
    static const int N = TypeInfo<X>::numCols;

    copy<M, N, TX, TY>(x.engine()._data, y.engine()._data);
}

template <typename X, typename Y>
void
copy(const TinyGeMatrix<X> &x, GeMatrix<Y> &y)
{
    if ((y.numRows()!=x.numRows()) || (y.numCols()!=x.numCols())) {
        y.resize(x.numRows(), x.numCols(),
                 x.firstRow(), x.firstCol());
    }

    for (int i=x.firstRow(), I=y.firstRow(); i<=x.lastRow(); ++i, ++I) {
        for (int j=x.firstCol(), J=y.firstCol(); j<=x.lastCol(); ++j, ++J) {
            y(I,J) = x(i,j);
        }
    }
}

template <typename X, typename Y>
void
copy(const GeMatrix<X> &x, TinyGeMatrix<Y> &y)
{
    if ((y.numRows()!=x.numRows()) || (y.numCols()!=x.numCols())) {
        y.resize(x.numRows(), x.numCols(),
                 x.firstRow(), x.firstCol());
    }

    for (int i=x.firstRow(), I=y.firstRow(); i<=x.lastRow(); ++i, ++I) {
        for (int j=x.firstCol(), J=y.firstCol(); j<=x.lastCol(); ++j, ++J) {
            y(I,J) = x(i,j);
        }
    }
}

template <typename T, typename X, typename Y>
void
axpy(T alpha, const TinyVector<X> &x, TinyVector<Y> &y)
{
    assert(TypeInfo<X>::length==TypeInfo<Y>::length);

    typedef typename TypeInfo<X>::ElementType TX;
    typedef typename TypeInfo<Y>::ElementType TY;

    axpy<TypeInfo<X>::length, T,
         TX, TypeInfo<X>::stride,
         TY, TypeInfo<Y>::stride>(alpha, x.engine()._data, y.engine()._data);
}

template <typename T, typename X, typename Y>
void
axpy(T alpha, const TinyGeMatrix<X> &x, TinyGeMatrix<Y> &y)
{
    assert(TypeInfo<X>::numRows==TypeInfo<Y>::numRows);
    assert(TypeInfo<X>::numCols==TypeInfo<Y>::numCols);

    typedef typename TypeInfo<X>::ElementType TX;
    typedef typename TypeInfo<Y>::ElementType TY;

    static const int M = TypeInfo<X>::numRows;
    static const int N = TypeInfo<X>::numCols;

    axpy<M, N, T, TX, TY>(alpha, x.engine()._data, y.engine()._data);
}

//-- Level 2 -------------------------------------------------------------------

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans,
   ALPHA alpha, const TinyGeMatrix<MA> &A, const TinyVector<VX> &x,
   BETA beta, TinyVector<VY> &y)
{
    typedef typename TypeInfo<MA>::ElementType TA;
    typedef typename TypeInfo<VX>::ElementType TX;
    typedef typename TypeInfo<VY>::ElementType TY;

    static const int M = TypeInfo<MA>::numRows;
    static const int N = TypeInfo<MA>::numCols;
    static const int NX = TypeInfo<VX>::length;
    static const int NY = TypeInfo<VY>::length;
    
    gemv<ALPHA, TA, M, N, TX, NX, BETA, TY, NY>(trans, alpha, A.engine()._data,
                                                x.engine()._data, beta,
                                                y.engine()._data);
}

} // namespace flens
