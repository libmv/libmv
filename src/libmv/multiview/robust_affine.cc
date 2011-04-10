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

#include "libmv/base/vector.h"
#include "libmv/multiview/affine_kernel.h"
#include "libmv/multiview/robust_affine.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

// Estimate robustly the 2d affine matrix between two dataset of 2D point
// (image coords space). The 2d affine solver relies on the 3 points solution.
double Affine2DFromCorrespondences3PointRobust(
    const Mat &x1,
    const Mat &x2,
    double max_error,
    Mat3 *H,
    vector<int> *inliers,
    double outliers_probability)
{
  // The threshold is on the sum of the squared errors in the two images.
  double threshold = 2 * Square(max_error);
  double best_score = HUGE_VAL;
  typedef affine::affine2D::kernel::Kernel KernelH;
  KernelH kernel(x1, x2);
  *H = Estimate(kernel, MLEScorer<KernelH>(threshold), inliers, 
                &best_score, outliers_probability);
  if (best_score == HUGE_VAL)
    return HUGE_VAL;
  else
    return std::sqrt(best_score / 2.0);  
}

} // namespace libmv
