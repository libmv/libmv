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

#include <flens/macros.h>

namespace flens {

//-- mv:  y = x*A -> y = A'*x --------------------------------------------------

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans,
   ALPHA alpha, const Vector<VX> &x, const Matrix<MA> &A,
   BETA beta, Vector<VY> &y)
{
    mv(Transpose(Trans^trans), alpha, A.impl(), x.impl(), beta, y.impl());
}

template <typename ALPHA, typename MA, typename T, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<T> > &As,
   const Vector<VX> &x,
   BETA beta, Vector<VY> &y)
{
    mv(trans, alpha*As.right(),  As.left(), x.impl(), beta, y.impl());
}

template <typename ALPHA, typename MA, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &As,
   const Vector<VX> &x,
   BETA beta, Vector<VY> &y)
{
    mv(Transpose(Trans^trans), alpha,  As.left(), x.impl(), beta, y.impl());
}

template <typename ALPHA, typename MA, typename T, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const Matrix<MA> &A,
   const VectorClosure<OpMult, VX, Scalar<T> > &x,
   BETA beta, Vector<VY> &y)
{
    mv(trans, alpha*x.right(),  A.impl(), x.left(), beta, y.impl());
}

template <typename ALPHA, typename MA, typename TA, typename TX, typename VX,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<TA> > &As,
   const VectorClosure<OpMult, VX, Scalar<TX> > &x,
   BETA beta, Vector<VY> &y)
{
    mv(trans, alpha*As.right()*x.right(),  As.left(), x.left(), beta, y.impl());
}

template <typename ALPHA, typename MA, typename VX, typename T,
          typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &As,
   const VectorClosure<OpMult, VX, Scalar<T> > &x,
   BETA beta, Vector<VY> &y)
{
    mv(Transpose(Trans^trans), alpha*x.right(),  As.left(), x.left(), beta, y.impl());
}

//-- mv for symmetric matrices -------------------------------------------------

template <typename I>
    class SymmetricMatrix;

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const SymmetricMatrix<MA> &A, const Vector<VX> &x,
   BETA beta, Vector<VY> &y)
{
    assert(trans==NoTrans);
    mv(alpha, A.impl(), x.impl(), beta, y.impl());
}

//-- mv for triangular matrices ------------------------------------------------

template <typename I>
    class TriangularMatrix;

template <typename ALPHA, typename MA, typename VX, typename BETA, typename VY>
void
mv(Transpose trans, ALPHA alpha,
   const TriangularMatrix<MA> &A, const Vector<VX> &x,
   BETA beta, Vector<VY> &y)
{
    assert(alpha==ALPHA(1));
    assert(beta==BETA(0));
    assert(ADDRESS(x)==ADDRESS(y));
    mv(trans, A.impl(), y.impl());
}

//-- mm ------------------------------------------------------------------------

// C = (a*A)*B
template <typename ALPHA, typename MA, typename T, typename MB,
          typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<T> > &A,
   const Matrix<MB> &B,
   BETA beta, Matrix<MC> &C)
{
    mm(transA, transB, alpha*A.right(), A.left(), B.impl(), beta, C.impl());
}

// C = A'*B
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &A,
   const Matrix<MB> &B,
   BETA beta, Matrix<MC> &C)
{
    mm(Transpose(Trans^transA), transB, alpha,
       A.left(), B.impl(), beta, C.impl());
}

// C = A*(b*B)
template <typename ALPHA, typename MA, typename MB, typename T,
          typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const Matrix<MA> &A,
   const MatrixClosure<OpMult, MB, Scalar<T> > &B,
   BETA beta, Matrix<MC> &C)
{
    mm(transA, transB, alpha*B.right(), A.impl(), B.left(), beta, C.impl());
}

