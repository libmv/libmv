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

namespace {
using namespace libmv;

TEST(Fundamental, MeanAndVariancesFromPoints) {
  int n = 4;
  std::vector<Vec2> points(n);
  points[0] = 0, 0;
  points[1] = 0, 2;
  points[2] = 1, 1;
  points[3] = 1, 3;

  double meanx, meany, varx, vary;
  MeanAndVariancesFromPoints(points, &meanx, &meany, &varx, &vary);

  EXPECT_NEAR(0.5, meanx, 1e-8);
  EXPECT_NEAR(1.5, meany, 1e-8);
  EXPECT_NEAR(0.25, varx, 1e-8);
  EXPECT_NEAR(1.25, vary, 1e-8);
}

TEST(Fundamental, PreconditionerFromPoints) {
  int n = 4;
  std::vector<Vec2> points(n);
  points[0] = 0, 0;
  points[1] = 0, 2;
  points[2] = 1, 1;
  points[3] = 1, 3;

  Mat3 T;
  PreconditionerFromPoints(points, &T);

  std::vector<Vec2> normalized_points;
  ApplyTransformationToPoints(points, T, &normalized_points);

  double meanx, meany, varx, vary;
  MeanAndVariancesFromPoints(normalized_points, &meanx, &meany, &varx, &vary);

  EXPECT_NEAR(0, meanx, 1e-8);
  EXPECT_NEAR(0, meany, 1e-8);
  EXPECT_NEAR(2, varx, 1e-8);
  EXPECT_NEAR(2, vary, 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondencesLinear) {
  int n = 8;
  std::vector<Vec2> x1(n);
  x1[0] = 0, 0;
  x1[1] = 0, 1;
  x1[2] = 0, 2;
  x1[3] = 1, 0;
  x1[4] = 1, 1;
  x1[5] = 1, 2;
  x1[6] = 2, 0;
  x1[7] = 2, 1;

  std::vector<Vec2> x2(n);
  for (int i = 0; i < n; ++i) {
    x2[i] = x1[i](0), x1[i](1) + 1;
  }

  Mat3 F;
  FundamentalFromCorrespondencesLinear(x1, x2, &F);

  std::vector<double> x_F_y(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_y;
    x = x1[i](0), x1[i](1), 1;
    y = x2[i](0), x2[i](1), 1;
    F_y = F * y;
    x_F_y[i] = dot(x, F_y);
  }

  // Something really weird was happening when this asserts were done inside
  // the for loop above.
  EXPECT_NEAR(0, x_F_y[0], 1e-8);
  EXPECT_NEAR(0, x_F_y[1], 1e-8);
  EXPECT_NEAR(0, x_F_y[2], 1e-8);
  EXPECT_NEAR(0, x_F_y[3], 1e-8);
  EXPECT_NEAR(0, x_F_y[4], 1e-8);
  EXPECT_NEAR(0, x_F_y[5], 1e-8);
  EXPECT_NEAR(0, x_F_y[6], 1e-8);
  EXPECT_NEAR(0, x_F_y[7], 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences8Point) {
  int n = 8;
  std::vector<Vec2> x1(n);
  x1[0] = 0, 0;
  x1[1] = 0, 1;
  x1[2] = 0, 2;
  x1[3] = 1, 0;
  x1[4] = 1, 1;
  x1[5] = 1, 2;
  x1[6] = 2, 0;
  x1[7] = 2, 1;

  std::vector<Vec2> x2(n);
  for (int i = 0; i < n; ++i) {
    x2[i] = x1[i](0), x1[i](1) + 1;
  }

  Mat3 F;
  FundamentalFromCorrespondences8Point(x1, x2, &F);

  std::vector<double> x_F_y(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_y;
    x = x1[i](0), x1[i](1), 1;
    y = x2[i](0), x2[i](1), 1;
    F_y = F * y;
    x_F_y[i] = dot(x, F_y);
  }

  // Something really weird was happening when this asserts were done inside
  // the for loop above.
  EXPECT_NEAR(0, x_F_y[0], 1e-8);
  EXPECT_NEAR(0, x_F_y[1], 1e-8);
  EXPECT_NEAR(0, x_F_y[2], 1e-8);
  EXPECT_NEAR(0, x_F_y[3], 1e-8);
  EXPECT_NEAR(0, x_F_y[4], 1e-8);
  EXPECT_NEAR(0, x_F_y[5], 1e-8);
  EXPECT_NEAR(0, x_F_y[6], 1e-8);
  EXPECT_NEAR(0, x_F_y[7], 1e-8);

  // TODO(pau) Check that det(F) == 0.
  //EXPECT_NEAR(0, det(F), 1e-8);
}
}
