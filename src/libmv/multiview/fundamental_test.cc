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
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

#include "libmv/multiview/generated.cc"

namespace {
using namespace libmv;

TEST(Fundamental, FileGenerator) {
  EXPECT_EQ(3, x);
}

TEST(Fundamental, P_From_KRt) {
  Mat34 P;
  Mat3 K, R;
  Vec3 t;

  K = 10,  1, 30,
       0, 20, 40,
       0,  0,  1;
  R = 1, 0, 0,
      0, 1, 0,
      0, 0, 1;
  t = 1, 2, 3;
  P_From_KRt(K, R, t, &P);
  KRt_From_P(P, &K, &R, &t);
  EXPECT_NEAR(10, K(0,0), 1e-8);
  EXPECT_NEAR( 1, K(0,1), 1e-8);
  EXPECT_NEAR(30, K(0,2), 1e-8);
  EXPECT_NEAR( 0, K(1,0), 1e-8);
  EXPECT_NEAR(20, K(1,1), 1e-8);
  EXPECT_NEAR(40, K(1,2), 1e-8);
  EXPECT_NEAR( 0, K(2,0), 1e-8);
  EXPECT_NEAR( 0, K(2,1), 1e-8);
  EXPECT_NEAR( 1, K(2,2), 1e-8);
  EXPECT_NEAR( 1, R(0,0), 1e-8);
  EXPECT_NEAR( 0, R(0,1), 1e-8);
  EXPECT_NEAR( 0, R(0,2), 1e-8);
  EXPECT_NEAR( 0, R(1,0), 1e-8);
  EXPECT_NEAR( 1, R(1,1), 1e-8);
  EXPECT_NEAR( 0, R(1,2), 1e-8);
  EXPECT_NEAR( 0, R(2,0), 1e-8);
  EXPECT_NEAR( 0, R(2,1), 1e-8);
  EXPECT_NEAR( 1, R(2,2), 1e-8);
  EXPECT_NEAR( 1, t(0), 1e-8);
  EXPECT_NEAR( 2, t(1), 1e-8);
  EXPECT_NEAR( 3, t(2), 1e-8);

  // This is for testing that det(R) = 1, which is not the current behaviour
  // of KRt_From_P
/*  P =-1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0;
  KRt_From_P(P, &K, &R, &t);
  EXPECT_NEAR( 1, K(0,0), 1e-8);
  EXPECT_NEAR( 0, K(0,1), 1e-8);
  EXPECT_NEAR( 0, K(0,2), 1e-8);
  EXPECT_NEAR( 0, K(1,0), 1e-8);
  EXPECT_NEAR(-1, K(1,1), 1e-8);
  EXPECT_NEAR( 0, K(1,2), 1e-8);
  EXPECT_NEAR( 0, K(2,0), 1e-8);
  EXPECT_NEAR( 0, K(2,1), 1e-8);
  EXPECT_NEAR( 1, K(2,2), 1e-8);
  EXPECT_NEAR(-1, R(0,0), 1e-8);
  EXPECT_NEAR( 0, R(0,1), 1e-8);
  EXPECT_NEAR( 0, R(0,2), 1e-8);
  EXPECT_NEAR( 0, R(1,0), 1e-8);
  EXPECT_NEAR(-1, R(1,1), 1e-8);
  EXPECT_NEAR( 0, R(1,2), 1e-8);
  EXPECT_NEAR( 0, R(2,0), 1e-8);
  EXPECT_NEAR( 0, R(2,1), 1e-8);
  EXPECT_NEAR( 1, R(2,2), 1e-8);
  EXPECT_NEAR( 0, t(0), 1e-8);
  EXPECT_NEAR( 0, t(1), 1e-8);
  EXPECT_NEAR( 0, t(2), 1e-8);
  */
}

TEST(Fundamental, FundamentalFromProjections) {

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

  // Something really weird was happening when this asserts were done inside
  // the for loop above.
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

  // Something really weird was happening when this asserts were done inside
  // the for loop above.
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);

  // TODO(pau) Check that det(F) == 0.
  //EXPECT_NEAR(0, det(F), 1e-8);
}
}
