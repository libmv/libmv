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

#include "libmv/numeric/numeric.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/robust_homography.h"
#include "libmv/multiview/homography_kernel.h"
#include "libmv/multiview/panography_kernel.h"
#include "libmv/multiview/robust_estimation.h"


namespace libmv {

double HomographyFromCorrespondences4PointRobust(const Mat &x1,
                                                 const Mat &x2,
                                                 double max_error,
                                                 Mat3 *H,
                                                 vector<int> *inliers) {
  // The threshold is on the sum of the squared errors in the two images.
  double threshold = 2 * Square(max_error);
  typedef homography::kernel::Kernel KernelH;
  KernelH kernel(x1, x2);
  *H = Estimate(kernel, MLEScorer<KernelH>(threshold), inliers);
  return 0.0;  // This doesn't mean much for the robust case.
}

double HomographyFromCorrespondences2PointRobust(const Mat &x1,
                                                  const Mat &x2,
                                                  double max_error,
                                                  Mat3 * H,
                                                  vector<int> *inliers) {
  // The threshold is on the sum of the squared errors in the two images.
  // Actually, Sampson's approximation of this error.
  double threshold = 2 * Square(max_error);
  typedef panography::kernel::Kernel Kernel;
  Kernel kernel(x1, x2);
  *H = Estimate(kernel, MLEScorer<Kernel>(threshold), inliers);
  return 0.0;  // This doesn't mean much for the robust case.
}

}  // namespace libmv
