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

#ifndef LIBMV_IMAGE_IMAGE_PAU_ND_H
#define LIBMV_IMAGE_IMAGE_PAU_ND_H

#include "libmv/image/array_nd.h"
#include <boost/shared_array.hpp>
#include <iostream>

using namespace std;
using libmv::ArrayND;

namespace libmv {

/// Image class storing the values in a 3D array (row,column,channel).
template <typename T>
class Image : public ArrayND<T, 3> {
 public:
  Image()
    : ArrayND<T,3>() {
  }
  Image(int height, int width, int depth=1)
    : ArrayND<T,3>(height,width,depth) {
  }

  int Height() {
    return ArrayND<T, 3>::Shape(0);
  }
  int Width() {
    return ArrayND<T, 3>::Shape(1);
  }
  int Depth() {
    return ArrayND<T, 3>::Shape(2);
  }

  /// Enable accessing with 2 indices for grayscale images.
  T &operator()(int i0, int i1, int i2 = 0) {
    return ArrayND<T, 3>::operator()(i0,i1,i2);
  }
  const T &operator()(int i0, int i1, int i2 = 0) const {
    return ArrayND<T, 3>::operaton()(i0,i1,i2);
  }
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_PAU_ND_H
