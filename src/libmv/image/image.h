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

#include "libmv/image/array_nd.h"
#include <boost/shared_array.hpp>
#include <iostream>
#include <math.h>

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

  int Height() const {
    return ArrayND<T, 3>::Shape(0);
  }
  int Width() const {
    return ArrayND<T, 3>::Shape(1);
  }
  int Depth() const {
    return ArrayND<T, 3>::Shape(2);
  }

  /// Enable accessing with 2 indices for grayscale images.
  T &operator()(int i0, int i1, int i2 = 0) {
    return ArrayND<T, 3>::operator()(i0,i1,i2);
  }
  const T &operator()(int i0, int i1, int i2 = 0) const {
    return ArrayND<T, 3>::operator()(i0,i1,i2);
  }

  /// Nearest neighbor interpolation.
  T Nearest(float y, float x, int v = 0) const {
    const int i = int(round(y));
    const int j = int(round(x));
    return operator()(i, j, v);
  }

  /// Linear interpolation.
  float Linear(float y, float x, int v = 0) const {
    int x1, y1, x2, y2;
    float dx1, dy1, dx2, dy2;

    LinearInitAxis(&y1, &y2, &dy1, &dy2, y, Height());
    LinearInitAxis(&x1, &x2, &dx1, &dx2, x, Width());

    const T im11 = operator()(y1,x1,v);
    const T im12 = operator()(y1,x2,v);
    const T im21 = operator()(y2,x1,v);
    const T im22 = operator()(y2,x2,v);

    return dy1 * ( dx1 * im11 + dx2 * im12 )
         + dy2 * ( dx1 * im21 + dx2 * im22 );
  }
 private:
  static void LinearInitAxis(int *x1, int *x2, float *dx1, float *dx2,
                             float fx, int width) {
    const int ix = int(fx);
    if (ix < 0) {
      *x1 = 0;
      *x2 = 0;
      *dx1 = 1;
      *dx2 = 0;
    } 
    else if (ix > width-2) {
      *x1 = width-1;
      *x2 = width-1;
      *dx1 = 1;
      *dx2 = 0;
    }
    else {
      *x1 = ix;
      *x2 = *x1 + 1;
      *dx1 = *x2 - fx;
      *dx2 = 1 - *dx1;
    }
  }
};

typedef Image<unsigned char> ByteImage;
typedef Image<float> FloatImage;

int ReadPgm(const char *filename, ByteImage *im);
int ReadPgmStream(FILE *file, ByteImage *im);
int WritePgm(const ByteImage &im, const char *filename);
int WritePgmStream(const ByteImage &im, FILE *file);

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_IMAGE_H
