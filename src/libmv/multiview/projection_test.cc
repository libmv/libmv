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

TEST(Fundamental, P_From_KRt) {
  Mat34 P;
  Mat3 K, R;
  Vec3 t;

  K << 10,  1, 30,
        0, 20, 40,
        0,  0,  1;
  R << 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  t << 1, 2, 3;
  P_From_KRt(K, R, t, &P);
  KRt_From_P(P, &K, &R, &t);
  EXPECT_NEAR(10, K(0,0), 1e-8);
  EXPECT_NEAR( 1, K(0,1), 1e-8);
  EXPECT_NEAR(30, K(0,2), 1e-8);
  EXPECT_NEAR( 0, K(1,0), 1e-8);
  EXPECT_NEAR(20, K(1,1), 1e-8);
  EXPECT_NEAR(40, K(1,2), 1e-8);
  EXPECT_NEAR( 0, K(2,0), 1e-8);
  EXPECT_NEAR( 0, K(2,1), 1e-8);
  EXPECT_NEAR( 1, K(2,2), 1e-8);
  EXPECT_NEAR( 1, R(0,0), 1e-8);
  EXPECT_NEAR( 0, R(0,1), 1e-8);
  EXPECT_NEAR( 0, R(0,2), 1e-8);
  EXPECT_NEAR( 0, R(1,0), 1e-8);
  EXPECT_NEAR( 1, R(1,1), 1e-8);
  EXPECT_NEAR( 0, R(1,2), 1e-8);
  EXPECT_NEAR( 0, R(2,0), 1e-8);
  EXPECT_NEAR( 0, R(2,1), 1e-8);
  EXPECT_NEAR( 1, R(2,2), 1e-8);
  EXPECT_NEAR( 1, t(0), 1e-8);
  EXPECT_NEAR( 2, t(1), 1e-8);
  EXPECT_NEAR( 3, t(2), 1e-8);

  // This is for testing that det(R) = 1, which is not the current behaviour
  // of KRt_From_P
/*  P =-1, 0, 0, 0,
      0, 1, 0, 0,
      0, 0, 1, 0;
  KRt_From_P(P, &K, &R, &t);
  EXPECT_NEAR( 1, K(0,0), 1e-8);
  EXPECT_NEAR( 0, K(0,1), 1e-8);
  EXPECT_NEAR( 0, K(0,2), 1e-8);
  EXPECT_NEAR( 0, K(1,0), 1e-8);
  EXPECT_NEAR(-1, K(1,1), 1e-8);
  EXPECT_NEAR( 0, K(1,2), 1e-8);
  EXPECT_NEAR( 0, K(2,0), 1e-8);
  EXPECT_NEAR( 0, K(2,1), 1e-8);
  EXPECT_NEAR( 1, K(2,2), 1e-8);
  EXPECT_NEAR(-1, R(0,0), 1e-8);
  EXPECT_NEAR( 0, R(0,1), 1e-8);
  EXPECT_NEAR( 0, R(0,2), 1e-8);
  EXPECT_NEAR( 0, R(1,0), 1e-8);
  EXPECT_NEAR(-1, R(1,1), 1e-8);
  EXPECT_NEAR( 0, R(1,2), 1e-8);
  EXPECT_NEAR( 0, R(2,0), 1e-8);
  EXPECT_NEAR( 0, R(2,1), 1e-8);
  EXPECT_NEAR( 1, R(2,2), 1e-8);
  EXPECT_NEAR( 0, t(0), 1e-8);
  EXPECT_NEAR( 0, t(1), 1e-8);
  EXPECT_NEAR( 0, t(2), 1e-8);
  */
}

} // namespace
