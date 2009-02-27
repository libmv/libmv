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
#include "third_party/glog/src/glog/logging.h"

namespace {

using namespace libmv;

TEST(RobustFundamental, FundamentalFromCorrespondences8PointRobust) {
  int n = 9;
  Mat x1(2,n);
  x1 << 0, 0, 0, 1, 1, 1, 2, 2, /**/  1000,
        0, 1, 2, 0, 1, 2, 0, 1, /**/ -1000;

  Mat x2(2,n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1,i) += 1;
  }

  Mat3 F;
  std::vector<int> inliers;
  FundamentalFromCorrespondences8PointRobust(x1, x2, &F, &inliers);

  // FIXME: This doesn't actually check the value of F!

  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << x1(0, i), x1(1, i), 1;
    y << x2(0, i), x2(1, i), 1;
    F_x = F * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_MATRIX_NEAR_ZERO(y_F_x, 1e-8);
  EXPECT_NEAR(0, F.determinant(), 1e-8);
  EXPECT_EQ(8, inliers.size());
}

TEST(RobustFundamental,
     FundamentalFromCorrespondences8PointRealisticNoOutliers) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 F_estimated;
  std::vector<int> inliers;
  FundamentalFromCorrespondences8PointRobust(d.x1, d.x2, &F_estimated,
                                             &inliers);
  EXPECT_EQ(d.x1.cols(), inliers.size());

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);
  LOG(INFO) << "F_gt_norm =\n" << F_gt_norm;
  LOG(INFO) << "F_estimated_norm =\n" << F_estimated_norm;

  EXPECT_MATRIX_NEAR(F_gt_norm, F_estimated_norm, 1e-8);

  // Check fundamental properties.
  int n = d.X.cols();
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y;
    x << d.x1(0, i), d.x1(1, i), 1;
    y << d.x2(0, i), d.x2(1, i), 1;
    y_F_x(i) = y.dot(F_estimated * x);
  }
  EXPECT_MATRIX_NEAR_ZERO(y_F_x, 1e-8);
  EXPECT_NEAR(0, F_estimated.determinant(), 1e-8);
}

TEST(RobustFundamental, FundamentalFromCorrespondences8PointRealistic) {
  TwoViewDataSet d = TwoRealisticCameras();

  d.X.set(3*Mat::Random(3, 50));
  LOG(INFO) << "X = \n" << d.X;

  Project(d.P1, d.X, &d.x1);
  Project(d.P2, d.X, &d.x2);
  LOG(INFO) << "x1 = \n" << d.x1;
  LOG(INFO) << "x2 = \n" << d.x2;

  Mat x1s, x2s;
  HorizontalStack(d.x1, 400*Mat::Random(2, 20), &x1s);
  HorizontalStack(d.x2, 400*Mat::Random(2, 20), &x2s);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  std::vector<int> inliers;
  FundamentalFromCorrespondences8PointRobust(x1s, x2s, &F_estimated, &inliers);

  EXPECT_LE(d.x1.cols(), inliers.size());

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);
  LOG(INFO) << "F_gt_norm =\n" << F_gt_norm;
  LOG(INFO) << "F_estimated_norm =\n" << F_estimated_norm;

  // Compare with ground truth.
  EXPECT_MATRIX_NEAR(F_gt_norm, F_estimated_norm, 1e-8);

  // Check fundamental properties.
  int n = d.X.cols();
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y;
    x << d.x1(0, i), d.x1(1, i), 1;
    y << d.x2(0, i), d.x2(1, i), 1;
    y_F_x(i) = y.dot(F_estimated * x);
  }
  EXPECT_MATRIX_NEAR_ZERO(y_F_x, 1e-8);
  EXPECT_NEAR(0, F_estimated.determinant(), 1e-8);
}

} // namespace
