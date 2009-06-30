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

#include <cstdio>
#include <vector>

#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/numeric/numeric.h"
#include "libmv/logging/logging.h"

namespace libmv {

struct FundamentalModel {
 public:
  FundamentalModel() {}
  FundamentalModel(Mat3 F) : F(F) {}

  template<typename TVec>
  double Error(TVec x1x2) {
    double sampson_error2 = SampsonDistance2(F, x1x2.start(2), x1x2.end(2));
    VLOG(5) << "Sampson error^2 = " << sampson_error2;
    return sampson_error2;
  }
  Mat3 F;
};

// Simple linear fitter.
class FundamentalFitter {
 public:
  void Fit(Mat4X &x1x2, std::vector<FundamentalModel> *models) {
    Mat3 F;
    int n = x1x2.cols();
    Mat2X x1s(x1x2.block(0, 0, 2, n));
    Mat2X x2s(x1x2.block(2, 0, 2, n));
    FundamentalFromCorrespondences8Point(x1s, x2s, &F);
    VLOG(4) << "x1\n" << x1s << "\n";
    VLOG(4) << "x2\n" << x2s << "\n";
    VLOG(4) << "F\n" << F << "\n";
    models->push_back(FundamentalModel(F));
  }
  int MinimumSamples() {
    return 8;
  }
};

double FundamentalFromCorrespondences8PointRobust(const Mat &x1,
                                                  const Mat &x2,
                                                  double max_error,
                                                  Mat3 *F,
                                                  std::vector<int> *inliers) {
  // The threshold is on the sum of the squared errors in the two images.
  // Actually, Sampson's approximation of this error.
  double threshold = 2 * Square(max_error);
  Mat4X x1x2;
  VerticalStack(x1, x2, &x1x2);
  FundamentalModel model =
      Estimate<FundamentalModel>(x1x2,
                                 FundamentalFitter(),
                                 ThresholdClassifier(threshold),
                                 MLECost(threshold),
                                 inliers);
  *F = model.F;
  return 0.0;  // This doesn't mean much for the robust case.
}

}  // namespace libmv
