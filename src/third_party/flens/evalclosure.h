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

#ifndef FLENS_EVALCLOSURE_H
#define FLENS_EVALCLOSURE_H 1

#include <flens/matvecclosures.h>

namespace flens {

//-- mv:  y = x*A -> y = A'*x --------------------------------------------------

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans,
   ALPHA alpha, const Vector<VX> &x, const Matrix<MA> &A,
   BETA beta, Vector<VY> &y);

template <typename ALPHA, typename MA, typename T, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<T> > &As,
   const Vector<VX> &x,
   BETA beta, Vector<VY> &y);

template <typename ALPHA, typename MA, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &As,
   const Vector<VX> &x,
   BETA beta, Vector<VY> &y);

template <typename ALPHA, typename MA, typename T, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const Matrix<MA> &A,
   const VectorClosure<OpMult, VX, Scalar<T> > &x,
   BETA beta, Vector<VY> &y);

template <typename ALPHA, typename MA, typename TA, typename TX, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<TA> > &As,
   const VectorClosure<OpMult, VX, Scalar<TX> > &x,
   BETA beta, Vector<VY> &y);

template <typename ALPHA, typename MA, typename VX, typename T,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &As,
   const VectorClosure<OpMult, VX, Scalar<T> > &x,
   BETA beta, Vector<VY> &y);

//-- mv for symmetric matrices -------------------------------------------------

template <typename I>
    class SymmetricMatrix;

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const SymmetricMatrix<MA> &A, const Vector<VX> &x,
   BETA beta, Vector<VY> &y);

//-- mv for triangular matrices ------------------------------------------------

template <typename I>
    class TriangularMatrix;

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const TriangularMatrix<MA> &A, const Vector<VX> &x,
   BETA beta, Vector<VY> &y);

//-- mm ------------------------------------------------------------------------

// C = (a*A)*B
template <typename ALPHA, typename MA, typename T, typename MB,
          typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const MatrixClosure<OpMult, MA, Scalar<T> > &A,
       const Matrix<MB> &B,
       BETA beta, Matrix<MC> &C);

// C = A'*B
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const MatrixClosure<OpTrans, MA, MA> &A,
       const Matrix<MB> &B,
       BETA beta, Matrix<MC> &C);


// C = A*(b*B)
template <typename ALPHA, typename MA, typename MB, typename T,
          typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const Matrix<MA> &A,
       const MatrixClosure<OpMult, MB, Scalar<T> > &B,
       BETA beta, Matrix<MC> &C);

// C = A*B'
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const Matrix<MA> &A,
       const MatrixClosure<OpTrans, MB, MB> &B,
       BETA beta, Matrix<MC> &C);

// C = (a*A)*(b*B)
template <typename ALPHA,
          typename MA, typename TA,
          typename MB, typename TB,
          typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const MatrixClosure<OpMult, MA, Scalar<TA> > &A,
       const MatrixClosure<OpMult, MB, Scalar<TB> > &B,
       BETA beta, Matrix<MC> &C);

// C = (a*A)*(B')
template <typename ALPHA,
          typename MA, typename T,
          typename MB,
          typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const MatrixClosure<OpMult, MA, Scalar<T> > &A,
       const MatrixClosure<OpTrans, MB, MB> &B,
       BETA beta, Matrix<MC> &C);

// C = (A')*(b*B)
template <typename ALPHA,
          typename MA,
          typename MB, typename T,
          typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const MatrixClosure<OpTrans, MA, MA> &A,
       const MatrixClosure<OpMult, MB, Scalar<T> > &B,
       BETA beta, Matrix<MC> &C);

// C = (A')*(B')
template <typename ALPHA,
          typename MA,
          typename MB,
          typename BETA, typename MC>
    void
    mm(Transpose transA, Transpose transB, ALPHA alpha,
       const MatrixClosure<OpTrans, MA, MA> &A,
       const MatrixClosure<OpTrans, MB, MB> &B,
       BETA beta, Matrix<MC> &C);

//-- copy for closures -------------------------------------------------------

//-- vector closures

// y = alpha*x
template <typename VX, typename T, typename VY>
void
copy(const VectorClosure<OpMult, VX, Scalar<T> > &ax, Vector<VY> &y);

// y = x1 + x2
template <typename VL, typename VR, typename VY>
void
copy(const VectorClosure<OpAdd, VL, VR> &x, Vector<VY> &y);

// y = x1 - x2
template <typename VL, typename VR, typename VY>
void
copy(const VectorClosure<OpSub, VL, VR> &x, Vector<VY> &y);

// y = A*x
template <typename MA, typename VX, typename VY>
void
copy(const VectorClosure<OpMult, MA, VX> &Ax, Vector<VY> &y);

//-- matrix closures

template <typename MX, typename MY>
void
copy(const MatrixClosure<OpTrans, MX, MX> &X, Matrix<MY> &Y);

// Y = alpha*X
template <typename MX, typename T, typename MY>
void
copy(const MatrixClosure<OpMult, MX, Scalar<T> > &aX, Matrix<MY> &Y);

// Y = X1 + X2
template <typename ML, typename MR, typename MY>
void
copy(const MatrixClosure<OpAdd, ML, MR> &X, Matrix<MY> &Y);

// Y = X1 - X2
template <typename ML, typename MR, typename MY>
void
copy(const MatrixClosure<OpSub, ML, MR> &X, Matrix<MY> &Y);

// Y = A*B
template <typename MA, typename MB, typename MC>
void
copy(const MatrixClosure<OpMult, MA, MB> &AB, Matrix<MC> &Y);

//-- axpy for closures ---------------------------------------------------------

//-- vector closures

// y += alpha*x
template <typename VX, typename T, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpMult, VX, Scalar<T> > &ax, Vector<VY> &y);

// y += x1 + x2
template <typename VL, typename VR, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpAdd, VL, VR> &x, Vector<VY> &y);

// y += x1 - x2
template <typename VL, typename VR, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpSub, VL, VR> &x, Vector<VY> &y);

// y += A*x
template <typename MA, typename VX, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpMult, MA, VX> &Ax, Vector<VY> &y);

//-- matrix closures

// Y += alpha*X
template <typename MX, typename T, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpMult, MX, Scalar<T> > &aX, Matrix<MY> &Y);

// Y += X1 + X2
template <typename ML, typename MR, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpAdd, ML, MR> &X, Matrix<MY> &Y);

// Y += X1 - X2
template <typename ML, typename MR, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpSub, ML, MR> &X, Matrix<MY> &Y);

// Y += A*B
template <typename MA, typename MB, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpMult, MA, MB> &AB, Matrix<MY> &Y);

} // namespace flens

#include <flens/evalclosure.tcc>

#endif // FLENS_EVALCLOSURE_H
