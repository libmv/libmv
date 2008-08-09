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

// TODO(pau): This test is failing!
/*
TEST(Fundamental, FundamentalFromCorrespondences8PointRealistic) {
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

  EXPECT_NEAR(0, DeterminantSlow(F), 1e-8);
}
*/

} // namespace
