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

#include <iostream>

#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/projection.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

#include "libmv/multiview/generated.cc"

namespace {
using namespace libmv;

TEST(Fundamental, FileGenerator) {
  EXPECT_EQ(3, x);
}

TEST(Fundamental, FundamentalFromProjections) {
  // TODO(pau) Write test for FundamentalFromProjections

}

TEST(Fundamental, PreconditionerFromPoints) {
  int n = 4;
  Mat points(2,n);
  points = 0, 0, 1, 1,
           0, 2, 1, 3; 

  Mat3 T;
  PreconditionerFromPoints(points, &T);

  Mat normalized_points;
  ApplyTransformationToPoints(points, T, &normalized_points);

  Vec mean, variance;
  MeanAndVarianceAlongRows(normalized_points, &mean, &variance);

  EXPECT_NEAR(0, mean(0), 1e-8);
  EXPECT_NEAR(0, mean(1), 1e-8);
  EXPECT_NEAR(2, variance(0), 1e-8);
  EXPECT_NEAR(2, variance(1), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondencesLinear) {
  int n = 8;
  Mat x1(2,n);
  x1 = 0, 0, 0, 1, 1, 1, 2, 2,
       0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2(2,n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1,i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondencesLinear(x1, x2, &F);

  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x = x1(0, i), x1(1, i), 1;
    y = x2(0, i), x2(1, i), 1;
    F_x = F * x;
    y_F_x(i) = dot(y, F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences8Point) {
  int n = 8;
  Mat x1(2,n);
  x1 = 0, 0, 0, 1, 1, 1, 2, 2,
       0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2(2,n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1,i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondences8Point(x1, x2, &F);

  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x = x1(0, i), x1(1, i), 1;
    y = x2(0, i), x2(1, i), 1;
    F_x = F * x;
    y_F_x(i) = dot(y, F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);

  EXPECT_NEAR(0, DeterminantSlow(F), 1e-8);
}

// Two cameras at (-1,-1,-10) and (2,1,-10) looking approximately towards z+.
void TwoRealisticCameras(Mat34 *P1, Mat34 *P2) {
  Vec2 p1, p2;
  double f1, f2;
  Mat3 K1, R1, K2, R2;
  Vec3 t1, t2;
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
  R1 = RotationAroundZ(-0.1);
  R2 = RotationAroundX(-0.1);
  t1 = 1, 1, 10;
  t2 = -2, -1, 10;
  P_From_KRt(K1, R1, t1, P1);
  P_From_KRt(K2, R2, t2, P2);
}

TEST(Fundamental, FundamentalFromCorrespondencesLinearRealistic) {
  Mat34 P1, P2;
  TwoRealisticCameras(&P1, &P2);

  // The 8 points and their projections.
  int n = 8;
  Mat X(3,n), x1, x2;
  X = 0, 1, 0, 1, 0, 1, 0, 1,
      0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 1, 1, 1, 2;
  Project(P1, X, &x1);
  Project(P2, X, &x2);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondencesLinear(x1, x2, &F_estimated);

  // Compute ground truth.
  Mat3 F_gt;
  FundamentalFromProjections(P1, P2, &F_gt);

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(F_gt, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);
  
  // Compare with ground truth.
  EXPECT_NEAR(F_gt_norm(0, 0), F_estimated_norm(0, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 1), F_estimated_norm(0, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 2), F_estimated_norm(0, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 0), F_estimated_norm(1, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 1), F_estimated_norm(1, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 2), F_estimated_norm(1, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 0), F_estimated_norm(2, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 1), F_estimated_norm(2, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 2), F_estimated_norm(2, 2), 1e-8);

  // Check fundamental properties.
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x = x1(0, i), x1(1, i), 1;
    y = x2(0, i), x2(1, i), 1;
    F_x = F_estimated * x;
    y_F_x(i) = dot(y, F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);
  EXPECT_NEAR(0, DeterminantSlow(F_estimated), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences8PointRealistic) {
  Mat34 P1, P2;
  TwoRealisticCameras(&P1, &P2);

  // The 8 points and their projections.
  int n = 8;
  Mat X(3,n), x1, x2;
  X = 0, 1, 0, 1, 0, 1, 0, 1,
      0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 1, 1, 1, 2;
  Project(P1, X, &x1);
  Project(P2, X, &x2);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondences8Point(x1, x2, &F_estimated);

  // Compute ground truth.
  Mat3 F_gt;
  FundamentalFromProjections(P1, P2, &F_gt);

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(F_gt, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);
  
  // Compare with ground truth.
  EXPECT_NEAR(F_gt_norm(0, 0), F_estimated_norm(0, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 1), F_estimated_norm(0, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 2), F_estimated_norm(0, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 0), F_estimated_norm(1, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 1), F_estimated_norm(1, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 2), F_estimated_norm(1, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 0), F_estimated_norm(2, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 1), F_estimated_norm(2, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 2), F_estimated_norm(2, 2), 1e-8);

  // Check fundamental properties.
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x = x1(0, i), x1(1, i), 1;
    y = x2(0, i), x2(1, i), 1;
    F_x = F_estimated * x;
    y_F_x(i) = dot(y, F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);
  EXPECT_NEAR(0, DeterminantSlow(F_estimated), 1e-8);
}

// 8 points in a cube is a degenerate configuration.
// FundamentalFromCorrespondencesX return a small number when linear estimation
// of F fails.
TEST(Fundamental, FundamentalFromCorrespondences8PointDegenerate) {
  Mat34 P1, P2;
  TwoRealisticCameras(&P1, &P2);

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
  double res;
  res = FundamentalFromCorrespondences8Point(x1, x2, &F_estimated);

  EXPECT_NEAR(0, res, 1e-8);
}

} // namespace
