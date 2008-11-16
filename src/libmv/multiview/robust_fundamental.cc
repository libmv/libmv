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

#include "libmv/numeric/numeric.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/robust_estimation.h"

namespace libmv {

struct FundamentalModel : public Model {
 public:
  virtual ~FundamentalModel() {};

  FundamentalModel(const Mat &x1s, const Mat &x2s, Mat3 F)
    : all_x1s_(x1s), all_x2s_(x2s), F(F) {}

  virtual void Cost(int sample, double *cost, bool *is_inlier) {
    Vec3 x1; x1.start(2) = all_x1s_.col(sample); x1(2) = 1.0;
    Vec3 x2; x2.start(2) = all_x2s_.col(sample); x2(2) = 1.0;
    *is_inlier = false;

    // Approximation of true error; page 287 of HZ equation 11.9. This avoids
    // triangulating the point, relying only on the entries in F.
    double sampson_error = (x1.transpose() * F * x2)(0,0)
         / ((F*x1).norm2() + (F.transpose()*x2).norm2());

    sampson_error = sampson_error*sampson_error;
    printf("sampson(%d)=%g\n", sample, sampson_error);

    // TODO(keir): Abstract this knob! In libmv1 this was adaptive.
    const double threshold = 1.0;
    *cost = threshold;
    if (sampson_error < threshold) {
      *is_inlier = true;
      *cost = sampson_error;
    }

  }

  const Mat &all_x1s_;
  const Mat &all_x2s_;
  Mat3 F;
};

class FundamentalFitter : public Fitter {
 public:
  virtual ~FundamentalFitter() {};
  FundamentalFitter(const Mat &all_x1s, const Mat &all_x2s)
    : all_x1s_(all_x1s), all_x2s_(all_x2s) {}

  virtual int MinimumSamples() {
    return 8;
  }
  virtual void Fit(const std::vector<int> &samples, std::vector<Model *> *models) {
    Mat x1s(2, samples.size());
    Mat x2s(2, samples.size());
    for (int i = 0; i < samples.size(); ++i) {
      printf("picked sample %d\n", samples[i]);
      x1s.col(i) = all_x1s_.col(samples[i]);
      x2s.col(i) = all_x2s_.col(samples[i]);
    }
    Mat3 F;
    FundamentalFromCorrespondences8Point(x1s, x2s, &F);
    models->push_back(new FundamentalModel(all_x1s_, all_x2s_, F));
  }
  const Mat &all_x1s_;
  const Mat &all_x2s_;
};

double FundamentalFromCorrespondences8PointRobust(const Mat &x1,
                                                  const Mat &x2,
                                                  Mat3 *F) {
  RobustFitter robust_fitter;
  FundamentalFitter fundamental_fitter(x1, x2);
  Model *F_model = robust_fitter.Fit(&fundamental_fitter, x1.cols());
  *F = static_cast<FundamentalModel *>(F_model)->F;
  return 0.0;  // This doesn't mean much for the robust case.
}

}  // namespace libmv
