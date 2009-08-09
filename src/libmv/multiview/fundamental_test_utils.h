// Copyright (c) 2007, 2009 libmv authors.
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


#ifndef LIBMV_MULTIVIEW_FUNDAMENTAL_TEST_UTILS_H
#define LIBMV_MULTIVIEW_FUNDAMENTAL_TEST_UTILS_H

#include "testing/testing.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

// Check the properties of a fundamental matrix:
//
//   1. The determinant is 0 (rank deficient)
//   2. The condition x'T*F*x = 0 is satisfied to precision.
//
template<typename TMat>
void ExpectFundamentalProperties(const TMat &F,
                                 const Mat &ptsA,
                                 const Mat &ptsB,
                                 double precision) {
  EXPECT_NEAR(0, F.determinant(), precision);
  assert(ptsA.cols() == ptsB.cols());
  Mat hptsA, hptsB;
  EuclideanToHomogeneous(ptsA, &hptsA);
  EuclideanToHomogeneous(ptsB, &hptsB);
  for (int i = 0; i < ptsA.cols(); ++i) {
    double residual = hptsB.col(i).dot(F * hptsA.col(i));
    EXPECT_NEAR(0.0, residual, precision);
  }
}

}

#endif