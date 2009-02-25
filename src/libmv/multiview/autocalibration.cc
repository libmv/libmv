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

#include "libmv/multiview/autocalibration.h"

namespace libmv {

void K_From_AbsoluteConic(const Mat3 &W, Mat3 *K) {
  // To compute upper-triangular Cholesky, we flip the indices of the input
  // matrix, compute lower-triangular Cholesky, and then unflip the result.
  Mat3 dual = W.inverse();
  Mat3 flipped_dual;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      flipped_dual(i,j) = dual(2 - i, 2 - j);
    }
  }

  Eigen::LLT<Mat3> llt(flipped_dual);
  Mat3 L = llt.matrixL();

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      (*K)(i,j) = L(2 - i, 2 - j);
    }
  }

  // Resolve sign ambiguities assuming positive diagonal.
  // TODO(pau) Check if Eigen::LLT does always return positive diagonal, and
  // remove this if so
  for (int j = 0; j < 3; ++j) {
    if ((*K)(j, j) < 0) {
      for (int i = 0; i < 3; ++i) {
        (*K)(i, j) = -(*K)(i, j);
      }
    }
  }
}

}  // namespace libmv
