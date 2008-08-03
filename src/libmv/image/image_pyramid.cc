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
#include "libmv/image/convolve.h"
#include "libmv/image/sample.h"

namespace libmv {

class ConcreteImagePyramid : public ImagePyramid {
 public:
  ConcreteImagePyramid() {}

  ConcreteImagePyramid(const FloatImage &image,
                       int num_levels,
                       double sigma = 0.9) {
    Init(image, num_levels, sigma);
  }

  virtual ~ConcreteImagePyramid() {
  }

  void Init(const FloatImage &image, int num_levels, double sigma = 0.9) {
    assert(image.Depth() == 1);

    levels_.resize(num_levels);

    BlurredImageAndDerivativesChannels(image, sigma, &levels_[0]);

    vector<Array3Df> downsamples(num_levels);
    downsamples[0] = image;

    for (int i = 1; i < NumLevels(); ++i) {
      DownsampleChannelsBy2(downsamples[i-1], &downsamples[i]);
      BlurredImageAndDerivativesChannels(downsamples[i], sigma, &levels_[i]);
    }
  }

  virtual const Array3Df &Level(int i) {
    assert(0 <= i && i < NumLevels());
    return levels_[i];
  }

  virtual int NumLevels() const {
    return levels_.size();
  }

 private:
  std::vector<Array3Df> levels_;
};

ImagePyramid *MakeImagePyramid(const Array3Df &image,
                               int num_levels,
                               double sigma) {
  return new ConcreteImagePyramid(image, num_levels, sigma);
}

}  // namespace libmv

