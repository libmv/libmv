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

#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

TEST(FocalFromFundamental, FundamentalShiftPrincipalPoints) {
  // Create two pair of cameras with the same parameters except for the
  // principal point.
  Vec2 p1, p1_new, p2, p2_new;
  Mat3 K1, K1_new, R1, K2, K2_new, R2;
  Vec3 t1, t2;
  Mat34 P1, P1_new, P2, P2_new;
  p1 = 3, 4;
  p1_new = 2, 1;
  p2 = -2, 1;
  p2_new = 4, -3;
  K1 = 1, 0, p1(0),
       0, 1, p1(1),
       0, 0,     1;
  K1_new = 1, 0, p1_new(0),
           0, 1, p1_new(1),
           0, 0,         1;
  K2 = 1, 0, p2(0),
       0, 1, p2(1),
       0, 0,     1;
  K2_new = 1, 0, p2_new(0),
           0, 1, p2_new(1),
           0, 0,         1;
  R1 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  R2 = 1, 0, 0,
       0, 1, 0,
       0, 0, 1;
  t1 = 0, 0, 0;
  t2 = 1, 0, 0;

  P_From_KRt(K1, R1, t1, &P1);
  P_From_KRt(K2, R2, t2, &P2);
  P_From_KRt(K1_new, R1, t1, &P1_new);
  P_From_KRt(K2_new, R2, t2, &P2_new);

  // Compute the fundamental matrix of each camera pair.
  Mat3 F, F_new;
  FundamentalFromProjections(P1, P2, &F);
  FundamentalFromProjections(P1_new, P2_new, &F_new);

  // Shift the principal point of the first fundamental matrix to match the
  // second one.
  Mat3 F_new_computed;
  FundamentalShiftPrincipalPoints(F, p1, p1_new, p2, p2_new, &F_new_computed);

  // TODO(pau) The matrices should be scaled before comparing since they have
  // to be equal only up to scale.
  EXPECT_NEAR(F_new(0,0), F_new_computed(0,0), 1e-8);
  EXPECT_NEAR(F_new(0,1), F_new_computed(0,1), 1e-8);
  EXPECT_NEAR(F_new(0,2), F_new_computed(0,2), 1e-8);
  EXPECT_NEAR(F_new(1,0), F_new_computed(1,0), 1e-8);
  EXPECT_NEAR(F_new(1,1), F_new_computed(1,1), 1e-8);
  EXPECT_NEAR(F_new(1,2), F_new_computed(1,2), 1e-8);
  EXPECT_NEAR(F_new(2,0), F_new_computed(2,0), 1e-8);
  EXPECT_NEAR(F_new(2,1), F_new_computed(2,1), 1e-8);
  EXPECT_NEAR(F_new(2,2), F_new_computed(2,2), 1e-8);
}

TEST(FocalFromFundamental, FindEpipoles) {
}

}
