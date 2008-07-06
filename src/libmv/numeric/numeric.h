// Copyright (c) 2007, 2008 libmv authors.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// Matrix and vector classes, based on FLENS.
//
// Avoid using flens classes directly; instead typedef them here.

#ifndef LIBMV_NUMERIC_NUMERIC_H
#define LIBMV_NUMERIC_NUMERIC_H

#include "third_party/flens/flens.h"

namespace libmv {

// Caveat: These use 1-indexing by default.
typedef flens::GeMatrix<flens::FullStorage<double, flens::ColMajor> > Mat;
typedef flens::DenseVector<flens::Array<double> >                     Vec;

// Do not use TinyVector<> directly; if needed, add more definitions here.
typedef flens::TinyVector<double, 2>     Vec2;
typedef flens::TinyVector<double, 3>     Vec3;
typedef flens::TinyVector<double, 4>     Vec4;
typedef flens::TinyMatrix<double, 3, 4>  Mat34;
typedef flens::TinyMatrix<double, 2, 2>  Mat2;
typedef flens::TinyMatrix<double, 3, 3>  Mat3;
typedef flens::TinyMatrix<double, 4, 4>  Mat4;

// For matrix and vector views. Example: A(_(1,3), (4,5)) is a 3x2 submatrix
// of A, that can be manipulated (changes underlying A).
using flens::_;

// Find U, s, and VT such that 
//   
//   A = U * diag(s) * VT
// 
// Destroys A.
inline void SVD(Mat *A, Vec *s, Mat *U, Mat *VT) {
  svd(*A, *s, *U, *VT);
}

// Specialization for tiny matrices.
template<class T, int M, int N>
inline void SVD(flens::TinyMatrix<T, M, N> *A, Vec *s, Mat *U, Mat *VT) {
  // TODO(keir): It's possible to eliminate this copying by pushing SVD support
  // for tiny matrices and vectors into FLENS.
  Mat Ap(M,N);
  Ap = *A;
  svd(Ap, *s, *U, *VT);
}

// Solve the linear system Ax = 0 via SVD. Store the solution in x, such that
// ||x|| = 1.0. Return the singluar value corresponding to the solution.
// Destroys A and resizes x if necessary.
template<class TMat, class TVec>
inline double Nullspace(TMat *A, TVec *x)
{
	int m = A->numRows();
	int n = A->numCols();
  Mat U, VT;
  Vec s;

  SVD(A, &s, &U, &VT);

  x->resize(n);
  *x = VT(n, _);
	return s(std::min(n,m));
}

// In place transpose for square matrices.
template<class T, int N>
inline void Transpose(flens::TinyMatrix<T, N, N> *A) {
  for (int i = 0; i < N; ++i) {
    for (int j = i+1; j < N; ++j) {
      std::swap((*A)(i,j), (*A)(j,i));
    }
  }
}

// Normalize a vector with the L1 norm, and return the norm before it was
// normalized.
template<typename TVec>
inline double NormalizeL1(TVec *x) {
  double sum = 0;
  for (int i = x->firstIndex(); i <= x->lastIndex(); ++i) {
    sum += fabs((*x)(i));
  }
  *x /= sum;
  return sum;
}

// Normalize a vector with the L2 norm, and return the norm before it was
// normalized.
template<typename TVec>
inline double NormalizeL2(TVec *x) {
  double sum = 0;
  for (int i = x->firstIndex(); i <= x->lastIndex(); ++i) {
    sum += (*x)(i)*(*x)(i);
  }
  double l2 = sqrt(sum);
  *x /= l2;
  return l2;
}

}  // namespace mv

#endif  // LIBMV_NUMERIC_NUMERIC_H
