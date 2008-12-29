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
// Matrix and vector classes, based on Eigen2.
//
// Avoid using Eigen2 classes directly; instead typedef them here.

#ifndef LIBMV_NUMERIC_NUMERIC_H
#define LIBMV_NUMERIC_NUMERIC_H

#include <Eigen/Core>
#include <Eigen/SVD>
#include <Eigen/LU>
#include <Eigen/Cholesky>
#include <Eigen/Geometry>

#include <iostream>

namespace libmv {

typedef Eigen::MatrixXd Mat;
typedef Eigen::VectorXd Vec;

typedef Eigen::Matrix<double, 2, 2> Mat2;
typedef Eigen::Matrix<double, 3, 3> Mat3;
typedef Eigen::Matrix<double, 3, 4> Mat34;
typedef Eigen::Matrix<double, 3, 5> Mat35;
typedef Eigen::Matrix<double, 4, 3> Mat43;
typedef Eigen::Matrix<double, 4, 4> Mat4;
typedef Eigen::Matrix<double, 4, 6> Mat46;

typedef Eigen::Matrix<double, 2, Eigen::Dynamic> Mat2X;
typedef Eigen::Matrix<double, 3, Eigen::Dynamic> Mat3X;
typedef Eigen::Matrix<double, 4, Eigen::Dynamic> Mat4X;
typedef Eigen::Matrix<double, Eigen::Dynamic, 2> MatX2;
typedef Eigen::Matrix<double, Eigen::Dynamic, 3> MatX3;
typedef Eigen::Matrix<double, Eigen::Dynamic, 4> MatX4;

typedef Eigen::Vector2d Vec2;
typedef Eigen::Vector3d Vec3;
typedef Eigen::Vector4d Vec4;
typedef Eigen::Matrix<double, 5, 1> Vec5;

typedef Eigen::Vector2f Vec2f;
typedef Eigen::Vector3f Vec3f;
typedef Eigen::Vector4f Vec4f;

typedef Eigen::VectorXi VecXi;

typedef Eigen::Vector2i Vec2i;
typedef Eigen::Vector3i Vec3i;
typedef Eigen::Vector4i Vec4i;

// Find U, s, and VT such that
//
//   A = U * diag(s) * VT
//
template <typename TMat, typename TVec>
inline void SVD(TMat *A, Vec *s, Mat *U, Mat *VT) {
  assert(0);
}

// Solve the linear system Ax = 0 via SVD. Store the solution in x, such that
// ||x|| = 1.0. Return the singluar value corresponding to the solution.
// Destroys A and resizes x if necessary.
template <typename TMat, typename TVec>
double Nullspace(TMat *A, TVec *nullspace) {
  if (A->rows() >= A->cols()) {
    Eigen::SVD<TMat> svd(*A);
    nullspace->set(svd.matrixV().col(A->cols()-1));
    return svd.singularValues()(A->cols()-1);
  }
  // Extend A with rows of zeros to make it square. It's a hack, but is
  // necessary until Eigen supports SVD with more columns than rows.
  Mat A_extended(A->cols(), A->cols());
  A_extended.block(A->rows(), 0, A->cols() - A->rows(), A->cols()).setZero();
  A_extended.block(0,0, A->rows(), A->cols()) = (*A);
  return Nullspace(&A_extended, nullspace);
}

// Solve the linear system Ax = 0 via SVD. Finds two solutions, x1 and x2, such
// that x1 is the best solution and x2 is the next best solution (in the L2
// norm sense). Store the solution in x1 and x2, such that ||x|| = 1.0. Return
// the singluar value corresponding to the solution x1.  Destroys A and resizes
// x if necessary.
template <typename TMat, typename TVec1, typename TVec2>
double Nullspace2(TMat *A, TVec1 *x1, TVec2 *x2) {
  if (A->rows() >= A->cols()) {
    Eigen::SVD<TMat> svd(*A);
    Mat V = svd.matrixV();
    x1->set(V.col(A->cols()-1));
    x2->set(V.col(A->cols()-2));
    return svd.singularValues()(A->cols()-1);
  }
  // Extend A with rows of zeros to make it square. It's a hack, but is
  // necessary until Eigen supports SVD with more columns than rows.
  Mat A_extended(A->cols(), A->cols());
  A_extended.block(A->rows(), 0, A->cols() - A->rows(), A->cols()).setZero();
  A_extended.block(0,0, A->rows(), A->cols()) = (*A);
  return Nullspace2(&A_extended, x1, x2);
}

// In place transpose for square matrices.
template<class TA>
inline void TransposeInPlace(TA *A) {
  *A = A->transpose().eval();
}

template<typename TVec>
inline double NormL1(const TVec &x) {
  return x.cwise().abs().sum();
}

template<typename TVec>
inline double NormL2(const TVec &x) {
  return x.norm();
}

template<typename TVec>
inline double NormLInfinity(const TVec &x) {
  return x.cwise().abs().maxCoeff();
}

template<typename TVec>
inline double DistanceL1(const TVec &x, const TVec &y) {
  return (x - y).cwise().abs().sum();
}

template<typename TVec>
inline double DistanceL2(const TVec &x, const TVec &y) {
  return (x - y).norm();
}
template<typename TVec>
inline double DistanceLInfinity(const TVec &x, const TVec &y) {
  return (x - y).cwise().abs().maxCoeff();
}

// Normalize a vector with the L1 norm, and return the norm before it was
// normalized.
template<typename TVec>
inline double NormalizeL1(TVec *x) {
  double norm = NormL1(*x);
  *x /= norm;
  return norm;
}

// Normalize a vector with the L2 norm, and return the norm before it was
// normalized.
template<typename TVec>
inline double NormalizeL2(TVec *x) {
  double norm = NormL2(*x);
  *x /= norm;
  return norm;
}

// Normalize a vector with the L^Infinity norm, and return the norm before it
// was normalized.
template<typename TVec>
inline double NormalizeLInfinity(TVec *x) {
  double norm = NormLInfinity(*x);
  *x /= norm;
  return norm;
}

// Return the square of a number.
template<typename T>
inline T Square(T x) {
  return x * x;
}
  
Mat Identity(int n);

Mat3 RotationAroundX(double angle);
Mat3 RotationAroundY(double angle);
Mat3 RotationAroundZ(double angle);

// Return a diagonal matrix from a vector containg the diagonal values.
template <typename TVec>
inline Mat Diag(const TVec &x) {
  return x.asDiagonal();
}

// Return the determinant of A computed by LU factorization.  A is destroyed.
//double DeterminantLU(Mat *A);

template <typename TMat>
inline double Determinant(const TMat &A) {
  return A.determinant();
}

template <typename TMatA, typename TMatI>
inline void Inverse(const TMatA &A, TMatI *I) {
  I->set(A.inverse());
}

template<typename TMat>
inline double FrobeniusNorm(const TMat &A) {
  return sqrt(A.cwise().abs2().sum());
}

template<typename TMat>
inline double FrobeniusDistance(const TMat &A, const TMat &B) {
  return FrobeniusNorm(A - B);
}

inline Vec3 CrossProduct(const Vec3 &x, const Vec3 &y) {
  return x.cross(y);
}

Mat3 CrossProductMatrix(const Vec3 &x);

void MeanAndVarianceAlongRows(const Mat &A,
                              Vec *mean_pointer,
                              Vec *variance_pointer);

void HorizontalStack(const Mat &left, const Mat &right, Mat *stacked);

template<typename TTop, typename TBot, typename TStacked>
void VerticalStack(const TTop &top, const TBot &bottom, TStacked *stacked) {
  assert(top.cols() == bottom.cols());
  int n1 = top.rows();
  int n2 = bottom.rows();
  int m = top.cols();

  stacked->resize(n1 + n2, m);
  stacked->block(0,  0, n1, m) = top;
  stacked->block(n1, 0, n2, m) = bottom;
}

void MatrixColumn(const Mat &A, int i, Vec2 *v);
void MatrixColumn(const Mat &A, int i, Vec3 *v);
void MatrixColumn(const Mat &A, int i, Vec4 *v);

template <typename TMat, typename TCols>
TMat ExtractColumns(const TMat &A, TCols columns) {
  TMat compressed(A.rows(), columns.size());
  for (int i = 0; i < columns.size(); ++i) {
    compressed.col(i) = A.col(columns[i]);
  }
  return compressed;
}

}  // namespace mv

#endif  // LIBMV_NUMERIC_NUMERIC_H
