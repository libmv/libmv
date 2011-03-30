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

#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/resection_kernel.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace libmv {
namespace resection {

using libmv::resection::kernel::Kernel;

TEST(ResectionKernel, RobustResection) {
  int nviews = 5;
  int npoints = 60;
  NViewDataSet d = NRealisticCamerasFull(nviews, npoints);
  for (int i = 0; i < nviews; ++i) {
    Mat4X X(4, npoints);
    X.block(0, 0, 3, npoints) = d.X;
    X.row(3).setOnes();
    Mat2X &x = d.x[i];
    LOG(INFO) << "\n" << x;

    // Now make 30% of the points in x totally wrong.
    x.block(0, 0, 2, 20).setRandom();

    Kernel kernel(x, X);
    vector<int> inliers;
    Mat34 P = Estimate(kernel, MLEScorer<Kernel>(1), &inliers);

    Mat34 P_expected = d.P(i);
    EXPECT_MATRIX_PROP(P_expected, P, 1e-7);

    // Make sure inliers were classified properly.
    for (int i = 0; i < inliers.size(); ++i) {
      LOG(INFO) << inliers[i];
      EXPECT_EQ(i + 20, inliers[i]);  // 0..19 are outliers.
    }
    EXPECT_EQ(40, inliers.size());
  }
}

}  // namespace resection
}  // namespace libmv