// C = A*B'
template <typename ALPHA, typename MA, typename MB, typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const Matrix<MA> &A,
   const MatrixClosure<OpTrans, MB, MB> &B,
   BETA beta, Matrix<MC> &C)
{
    mm(transA, Transpose(Trans^transB), alpha,
       A.impl(), B.left(), beta, C.impl());
}

// C = (a*A)*(b*B)
template <typename ALPHA,
          typename MA, typename TA,
          typename MB, typename TB,
          typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<TA> > &A,
   const MatrixClosure<OpMult, MB, Scalar<TB> > &B,
   BETA beta, Matrix<MC> &C)
{
    mm(transA, transB, alpha*A.right()*B.right(),
       A.left(), B.left(), beta, C.impl());
}

// C = (a*A)*(B')
template <typename ALPHA,
          typename MA, typename T,
          typename MB,
          typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const MatrixClosure<OpMult, MA, Scalar<T> > &A,
   const MatrixClosure<OpTrans, MB, MB> &B,
   BETA beta, Matrix<MC> &C)
{
    mm(transA, Transpose(Trans^transB), alpha*A.right(),
       A.left(), B.left(), beta, C.impl());
}

// C = (A')*(b*B)
template <typename ALPHA,
          typename MA,
          typename MB, typename T,
          typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &A,
   const MatrixClosure<OpMult, MB, Scalar<T> > &B,
   BETA beta, Matrix<MC> &C)
{
    mm(Transpose(Trans^transA), transB, alpha*B.right(),
       A.left(), B.left(), beta, C.impl());
}

// C = (A')*(B')
template <typename ALPHA,
          typename MA,
          typename MB,
          typename BETA, typename MC>
void
mm(Transpose transA, Transpose transB, ALPHA alpha,
   const MatrixClosure<OpTrans, MA, MA> &A,
   const MatrixClosure<OpTrans, MB, MB> &B,
   BETA beta, Matrix<MC> &C)
{
    mm(Transpose(Trans^transA), Transpose(Trans^transB), alpha,
       A.left(), B.left(), beta, C.impl());
}

//-- copy for closures -------------------------------------------------------

// for vector closures

// y = alpha*x
template <typename VX, typename T, typename VY>
void
copy(const VectorClosure<OpMult, VX, Scalar<T> > &ax, Vector<VY> &y)
{
    copy(ax.left(), y.impl());
    scal(ax.right(), y.impl());
}

// y = x1 + x2
template <typename VL, typename VR, typename VY>
void
copy(const VectorClosure<OpAdd, VL, VR> &x, Vector<VY> &y)
{
    ASSERT(!DebugClosure::search(x.right(), ADDRESS(y)));

    typedef typename VY::ElementType T;

    copy(x.left(), y.impl());
    axpy(T(1.), x.right(), y.impl());
}

// y = x1 - x2
template <typename VL, typename VR, typename VY>
void
copy(const VectorClosure<OpSub, VL, VR> &x, Vector<VY> &y)
{
    ASSERT(!DebugClosure::search(x.right(), ADDRESS(y)));

    typedef typename VY::ElementType T;

    copy(x.left(), y.impl());
    axpy(T(-1), x.right(), y.impl());
}

// y = A*x
template <typename MA, typename VX, typename VY>
void
copy(const VectorClosure<OpMult, MA, VX> &Ax, Vector<VY> &y)
{
    typedef typename VY::ElementType T;
    mv(NoTrans, T(1), Ax.left(), Ax.right(), T(0), y.impl());
}

// for matrix closures

// Y = X'
template <typename MX, typename MY>
void
copy(const MatrixClosure<OpTrans, MX, MX> &X, Matrix<MY> &Y)
{
    copyTrans(X.left(), Y.impl());
}

// Y = conj(X)'
template <typename MX, typename MY>
void
copy(const MatrixClosure<OpConjTrans, MX, MX> &X, Matrix<MY> &Y)
{
    copyConjugateTrans(X.left(), Y.impl());
}

