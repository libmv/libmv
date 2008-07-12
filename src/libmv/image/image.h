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

#ifndef LIBMV_IMAGE_IMAGE_IMAGE_H
#define LIBMV_IMAGE_IMAGE_IMAGE_H

#include <math.h>

#include "libmv/image/array_nd.h"

namespace libmv {

/// Image class storing the values in a 3D array (row,column,channel).
template <typename T>
class Image : public ArrayND<T, 3> {
  typedef ArrayND<T, 3> Base;
 public:
  Image()
    : Base() {
  }
  Image(int height, int width, int depth=1)
    : Base(height,width,depth) {
  }

  void Resize(int height, int width, int depth=1) {
    Base::Resize(height, width, depth);
  }

  int Height() const {
    return Base::Shape(0);
  }
  int Width() const {
    return Base::Shape(1);
  }
  int Depth() const {
    return Base::Shape(2);
  }

  /// Enable accessing with 2 indices for grayscale images.
  T &operator()(int i0, int i1, int i2 = 0) {
    assert(0 <= i0 && i0 < Height());
    assert(0 <= i1 && i1 < Width());
    return Base::operator()(i0,i1,i2);
  }
  const T &operator()(int i0, int i1, int i2 = 0) const {
    assert(0 <= i0 && i0 < Height());
    assert(0 <= i1 && i1 < Width());
    return Base::operator()(i0,i1,i2);
  }
};

typedef Image<unsigned char> ByteImage;
typedef Image<float> FloatImage;

void ConvertFloatImageToByteImage(const FloatImage &float_image,
                                  ByteImage *byte_image);
void ConvertByteImageToFloatImage(const ByteImage &byte_image,
                                  FloatImage *float_image);

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_IMAGE_H
