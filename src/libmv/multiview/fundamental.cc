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
// 

#include "libmv/numeric/numeric.h"

namespace libmv {


void EliminateRow(const Mat34 &P, int row, Mat *X) {
  X->resize(2,4);
  int first_row = (row + 1) % 3;
  int second_row = (row + 2) % 3;
  for (int i = 0; i < 4; ++i) {
    (*X)(0, i) = P(first_row, i);
    (*X)(1, i) = P(second_row, i);
  }
}

// Addapted from vgg_F_from_P.
void FundamentalFromProjections(const Mat34 &P1, const Mat34 &P2, Mat3 *F)
{
  Mat X[3], Y[3], XY;

  for (int i = 0; i < 3; ++i) {
    EliminateRow(P1, i, X + i);
    EliminateRow(P2, i, Y + i);
  }

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      VerticalStack(X[j], Y[i], &XY);
      (*F)(i, j) = DeterminantSlow(XY);
    }
  }
}

// HZ 4.4.4 pag.109
void PreconditionerFromPoints(const Mat &points, Mat3 *T) {
  Vec mean, variance;
  MeanAndVarianceAlongRows(points, &mean, &variance);

  double xfactor = sqrt(2 / variance(0));
  double yfactor = sqrt(2 / variance(1));

  *T = xfactor, 0, -xfactor * mean(0),
       0, yfactor, -yfactor * mean(1),
       0, 0, 1;
}

// TODO(pau) this can be done by matrix multiplication.
void ApplyTransformationToPoints(const Mat &points,
                                 const Mat3 &T,
                                 Mat *transformed_points) {
  int n = points.numCols();
  transformed_points->resize(2,n);
  for (int i = 0; i < n; ++i) {
    Vec3 in, out;
    in = points(0, i), points(1, i), 1;
    out = T * in;
    (*transformed_points)(0, i) = out(0);
    (*transformed_points)(1, i) = out(1);
  }
}

// HZ 11.1 pag.279 (x1 = x, x2 = x')
void FundamentalFromCorrespondencesLinear(const Mat &x1,
                                          const Mat &x2,
                                          Mat3 *F) {
  assert(2 == x1.numRows());
  assert(8 <= x1.numCols());
  assert(x1.numRows() == x2.numRows());
  assert(x1.numCols() == x2.numCols());

  int n = x1.numCols();
  Mat A(n, 9);
  for (int i = 0; i < n; ++i) {
    A(i, 0) = x2(0, i) * x1(0, i);
    A(i, 1) = x2(0, i) * x1(1, i);
    A(i, 2) = x2(0, i);
    A(i, 3) = x2(1, i) * x1(0, i);
    A(i, 4) = x2(1, i) * x1(1, i);
    A(i, 5) = x2(1, i);
    A(i, 6) = x1(0, i);
    A(i, 7) = x1(1, i);
    A(i, 8) = 1;
  }
  Vec f;
  Nullspace(&A, &f);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      (*F)(i, j) = f(3 * i + j);
    }
  }
}

// HZ 11.1.1 pag.280
void EnforceFundamentalRank2Constraint(Mat3 *F) {
  Mat U, Vt, d_Vt, U_d_Vt;
  Vec d;
  SVD(F, &d, &U, &Vt);
  d(2) = 0;
  d_Vt = Diag(d) * Vt;
  U_d_Vt = U * d_Vt;
  *F = U_d_Vt;
}

// HZ 11.2 pag.281 (x1 = x, x2 = x')
void FundamentalFromCorrespondences8Point(const Mat &x1,
                                          const Mat &x2,
                                          Mat3 *F) {
  assert(2 == x1.numRows());
  assert(8 <= x1.numCols());
  assert(x1.numRows() == x2.numRows());
  assert(x1.numCols() == x2.numCols());

  // Normalize the data.
  Mat3 T1, T2;
  PreconditionerFromPoints(x1, &T1);
  PreconditionerFromPoints(x2, &T2);
  Mat x1_normalized, x2_normalized;
  ApplyTransformationToPoints(x1, T1, &x1_normalized);
  ApplyTransformationToPoints(x2, T2, &x2_normalized);

  // Estimate the fundamental matrix.
  FundamentalFromCorrespondencesLinear(x1_normalized, x2_normalized, F);
  EnforceFundamentalRank2Constraint(F);

  // Denormalize the fundamental matrix.
  Mat3 F_T1;
  F_T1 = (*F) * T1; 
  *F = transpose(T2) * F_T1; 
}

}  // namespace libmv