// Y = alpha*X
template <typename MX, typename T, typename MY>
void
copy(const MatrixClosure<OpMult, MX, Scalar<T> > &aX, Matrix<MY> &Y)
{
    copy(aX.left(), Y.impl());
    scal(aX.right(), Y.impl());
}

// Y = X1 + X2
template <typename ML, typename MR, typename MY>
void
copy(const MatrixClosure<OpAdd, ML, MR> &X, Matrix<MY> &Y)
{
    ASSERT(!DebugClosure::search(X.right(), ADDRESS(Y)));

    typedef typename MY::ElementType T;

    copy(X.left(), Y.impl());
    axpy(T(1.), X.right(), Y.impl());
}

// Y = X1 - X2
template <typename ML, typename MR, typename MY>
void
copy(const MatrixClosure<OpSub, ML, MR> &X, Matrix<MY> &Y)
{
    ASSERT(!DebugClosure::search(X.right(), ADDRESS(Y)));

    typedef typename MY::ElementType T;

    copy(X.left(), Y.impl());
    axpy(T(-1), X.right(), Y.impl());
}

// Y = A*B
template <typename MA, typename MB, typename MY>
void
copy(const MatrixClosure<OpMult, MA, MB> &AB, Matrix<MY> &Y)
{
    typedef typename MY::ElementType T;
    mm(NoTrans, NoTrans, T(1), AB.left(), AB.right(), T(0), Y.impl());
}

//-- axpy for closures  --------------------------------------------------------

// for vector closures

// y += alpha*x
template <typename VX, typename T, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpMult, VX, Scalar<T> > &ax, Vector<VY> &y)
{
    axpy(alpha*ax.right(), ax.left(), y.impl());
}

// y += x1 + x2
template <typename VL, typename VR, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpAdd, VL, VR> &x, Vector<VY> &y)
{
    ASSERT(!DebugClosure::search(x.right(), ADDRESS(y)));

    axpy(alpha, x.left(), y.impl());
    axpy(alpha, x.right(), y.impl());
}

// y += x1 - x2
template <typename VL, typename VR, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpSub, VL, VR> &x, Vector<VY> &y)
{
    ASSERT(!DebugClosure::search(x.right(), ADDRESS(y)));

    axpy(alpha, x.left(), y.impl());
    axpy(-alpha, x.right(), y.impl());
}

// y += A*x
template <typename MA, typename VX, typename VY>
void
axpy(typename VY::ElementType alpha,
     const VectorClosure<OpMult, MA, VX> &Ax, Vector<VY> &y)
{
    typedef typename VY::ElementType T;

    mv(NoTrans, alpha, Ax.left(), Ax.right(), T(1), y.impl());
}

// for matrix closures

// Y += alpha*X
template <typename MX, typename T, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpMult, MX, Scalar<T> > &aX, Matrix<MY> &Y)
{
    axpy(alpha*aX.right(), aX.left(), Y.impl());
}

// Y += X1 + X2
template <typename ML, typename MR, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpAdd, ML, MR> &X, Matrix<MY> &Y)
{
    ASSERT(!DebugClosure::search(X.right(), ADDRESS(Y)));

    axpy(alpha, X.left(), Y.impl());
    axpy(alpha, X.right(), Y.impl());
}

// Y += X1 - X2
template <typename ML, typename MR, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpSub, ML, MR> &X, Matrix<MY> &Y)
{
    ASSERT(!DebugClosure::search(X.right(), ADDRESS(Y)));

    axpy(alpha, X.left(), Y.impl());
    axpy(-alpha, X.right(), Y.impl());
}

// Y += A*B
template <typename MA, typename MB, typename MY>
void
axpy(typename MY::ElementType alpha,
     const MatrixClosure<OpMult, MA, MB> &AB, Matrix<MY> &Y)
{
    typedef typename MY::ElementType T;

    mm(NoTrans, NoTrans, T(1), AB.left(), AB.right(), T(1), Y.impl());
}

} // namespace flens
