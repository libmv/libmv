// Copyright (c) 2011 libmv authors.
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

#include "libmv/base/vector.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/robust_similarity.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

TEST(RobustSimilarity, Similarity2DFromCorrespondences2PointRobust) {
  // Define a few similarities.
  const int num_h = 3;
  Mat3 H_gt[num_h];

  H_gt[0] = Mat3::Identity();
  
  double angle = 0.3;
  double scale = 2.1;
  H_gt[1] << scale*cos(angle), -scale*sin(angle), -4,
             scale*sin(angle),  scale*cos(angle),  5,
             0,  0,  1;
  angle = 2.3;
  scale = 0.2;
  H_gt[2] << scale*cos(angle), -scale*sin(angle), 3,
             scale*sin(angle),  scale*cos(angle),-6,
             0,  0,  1;
  

  // Define a set of points.
  int n = 20;
  Mat x(2, n), xh;
  x << 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3,
       0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 0, 1, 2, 3, 4;
  EuclideanToHomogeneous(x, &xh);

  Mat3 H[num_h];
  for (int i = 0; i < num_h; ++i) {
    // Transform points by the ground truth similarity.
    Mat yh = H_gt[i] * xh;
    Mat y;
    HomogeneousToEuclidean(yh, &y);

    // Introduce outliers.
    for (int j = 0; j < 8; j++) {
      x(0,j) = x(0,j) + j * 5.5;
      x(1,j) = x(1,j) + 7.8;
    }

    // Estimate similarity from points.
    vector<int> inliers;
    Similarity2DFromCorrespondences2PointRobust(x, y, 0.1, &H[i], &inliers);
    H[i] /= H[i](2,2);
  }

  EXPECT_MATRIX_NEAR(H_gt[0], H[0], 1e-8);
  EXPECT_MATRIX_NEAR(H_gt[1], H[1], 1e-8);
  EXPECT_MATRIX_NEAR(H_gt[2], H[2], 1e-8);
}

}  // namespace
