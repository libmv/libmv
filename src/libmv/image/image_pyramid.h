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

#ifndef LIBMV_IMAGE_IMAGE_PYRAMID_H
#define LIBMV_IMAGE_IMAGE_PYRAMID_H

#include <vector>

#include "libmv/image/convolve.h"
#include "libmv/image/image.h"
#include "libmv/image/sample.h"

using namespace std;
using namespace libmv;

namespace libmv {

class ImagePyramid {
 public:
  ImagePyramid() : num_levels_(0) {
  }

  ~ImagePyramid() {
  }

  void Init(const FloatImage &im, int num_levels) {
    num_levels_ = num_levels;
    levels_.clear();
    levels_.resize(NumLevels());

    levels_[0] = im;
    for (int i = 1; i < NumLevels(); ++i) {
      ComputeLevel(i);
    }
  }

  const FloatImage &Level(int i) const {
    assert(0 <= i && i < NumLevels());
    return levels_[i];
  }

  int NumLevels() const {
    return num_levels_;
  }

 protected:
  void ComputeLevel(int l) {
    assert(0 < l && l < NumLevels());

    int height = Level(l-1).Height();
    int width = Level(l-1).Width();
    int new_height = height / 2;
    int new_width = width / 2;
    double sigma = 0.9;  // this is kind of magic

    FloatImage blured(height, width);
    ConvolveGaussian(Level(l-1), sigma, &blured);

    levels_[l].Resize(new_height, new_width, 1);
    for (int i = 0; i < new_height; ++i) {
      for (int j = 0; j < new_width; ++j) {
        levels_[l-1](i,j) = SampleLinear(levels_[l], 2*i + .5, 2*j + .5);
      }
    }
  }

 protected:
  int num_levels_;
  std::vector<FloatImage> levels_;

};

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_PYRAMID_H
