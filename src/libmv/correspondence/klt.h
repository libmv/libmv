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

#ifndef LIBMV_CORRESPONDENCE_KLT_H_
#define LIBMV_CORRESPONDENCE_KLT_H_

#include <cassert>
#include <vector>

#include "libmv/image/image.h"

using std::vector;

namespace libmv {

class KltContext {
 public:
  struct DetectedFeature {
    int int_x, int_y;
    float real_x, real_y;
    float trackness;
  };

  KltContext(int window_size=7) : window_size_(window_size) {}

  void DetectGoodFeatures(const FloatImage &image,
                          vector<DetectedFeature> *features);

  void ComputeGradientMatrix(const FloatImage &image,
                             FloatImage *gxx_pointer,
                             FloatImage *gxy_pointer,
                             FloatImage *gyy_pointer);

  // Compute trackness of every pixel given the gradient matrix.
  void ComputeTrackness(const FloatImage &gxx,
                        const FloatImage &gxy,
                        const FloatImage &gyy,
                        FloatImage *trackness_pointer);

  void FindLocalMaxima(const FloatImage &trackness,
                       vector<DetectedFeature> *points);

  // Given the three distinct elements of the symmetric 2x2 matrix
  //
  //                     [gxx gxy]
  //                     [gxy gyy],
  //
  // return the minimum eigenvalue of the matrix.  
  // Borrowed from Stan Birchfield's KLT implementation.
  static float MinEigenValue(float gxx, float gxy, float gyy) {
    return (gxx + gyy - sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy)) / 2.0f;
  }

  int WindowSize() { return window_size_; }
  
 protected:
  int window_size_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_KLT_H_
