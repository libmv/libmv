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

#include "libmv/multiview/projection.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(Projection, P_From_KRt) {
  Mat3 K, Kp;
  K << 10,  1, 30,
        0, 20, 40,
        0,  0,  1;

  Mat3 R, Rp;
  R << 1, 0, 0,
       0, 1, 0,
       0, 0, 1;

  Vec3 t, tp;
  t << 1, 2, 3;

  Mat34 P;
  P_From_KRt(K, R, t, &P);
  KRt_From_P(P, &Kp, &Rp, &tp);

  EXPECT_MATRIX_NEAR(K, Kp, 1e-8);
  EXPECT_MATRIX_NEAR(R, Rp, 1e-8);
  EXPECT_MATRIX_NEAR(t, tp, 1e-8);

  // TODO(keir): Change the code to ensure det(R) == 1, which is not currently
  // the case. Also add a test for that here.
}

TEST(Projection, Projection) {
  Mat3X X(3, 4);
  X << 1, 0, 0, 1,
       0, 1, 0, 1,
       0, 0, 1, 1;

  Mat4X HX(4, 4);
  HX << 1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 1,
        1, 1, 1, 1;

  Mat34 P;
  P << 1, 0, 0, 100,
       1, 2, 3, 100,
       0, 0, 1, 100;

  Mat2X x(2, 4);
  x << 101 / 100., 100 / 100., 100 / 101., 101 / 101.,
       101 / 100., 102 / 100., 103 / 101., 106 / 101.;

  EXPECT_MATRIX_NEAR(x, Project(P, X),  1e-16);
  EXPECT_MATRIX_NEAR(x, Project(P, HX), 1e-16);
}

} // namespace
