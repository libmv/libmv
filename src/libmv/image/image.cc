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

#include <iostream>

#include "libmv/image/image.h"

namespace libmv {

void ConvertFloatImageToByteImage(const FloatImage &float_image,
                                  ByteImage *byte_image) {
  byte_image->ResizeLike(float_image);
  float minval=HUGE_VAL, maxval=-HUGE_VAL;
  for (int i = 0; i < float_image.Height(); ++i) {
    for (int j = 0; j < float_image.Width(); ++j) {
      for (int k = 0; k < float_image.Depth(); ++k) {
        minval = std::min(minval, float_image(i,j,k));
        maxval = std::max(maxval, float_image(i,j,k));
      }
    }
  }
  for (int i = 0; i < float_image.Height(); ++i) {
    for (int j = 0; j < float_image.Width(); ++j) {
      for (int k = 0; k < float_image.Depth(); ++k) {
        float unscaled = (float_image(i,j,k) - minval) / (maxval - minval);
        (*byte_image)(i,j,k) = (unsigned char)(255 * unscaled);
      }
    }
  }
}

void ConvertByteImageToFloatImage(const ByteImage &byte_image,
                                  FloatImage *float_image) {
  float_image->ResizeLike(byte_image);
  for (int i = 0; i < byte_image.Height(); ++i) {
    for (int j = 0; j < byte_image.Width(); ++j) {
      for (int k = 0; k < byte_image.Depth(); ++k) {
	      (*float_image)(i,j,k) = float(byte_image(i,j,k)) / 255.0f;
      }
    }
  }
}

}  // namespace libmv
