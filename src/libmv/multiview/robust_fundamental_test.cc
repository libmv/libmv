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
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

// TODO(keir): Finish this! It is totally incomplete.

namespace {
using namespace libmv;

TEST(RobustFundamental, FundamentalFromCorrespondences8PointRobust) {
  int n = 8;
  Mat x1(2,n);
  x1 << 0, 0, 0, 1, 1, 1, 2, 2,
        0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2(2,n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1,i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondences8PointRobust(x1, x2, &F);

  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << x1(0, i), x1(1, i), 1;
    y << x2(0, i), x2(1, i), 1;
    F_x = F * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);

  EXPECT_NEAR(0, Determinant(F), 1e-8);
}

// TODO(keir): Finish this! It is totally incomplete. The test is broken at the
// moment.
/*
TEST(Fundamental, FundamentalFromCorrespondences8PointRealistic) {
  TwoViewDataSet d = TwoRealisticCameras();

  d.X.set(Mat::Random(3, 50));
  std::cout << d.X << std::endl;

  Project(d.P1, d.X, &d.x1);
  Project(d.P2, d.X, &d.x2);
  std::cout << "x1::::" << d.x1 << std::endl;

  Mat x1s, x2s;
  HorizontalStack(d.x1, Mat::Random(2, 50), &x1s);
  HorizontalStack(d.x2, Mat::Random(2, 50), &x2s);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondences8PointRobust(x1s, x2s, &F_estimated);

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
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
  int n = d.X.cols();
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << d.x1(0, i), d.x1(1, i), 1;
    y << d.x2(0, i), d.x2(1, i), 1;
    F_x = F_estimated * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);
  EXPECT_NEAR(0, Determinant(F_estimated), 1e-8);
}
*/

} // namespace
