// Copyright (c) 2010 libmv authors.
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
#include "libmv/multiview/robust_resection.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace libmv {
namespace {

TEST(UncalibratedResectionRobustKernel, TestSynthetic6FullViews) {
  int nviews = 6;
  int npoints = 50;
  int noutliers = 0.4*npoints;
  double threshold_inlier = 0.2;
  NViewDataSet d = NRealisticCameras(nviews, npoints);
  Mat4X X;
  EuclideanToHomogeneous(d.X, &X);
  
  for (int i = 0; i < nviews; ++i) {
    Mat2X x = d.x[i];
    
    // Now make 40% of the points in x totally wrong.
    x.block(0, 0, 2, noutliers).setRandom();
  
    Mat34 P;
    vector<int> inliers;
    ResectionRobust(x, X, threshold_inlier, &P, &inliers);
    
    EXPECT_MATRIX_PROP(P, d.P(i), 3e-8);
    
    // Make sure inliers were classified properly.
    for (int i = 0; i < inliers.size(); ++i) {
      LOG(INFO) << inliers[i];
      EXPECT_EQ(i + noutliers, inliers[i]);  // 0..19 are outliers.
    }
    EXPECT_EQ(npoints-noutliers, inliers.size());
  }
}

}  // namespace
}  // namespace libmv
