// Copyright (c) 2007, 2008, 2009 libmv authors.
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

#ifndef LIBMV_IMAGE_NON_MAXIMAL_SUPPRESSION_H
#define LIBMV_IMAGE_NON_MAXIMAL_SUPPRESSION_H

#include <cmath>

#include "third_party/glog/src/glog/logging.h"
#include "libmv/numeric/numeric.h"

template<typename TArray>
inline bool IsLocalMax3D(const TArray &f, int width, int x, int y, int z) {
  typedef typename TArray::Scalar Scalar;
  using std::max;
  using std::min;
  assert(width % 2 == 1);
  int r = width / 2;
  for (int xx = max(0, x - r); xx <= min(x + r, f.depth() - 1); ++xx) {
    for (int yy = max(0, y - r); yy <= min(y + r, f.rows() - 1); ++yy) {
      for (int zz = max(0, z - r); zz <= min(z + r, f.cols() - 1); ++zz) {
        if (f(x, y, z) <= f(xx, yy, zz) &&
            ((xx != x) && (yy != y) && (zz != z))) {
          return false;
        }
      }
    }
  }
  return true;
}

// Find the local maximums of f(x, y, z) within boxes of width^3. Call callback
// for each absolute extremum within any cube box of size width x width x
// width. Callback is of the form void(int x, int y, int z, Scalar value);
template<typename TArray, typename TCallback>
inline void FindLocalMaxima3D(const TArray &f,
                              int width,
                              TCallback *callback) {
  typedef typename TArray::Scalar Scalar;

  for (int x = 0; x < f.depth(); x += width) {
    for (int y = 0; y < f.rows(); y += width) {
      for (int z = 0; z < f.cols(); z += width) {

        // Scan the pixels in this block to find the extremum.
        int x_max, y_max, z_max;
        Scalar max_val = Scalar(-HUGE_VAL);
        for (int xx = x; xx < std::min(x + width, f.depth()); ++xx) {
          for (int yy = y; yy < std::min(y + width, f.rows()); ++yy) {
            for (int zz = z; zz < std::min(z + width, f.cols()); ++zz) {
              if (f(xx, yy, zz) > max_val) {
                max_val = f(xx, yy, zz);
                x_max = xx;
                y_max = yy;
                z_max = zz;
              }
            }
          }
        }
        // Check if the found extremum is an extremum across block boundaries.
        if (IsLocalMax3D(f, width, x_max, y_max, z_max)) {
          (*callback)(x_max, y_max, z_max, max_val);
        }
      }
    }
  }
}

#endif  // LIBMV_IMAGE_NON_MAXIMAL_SUPPRESSION_H
