// Copyright (c) 2008, 2009 libmv authors.
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

#include "libmv/logging/logging.h"
#include "libmv/multiview/homography.h"

#include <iostream>

namespace libmv {

bool Homography2DFromCorrespondencesLinear(const Mat &x1,
                                           const Mat &x2,
                                           Mat3 *H,
                                           double expected_precision) {
  assert(2 == x1.rows());
  assert(4 >= x1.cols());
  assert(x1.rows() == x2.rows());
  assert(x1.cols() == x2.cols());

  int n = x1.cols();
  MatX9 L = Mat::Zero(n * 2, 9);
  for (int i = 0; i < n; ++i) {
    int j = 2 * i;
    L(j, 0) = x1(0, i);
    L(j, 1) = x1(1, i);
    L(j, 2) = 1.0;
    L(j, 6) = -x2(0, i) * x1(0, i);
    L(j, 7) = -x2(0, i) * x1(1, i);
    L(j, 8) = -x2(0, i);

    ++j;
    L(j, 3) = x1(0, i);
    L(j, 4) = x1(1, i);
    L(j, 5) = 1.0;
    L(j, 6) = -x2(1, i) * x1(0, i);
    L(j, 7) = -x2(1, i) * x1(1, i);
    L(j, 8) = -x2(1, i);
  }

  Vec h;
  const double prec = Nullspace(&L, &h);
  if (prec < expected_precision)  {
    *H = Map<RMat3>(h.data());
    return true;
  } else {
    return false;
  }
}
}  // namespace libmv
