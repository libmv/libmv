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

#include "libmv/numeric/numeric.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/triangulation.h"
#include "libmv/multiview/fundamental.h"

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
double FundamentalFromCorrespondencesLinear(const Mat &x1,
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
  double smaller_singular_value;
  smaller_singular_value = Nullspace(&A, &f);
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      (*F)(i, j) = f(3 * i + j);
    }
  }
  return smaller_singular_value;
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
double FundamentalFromCorrespondences8Point(const Mat &x1,
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
  double smaller_singular_value;
  smaller_singular_value = 
      FundamentalFromCorrespondencesLinear(x1_normalized, x2_normalized, F);
  EnforceFundamentalRank2Constraint(F);

  // Denormalize the fundamental matrix.
  Mat3 F_T1;
  F_T1 = (*F) * T1; 
  *F = transpose(T2) * F_T1; 

  return smaller_singular_value;
}

void NormalizeFundamental(const Mat3 F, Mat3 *F_normalized) {
  Mat F_tmp;
  F_tmp = F;
  F_tmp /= FrobeniusNorm(F);
  if(F_tmp(2,2) < 0) {
    F_tmp *= -1;
  }
  *F_normalized = F_tmp;
}


// HZ 9.6 pag 257
void EssentialFromFundamental(const Mat3 &F,
                              const Mat3 &K1,
                              const Mat3 &K2,
                              Mat3 *E) {
  Mat3 F_K1;
  F_K1 = F * K1;
  *E = transpose(K2) * F_K1;
}

void RelativeCameraMotion(const Mat3 &R1,
                          const Vec3 &t1,
                          const Mat3 &R2,
                          const Vec3 &t2,
                          Mat3 *R,
                          Vec3 *t) {
  *R = R2 * transpose(R1);
  *t = t2 - (*R) * t1;
}

// HZ 9.6 pag 257
void EssentialFromRt(const Mat3 &R1,
                     const Vec3 &t1,
                     const Mat3 &R2,
                     const Vec3 &t2,
                     Mat3 *E) {
  Mat3 R;
  Vec3 t;
  RelativeCameraMotion(R1, t1, R2, t2, &R, &t);
  Mat3 Tx = CrossProductMatrix(t);
  *E = Tx * R;
}

void MotionFromEssential(const Mat3 &E,
                         std::vector<Mat3> *Rs,
                         std::vector<Vec3> *ts) {
  Mat E_tmp(3,3), U(3,3), Vt(3,3), W(3,3), U_W, U_Wt, U_W_Vt, U_Wt_Vt;
  Vec d(3), u3(3), m_u3(3);
  E_tmp = E;

  SVD(&E_tmp, &d, &U, &Vt);

  // Last column of U is undetermined since d = (a a 0).
  if (DeterminantSlow(U) < 0) {
    U(_, 2) *= -1;
  }
  // Last row of Vt is undetermined since d = (a a 0).
  if (DeterminantSlow(Vt) < 0) {
    Vt(2, _) *= -1;
  }

  u3 = U(0,2), U(1,2), U(2,2);
  m_u3 = -u3;
  W = 0, -1, 0,
      1,  0, 0,
      0,  0, 1;
  // TODO(pau) We need a MatrixMultiply(res, A1, A2, ..., An);
  U_W = U * W;
  U_Wt = U * transpose(W);
  U_W_Vt = U_W * Vt;
  U_Wt_Vt = U_Wt * Vt;

  Rs->resize(4);
  ts->resize(4);
  (*Rs)[0] = U_W_Vt;
  (*ts)[0] = u3;
  (*Rs)[1] = U_W_Vt;
  (*ts)[1] = m_u3;
  (*Rs)[2] = U_Wt_Vt;
  (*ts)[2] = u3;
  (*Rs)[3] = U_Wt_Vt;
  (*ts)[3] = m_u3;
}

int MotionFromEssentialChooseSolution(const std::vector<Mat3> &Rs,
                                      const std::vector<Vec3> &ts,
                                      const Mat3 &K1,
                                      const Vec2 &x1,
                                      const Mat3 &K2,
                                      const Vec2 &x2) {
  assert(Rs.size() == 4);
  assert(ts.size() == 4);

  Mat34 P1, P2;
  Mat3 R1;
  Vec3 t1;
  R1 = Identity(3);
  t1 = 0;
  P_From_KRt(K1, R1, t1, &P1);
  for (int i = 0; i < 4; ++i) {
    const Mat3 &R2 = Rs[i];
    const Vec3 &t2 = ts[i];
    P_From_KRt(K2, R2, t2, &P2);
    Vec3 X;
    TriangulateDLT(P1, x1, P2, x2, &X);
    double d1 = Depth(R1, t1, X);
    double d2 = Depth(R2, t2, X);
    if (d1 > 0 && d2 > 0) {
      return i;
    }
  }
  return -1;
}

bool MotionFromEssentialAndCorrespondence(const Mat3 &E,
                                          const Mat3 &K1,
                                          const Vec2 &x1,
                                          const Mat3 &K2,
                                          const Vec2 &x2,
                                          Mat3 *R,
                                          Vec3 *t) {
  std::vector<Mat3> Rs;
  std::vector<Vec3> ts;
  MotionFromEssential(E, &Rs, &ts);
  int solution = MotionFromEssentialChooseSolution(Rs, ts, K1, x1, K2, x2);
  if (solution >= 0) {
    *R = Rs[solution];
    *t = ts[solution];
    return true;
  }
  else {
    return false;
  }
}

}  // namespace libmv
