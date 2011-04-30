// Copyright (c) 2009 libmv authors.
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
#include "libmv/multiview/homography_kernel.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

using testing::Types;

namespace {

using namespace libmv;
using namespace libmv::homography::homography2D::kernel;

template <class Kernel>
struct HomographyKernelTest : public testing::Test {
};

typedef Types<Kernel,
              UnnormalizedKernel>
  HomographyKernelImplementations;

TYPED_TEST_CASE(HomographyKernelTest, HomographyKernelImplementations);

TYPED_TEST(HomographyKernelTest, Fitting) {
  // Define a few homographies.
  vector<Mat3> H_gt(3);

  H_gt[0] = Mat3::Identity();
  H_gt[1] << 1,  0, -4,
             0,  1,  5,
             0,  0,  1;
  H_gt[2] << 1, -2,  3,
             4,  5, -6,
            -7,  8,  1;

  // Define a set of points.
  Mat x(2, 9), xh;
  x << 0, 0, 0, 1, 1, 1, 2, 2, 2,
       0, 1, 2, 0, 1, 2, 0, 1, 2;
  EuclideanToHomogeneous(x, &xh);

  for (int i = 0; i < H_gt.size(); ++i) {
    SCOPED_TRACE(i);

    // Transform points by the ground truth homography.
    Mat y, yh = H_gt[i] * xh;
    HomogeneousToEuclidean(yh, &y);

    TypeParam kernel(x, y);

    vector<int> samples;
    samples.push_back(0);
    samples.push_back(1);
    samples.push_back(2);
    samples.push_back(3);
    samples.push_back(4);
    for (int j = 4; samples.size() < x.cols(); samples.push_back(++j)) {
      SCOPED_TRACE(samples.size());
      vector<Mat3> Hs;
      kernel.Fit(samples, &Hs);
      ASSERT_EQ(1, Hs.size());
      EXPECT_MATRIX_PROP(H_gt[i], Hs[0], 5e-8);
    }
  }
}
}  // namespace
