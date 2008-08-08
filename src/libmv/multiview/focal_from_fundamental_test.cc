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

#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

// Two cameras looking with orthogonal (non-crossing) viewing rays.
// One at the origin and looking to the z direction.
// The other at (1,1,1) and looking down the y direction.
void OrthogonalViewsFundamental(Mat3 *F) {
  Mat3 K1, R1, K2, R2;
  Vec3 t1, t2;
  Mat34 P1, P2;
  K1 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  K2 = K1;
  R1 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  R2 = 1, 0,  0,
       0, 0, -1,
       0, 1,  0;
  t1 = 0, 0, 0;
  t2 = -1, 1, 1;

  P_From_KRt(K1, R1, t1, &P1);
  P_From_KRt(K2, R2, t2, &P2);
  FundamentalFromProjections(P1, P2, F);
}

TEST(FocalFromFundamental, EpipolesFromFundamental) {
  Mat3 F;
  OrthogonalViewsFundamental(&F);

  Vec3 e1, e2;
  EpipolesFromFundamental(F, &e1, &e2);
  e1 /= e1(2);
  e2 /= e2(2);
  EXPECT_NEAR( 1, e1(0), 1e-8);
  EXPECT_NEAR(-1, e1(1), 1e-8);
  EXPECT_NEAR( 1, e1(2), 1e-8);
  EXPECT_NEAR(-1, e2(0), 1e-8);
  EXPECT_NEAR( 1, e2(1), 1e-8);
  EXPECT_NEAR( 1, e2(2), 1e-8);
}

TEST(FocalFromFundamental, RotationToEliminateY) {
  Vec3 a, b;
  a = 1, -1, 0;
  Mat3 T;
  RotationToEliminateY(a, &T);
  b = T * a;
  EXPECT_NEAR(sqrt(2), b(0), 1e-8);
  EXPECT_NEAR(      0, b(1), 1e-8);
  EXPECT_NEAR(      0, b(2), 1e-8);
}

TEST(FocalFromFundamental, FundamentalAlignEpipolesToXAxis) {
  Mat3 F, F_rotated;
  OrthogonalViewsFundamental(&F);
  FundamentalAlignEpipolesToXAxis(F,&F_rotated);
  Vec3 e1, e2;
  EpipolesFromFundamental(F_rotated, &e1, &e2);
  e1 /= e1(2);
  e2 /= e2(2);
  EXPECT_NEAR(sqrt(2), fabs(e1(0)), 1e-8); // The sign of x is undetermined.
                                           // It depends on the sign of z
                                           // before the alignement.
  EXPECT_NEAR(      0,       e1(1), 1e-8); // y coordinate is now 0.
  EXPECT_NEAR(      1,       e1(2), 1e-8);
  EXPECT_NEAR(sqrt(2), fabs(e2(0)), 1e-8);
  EXPECT_NEAR(      0,       e2(1), 1e-8); // y coordinate is now 0.
  EXPECT_NEAR(      1,       e2(2), 1e-8);
}

TEST(FocalFromFundamental, FundamentalShiftPrincipalPoints) {
  // Create two pair of cameras with the same parameters except for the
  // principal point.
  Vec2 p1, p1_new, p2, p2_new;
  Mat3 K1, K1_new, R1, K2, K2_new, R2;
  Vec3 t1, t2;
  Mat34 P1, P1_new, P2, P2_new;
  p1 = 3, 4;
  p1_new = 2, 1;
  p2 = -2, 1;
  p2_new = 4, -3;
  K1 = 1, 0, p1(0),
       0, 1, p1(1),
       0, 0,     1;
  K1_new = 1, 0, p1_new(0),
           0, 1, p1_new(1),
           0, 0,         1;
  K2 = 1, 0, p2(0),
       0, 1, p2(1),
       0, 0,     1;
  K2_new = 1, 0, p2_new(0),
           0, 1, p2_new(1),
           0, 0,         1;
  R1 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  R2 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  t1 = 0, 0, 0;
  t2 = 1, 0, 0;

  P_From_KRt(K1, R1, t1, &P1);
  P_From_KRt(K2, R2, t2, &P2);
  P_From_KRt(K1_new, R1, t1, &P1_new);
  P_From_KRt(K2_new, R2, t2, &P2_new);

  // Compute the fundamental matrix of each camera pair.
  Mat3 F, F_new;
  FundamentalFromProjections(P1, P2, &F);
  FundamentalFromProjections(P1_new, P2_new, &F_new);

  // Shift the principal point of the first fundamental matrix to match the
  // second one.
  Mat3 F_new_computed;
  FundamentalShiftPrincipalPoints(F, p1, p1_new, p2, p2_new, &F_new_computed);

  // TODO(pau) The matrices should be scaled before comparing since they have
  // to be equal only up to scale.
  EXPECT_NEAR(F_new(0,0), F_new_computed(0,0), 1e-8);
  EXPECT_NEAR(F_new(0,1), F_new_computed(0,1), 1e-8);
  EXPECT_NEAR(F_new(0,2), F_new_computed(0,2), 1e-8);
  EXPECT_NEAR(F_new(1,0), F_new_computed(1,0), 1e-8);
  EXPECT_NEAR(F_new(1,1), F_new_computed(1,1), 1e-8);
  EXPECT_NEAR(F_new(1,2), F_new_computed(1,2), 1e-8);
  EXPECT_NEAR(F_new(2,0), F_new_computed(2,0), 1e-8);
  EXPECT_NEAR(F_new(2,1), F_new_computed(2,1), 1e-8);
  EXPECT_NEAR(F_new(2,2), F_new_computed(2,2), 1e-8);
}

