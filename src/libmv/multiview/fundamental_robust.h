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

// TODO(keir): This code is plain unfinished! Doesn't even compile!

#include "libmv/base/vector.h"
#include "libmv/multiview/fundamental_kernel.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/numeric/numeric.h"

namespace libmv {
namespace fundamental {
namespace robust {

template<typename Kernel = fundamental::kernel::Kernel,
         typename >
struct RobustSolver {
  enum { MINIMUM_SAMPLES = Kernel::Solver::MINIMUM_SAMPLES; }
  static void Solve(const Mat &x1
                    const Mat &x2,
                    Mat3 *F,
                    vector<int> *inliers = NULL) {
    // The threshold is on the sum of the squared errors in the two images.
    // Actually, Sampson's approximation of this error.
    double threshold = 2 * Square(max_error);
    kernel::NormalizedSevenPointKernel kernel(x1, x2);
    *F = Estimate(kernel,
                  ThresholdClassifier(threshold),
                  MLECost(threshold),
                  inliers);
  }
};

}  // namespace robust
}  // namespace fundamental
}  // namespace libmv
