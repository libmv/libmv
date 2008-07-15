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

#include "libmv/image/image_pyramid.h"

namespace libmv {

void ImagePyramid::Init(const FloatImage &image,
                        int num_levels,
                        double sigma) {
  sigma_ = sigma;
  levels_.clear();
  levels_.resize(num_levels);
  gradient_x_.clear();
  gradient_x_.resize(num_levels);
  gradient_y_.clear();
  gradient_y_.resize(num_levels);

  FirstOrderGaussianJet(image, Sigma(),
                        &levels_[0], &gradient_x_[0], &gradient_y_[0]);

  for (int i = 1; i < NumLevels(); ++i) {
    ComputeLevel(i);
  }
}

void ImagePyramid::ComputeLevel(int l) {
  assert(0 < l && l < NumLevels());

  int new_height = Level(l-1).Height() / 2;
  int new_width = Level(l-1).Width() / 2;

  FloatImage subsampled(new_height, new_width);
  for (int i = 0; i < new_height; ++i) {
    for (int j = 0; j < new_width; ++j) {
      subsampled(i,j) = SampleLinear(levels_[l-1], 2 * i + .5, 2 * j + .5);
    }
  }

  FirstOrderGaussianJet(subsampled, Sigma(),
                        &levels_[l], &gradient_x_[l], &gradient_y_[l]);
}

}  // namespace libmv

