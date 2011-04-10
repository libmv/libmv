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

#include "libmv/multiview/euclidean_resection_kernel.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/multiview/robust_euclidean_resection.h"
#include "libmv/numeric/numeric.h"

namespace libmv {
// Estimate robustly the the extrinsic parameters, R and t for a calibrated
// camera from 4 or more 3D points and their images.
// The euclidean resection solver relies on the EPnP method.
double EuclideanResectionEPnPRobust(const Mat2X &x_image, 
                                    const Mat3X &X_world,
                                    const Mat3  &K,
                                    double max_error,
                                    Mat3 *R, Vec3 *t,
                                    vector<int> *inliers,
                                    double outliers_probability) {
  // The threshold is on the sum of the squared errors.
  double threshold = Square(max_error);
  double best_score = HUGE_VAL;
  typedef libmv::euclidean_resection::kernel::Kernel Kernel;
  Kernel kernel(x_image, X_world, K);
  Mat34 P = Estimate(kernel, MLEScorer<Kernel>(threshold), 
                     inliers, &best_score, outliers_probability);
  Mat3 K_unused;
  KRt_From_P(P, &K_unused, R, t);
  if (best_score == HUGE_VAL)
    return HUGE_VAL;
  else
    return std::sqrt(best_score / 2.0);  
}

}  // namespace libmv
