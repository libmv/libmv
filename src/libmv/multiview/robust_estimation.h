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

#ifndef LIBMV_MULTIVIEW_ROBUST_ESTIMATION_H_
#define LIBMV_MULTIVIEW_ROBUST_ESTIMATION_H_

#include <vector>

namespace libmv {

class Model {
 public:
  virtual ~Model();
  virtual void Cost(int sample, double *cost, bool *is_inlier) = 0;
};

class Fitter {
 public:
  virtual ~Fitter();
  virtual int MinimumSamples() = 0;
  virtual void ImproveFit(const std::vector<int> &samples, Model *model) {
    (void) samples;
    (void) model;
  };
  virtual void Fit(const std::vector<int> &samples,
                   std::vector<Model *> *models) = 0;
};

// TODO(keir): This may not need to be a class.
class RobustFitter {
 public:
  virtual ~RobustFitter();
  virtual Model *Fit(Fitter *fitter, int total_samples);

  // Pick n unique integers in [0, total).
	virtual void PickSubset(int num_samples,
                          int total,
                          std::vector<int> *samples);

 private:
  Fitter *fitter_;
  Model *best_model_;
  double best_cost_;
  int total_samples_;
  std::vector<int> inlier_samples_;
};

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_ROBUST_ESTIMATION_H_
