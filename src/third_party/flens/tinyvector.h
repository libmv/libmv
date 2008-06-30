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

#ifndef FLENS_TINYVECTOR_H
#define FLENS_TINYVECTOR_H 1

#include <flens/matvec.h>

namespace flens {

// == TinyVector ==============================================================

template <typename T, int N>
class TinyVector
    : public Vector<TinyVector<T, N> >
{
    public:
        TinyVector()
        {
        }

        // -- operators --------------------------------------------------------

        TinyVector<T, N> &
        operator=(const TinyVector<T, N> &rhs)
        {
            copy(rhs, *this);
            return *this;
        }

        template <typename RHS>
            TinyVector<T, N> &
            operator=(const Vector<RHS> &rhs)
            {
                copy(rhs.impl(), *this);
                return *this;
            }

        template <typename RHS>
            TinyVector<T, N> &
            operator+=(const Vector<RHS> &rhs)
            {
                axpy(T(1), rhs.impl(), *this);
                return *this;
            }

        template <typename RHS>
            TinyVector<T, N> &
            operator-=(const Vector<RHS> &rhs)
            {
                axpy(T(1), rhs.impl(), *this);
                return *this;
            }

        const T &
        operator()(int index) const
        {
            assert(index>=0);
            assert(index<N);

            return _data[index];
        }

        T &
        operator()(int index)
        {
            assert(index>=0);
            assert(index<N);

            return _data[index];
        }

        T _data[N];
};

template <typename T, int N>
struct TypeInfo<TinyVector<T, N> >
{
    typedef TinyVector<T, N> Impl;
    typedef T ElementType;
};

template <typename XT, typename YT, int N>
void
copy(const TinyVector<XT,N> &x, TinyVector<YT,N> &y)
{
    for (int i=0; i<N; ++i) {
        y._data[i] = x._data[i];
    }
}

template <typename XT, typename YT, int N>
typename Promotion<XT, YT>::Type
dot(const TinyVector<XT,N> &x, const TinyVector<YT,N> &y)
{
    typedef typename Promotion<XT,YT>::Type T;
    T result = T(0);
    for (int i=0; i<N; ++i) {
        result += y._data[i] * x._data[i];
    }
    return result;
}

template <typename T, typename XT, typename YT, int N>
void
axpy(T alpha, const TinyVector<XT,N> &x, TinyVector<YT,N> &y)
{
    for (int i=0; i<N; ++i) {
        y._data[i] += alpha*x._data[i];
    }
}

// == TinyVector ==============================================================
 
template <typename T, int M, int N>
class TinyMatrix
    : public GeneralMatrix<TinyMatrix<T,M,N> >
{
    public:
        // -- constructors -----------------------------------------------------
        TinyMatrix()
        {
        }

        // -- operators --------------------------------------------------------
        TinyMatrix<T,M,N> &
        operator=(const TinyMatrix<T,M,N> &rhs)
        {
            copy(rhs, *this);
            return *this;
        }

        template <typename RHS>
            TinyMatrix<T,M,N> &
            operator=(const Matrix<RHS> &rhs)
            {
                copy(rhs.impl(), *this);
                return *this;
            }

        TinyMatrix<T,M,N> &
        operator+=(const TinyMatrix<T,M,N> &rhs)
        {
            axpy(T(1), rhs, *this);
            return *this;
        }

        template <typename RHS>
            TinyMatrix<T,M,N> &
            operator+=(const Matrix<RHS> &rhs)
            {
                axpy(T(1), rhs.impl(), *this);
                return *this;
            }

        TinyMatrix<T,M,N> &
        operator-=(const TinyMatrix<T,M,N> &rhs)
        {
            axpy(T(-1), rhs, *this);
            return *this;
        }

        template <typename RHS>
            TinyMatrix<T,M,N> &
            operator-=(const Matrix<RHS> &rhs)
            {
                axpy(T(-1), rhs, *this);
                return *this;
            }

        const T &
        operator()(int row, int col) const
        {
            assert(row>=0);
            assert(col>=0);
            assert(row<M);
            assert(col<N);
            return _data[row][col];
        }

        T &
        operator()(int row, int col)
        {
            assert(row>=0);
            assert(col>=0);
            assert(row<M);
            assert(col<N);
            return _data[row][col];
        }

        // -- methods ----------------------------------------------------------
        int
        leadingDimension() const
        {
            return N;
        }

        T _data[M][N];
};

template <typename T, int M, int N>
struct TypeInfo<TinyMatrix<T,M,N> >
{
    typedef TinyMatrix<T,M,N> Impl;
    typedef T                 ElementType;
};

template <typename XT, typename YT, int M, int N>
void
copy(const TinyMatrix<XT,M,N> &x, TinyMatrix<YT,M,N> &y)
{
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            y._data[i][j] = x._data[i][j];
        }
    }
}

template <typename T, typename XT, typename YT, int M, int N>
void
axpy(T alpha, const TinyMatrix<XT,M,N> &x, TinyMatrix<YT,M,N> &y)
{
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            y._data[i][j] += alpha*x._data[i][j];
        }
    }
}

template <typename ALPHA, typename AT, int M, int N, typename XT, int XN,
          typename BETA, typename YT, int YN>
void
mv(Transpose trans,
   const ALPHA alpha, const TinyMatrix<AT,M,N> &A, const TinyVector<XT,XN> &x,
   const BETA beta, TinyVector<YT,YN> &y)
{
    assert(((trans==NoTrans) && (N==XN) && (M==YN))
        || ((trans==Trans)   && (M==XN) && (N==YN))
    );
    
    if (trans==NoTrans) {
        for (int i=0; i<M; ++i) {
            YT sum = beta*y._data[i];
            for (int j=0; j<N; ++j) {
                sum += alpha * A._data[i][j] * x._data[j];
            }
            y._data[i] = sum;
        }
    } else {
        for (int j=0; j<N; ++j) {
            YT sum = beta*y._data[j];
            for (int i=0; i<M; ++i) {
                sum += alpha * A._data[i][j] * x._data[i];
            }
            y._data[j] = sum;
        }
    }
}

} // namespace flens

#endif // FLENS_TINYVECTOR_H
