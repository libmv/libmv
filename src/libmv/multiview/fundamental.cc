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
// http://axiom.anu.edu.au/~hartley/Papers/focal-lengths/focal.pdf

#include "libmv/numeric/numeric.h"

namespace libmv {

//TODO(pau) Find the flens function for this or move to numeric.h.
Mat Diag(const Vec &x) {
  int n = x.length();
  Mat A(n,n);
  A = 0;
  for (int i = 0; i < n; ++i) {
    A(i,i) = x(i);
  }
  return A;
}

void MeanAndVariancesFromPoints(const std::vector<Vec2> &points,
                                double *meanx,
                                double *meany,
                                double *varx,
                                double *vary) {
  int n = points.size();
  double sumx = 0, sumx2 = 0;
  double sumy = 0, sumy2 = 0;
  for (int i = 0; i < n; ++i) {
    double x = points[i](0);
    double y = points[i](1);
    sumx += x;
    sumx2 += x * x;
    sumy += y;
    sumy2 += y * y;
  }
  *meanx = sumx / n;
  *meany = sumy / n;
  *varx = sumx2 / n - Square(*meanx);
  *vary = sumy2 / n - Square(*meany);
}

// HZ 4.4.4 pag.109
void PreconditionerFromPoints(const std::vector<Vec2> &points, Mat3 *T) {
  double meanx, meany, varx, vary;
  MeanAndVariancesFromPoints(points, &meanx, &meany, &varx, &vary);

  double xfactor = sqrt(2 / varx);
  double yfactor = sqrt(2 / vary);

  *T = xfactor, 0, -xfactor * meanx,
       0, yfactor, -yfactor * meany,
       0, 0, 1;
}

void ApplyTransformationToPoints(const std::vector<Vec2> &points,
                                 const Mat3 &T,
                                 std::vector<Vec2> *transformed_points) {
  int n = points.size();
  transformed_points->resize(n);
  for (int i = 0; i < n; ++i) {
    Vec3 in, out;
    in = points[i](0), points[i](1), 1;
    out = T * in;
    (*transformed_points)[i] = out(0), out(1);
  }
}

// HZ 11.1 pag.279
void FundamentalFromCorrespondencesLinear(const std::vector<Vec2> &x1,
                                          const std::vector<Vec2> &x2,
                                          Mat3 *F) {
  assert(8 <= x1.size());
  assert(x1.size() == x2.size());

  int n = x1.size();
  Mat A(n, 9);
  for (int i = 0; i < n; ++i) {
    A(i, 0) = x1[i](0) * x2[i](0);
    A(i, 1) = x1[i](0) * x2[i](1);
    A(i, 2) = x1[i](0);
    A(i, 3) = x1[i](1) * x2[i](0);
    A(i, 4) = x1[i](1) * x2[i](1);
    A(i, 5) = x1[i](1);
    A(i, 6) = x2[i](0);
    A(i, 7) = x2[i](1);
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

// HZ 11.2 pag.281
void FundamentalFromCorrespondences8Point(const std::vector<Vec2> &x1,
                                          const std::vector<Vec2> &x2,
                                          Mat3 *F) {
  assert(8 <= x1.size());
  assert(x1.size() == x2.size());

  // Normalize the data.
  // TODO

  FundamentalFromCorrespondencesLinear(x1, x2, F);
  EnforceFundamentalRank2Constraint(F);

  // Denormalize the fundamental matrix.
  // TODO
}

}  // namespace libmv