TEST(FocalFromFundamental, FocalFromFundamental) {
  Mat3 F;
  Vec2 zero2;
  zero2 = 0, 0;
  double f1, f2;
  OrthogonalViewsFundamental(&F);
  FocalFromFundamental(F, zero2, zero2, &f1, &f2);
  EXPECT_NEAR(1, f1, 1e-8);
  EXPECT_NEAR(1, f2, 1e-8);
}

//TODO(pau) Move this to a proper place.
void HomogeneousToEuclidean(const Mat &H, Mat *X) {
  int d = H.numRows() - 1;
  int n = H.numCols();
  X->resize(d, n);
  for (int i = 0; i < n; ++i) {
    double h = H(d, i);
    for (int j = 0; j < d; ++j) {
      (*X)(j, i) = H(j, i) / h;
    }
  }
}

//TODO(pau) Move this to a proper place.
void EuclideanToHomogeneous(const Mat &X, Mat *H) {
  int d = X.numRows();
  int n = X.numCols();
  H->resize(d + 1, n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < d; ++j) {
      (*H)(j, i) = X(j, i);
    }
    (*H)(d, i) = 1;
  }
}

//TODO(pau) Move this to a proper place.
void Project(const Mat34 &P, const Mat &X, Mat *x) {
  Mat PP, XX, xx;
  PP = P;
  EuclideanToHomogeneous(X, &XX);
  xx = PP * XX;
  HomogeneousToEuclidean(xx, x);
}

TEST(FocalFromFundamental, TwoViewReconstruction) {
  // Two cameras at (0,0,-10) and (2,1,-10) looking towards z+.
  Vec2 p1, p2;
  double f1, f2;
  Mat3 K1, R1, K2, R2;
  Vec3 t1, t2;
  Mat34 P1, P2;
  f1 = 320;
  f2 = 360;
  p1 = 160, 120;
  p2 = 170, 110;
  K1 = f1,  0, p1(0),
        0, f1, p1(1),
        0,  0,     1;
  K2 = f2,  0, p2(0),
        0, f2, p2(1),
        0,  0,     1;
  R1 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  R2 = R1;
  t1 = 0, 0, 10;
  t2 = -2, -1, 10;
  P_From_KRt(K1, R1, t1, &P1);
  P_From_KRt(K2, R2, t2, &P2);

  // The 8 points of a cube and their projections.
  int n = 8;
  Mat X(3,n), x1, x2;
  X = 0, 1, 0, 1, 0, 1, 0, 1,
      0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 1, 1, 1, 1;
  Project(P1, X, &x1);
  Project(P2, X, &x2);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondences8Point(x1, x2, &F_estimated);

  // Compute focal lenght.
  double f1_estimated, f2_estimated;
  FocalFromFundamental(F_estimated, p1, p2, &f1_estimated, &f2_estimated);

  // Build K matrices from the known principal points and the compted focals.
  Mat3 K1_estimated, K2_estimated;
  K1_estimated = f1_estimated,            0, p1(0),
                            0, f1_estimated, p1(1),
                            0,            0,     1;
  K2_estimated = f2_estimated,            0, p2(0),
                            0, f2_estimated, p2(1),
                            0,            0,     1;

  Mat3 F;
  FundamentalFromProjections(P1, P2, &F);
  //std::cout << F << std::endl;
  //std::cout << F_estimated << std::endl;

  //std::cout << K1 << std::endl;
  //std::cout << K1_estimated << std::endl;
  //std::cout << K2 << std::endl;
  //std::cout << K2_estimated << std::endl;

  // TODO: Recover R, t from F and K
}

}
