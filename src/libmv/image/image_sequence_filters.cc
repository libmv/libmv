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

#include "libmv/image/convolve.h"
#include "libmv/image/filtered_sequence.h"
#include "libmv/image/sample.h"

namespace libmv {

namespace {

class BlurAndTakeDerivativesFilter : public Filter {
 public:
  BlurAndTakeDerivativesFilter(double sigma)
      : sigma_(sigma) {}
  virtual ~BlurAndTakeDerivativesFilter() {}
  virtual void RunFilter(const Array3Df &source, Array3Df *destination) {
    BlurredImageAndDerivativesChannels(source, sigma_, destination);
  }
  double sigma_;
};

class DownSampleBy2Filter : public Filter {
 public:
  virtual ~DownSampleBy2Filter() {}
  void RunFilter(const Array3Df &source, Array3Df *destination) {
    DownsampleChannelsBy2(source, destination);
  }
};

}  // namespace

ImageSequence *BlurSequenceAndTakeDerivatives(ImageSequence *source,
                                              double sigma) {
  return new FilteredImageSequence(source,
                                   new BlurAndTakeDerivativesFilter(sigma));
}

ImageSequence *DownsampleSequenceBy2(ImageSequence *source) {
  return new FilteredImageSequence(source, new DownSampleBy2Filter());
}

}  // namespace libmv
