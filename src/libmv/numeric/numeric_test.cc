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

#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

using namespace libmv;
/*
//using libmv::Mat;
using libmv::Mat2;
using libmv::Mat34;
using libmv::Vec;
using libmv::Vec2;
using libmv::Nullspace;
using libmv::Transpose;
*/

namespace {

TEST(Numeric, Nullspace) {
  Mat A(3, 4);
  A = 0.76026643, 0.01799744, 0.55192142, 0.8699745 ,
      0.42016166, 0.97863392, 0.33711682, 0.14479271,
      0.51016811, 0.66528302, 0.54395496, 0.57794893;
  Vec x;
  double s = Nullspace(&A, &x);
  EXPECT_NEAR(s, 0.122287, 1e-7);
  EXPECT_NEAR(x(0), -0.05473917, 1e-7);
  EXPECT_NEAR(x(1),  0.21822937, 1e-7);
  EXPECT_NEAR(x(2), -0.80258116, 1e-7);
  EXPECT_NEAR(x(3),  0.55248805, 1e-7);
}

TEST(Numeric, TinyMatrixNullspace) {
  Mat34 A;
  A = 0.76026643, 0.01799744, 0.55192142, 0.8699745 ,
      0.42016166, 0.97863392, 0.33711682, 0.14479271,
      0.51016811, 0.66528302, 0.54395496, 0.57794893;
  Vec x;
  double s = Nullspace(&A, &x);
  EXPECT_NEAR(s, 0.122287, 1e-7);
  EXPECT_NEAR(x(0), -0.05473917, 1e-7);
  EXPECT_NEAR(x(1),  0.21822937, 1e-7);
  EXPECT_NEAR(x(2), -0.80258116, 1e-7);
  EXPECT_NEAR(x(3),  0.55248805, 1e-7);
}

TEST(Numeric, TinyMatrixSquareTranspose) {
  Mat2 A;
  A = 1.0, 2.0, 3.0, 4.0;
  libmv::Transpose(&A);
  EXPECT_EQ(1.0, A(0, 0));
  EXPECT_EQ(3.0, A(0, 1));
  EXPECT_EQ(2.0, A(1, 0));
  EXPECT_EQ(4.0, A(1, 1));
}

TEST(Numeric, NormalizeL1) {
  Vec2 x;
  x = 1, 2;
  double l1 = NormalizeL1(&x);
  EXPECT_DOUBLE_EQ(3., l1);
  EXPECT_DOUBLE_EQ(1./3., x(0));
  EXPECT_DOUBLE_EQ(2./3., x(1));
}

TEST(Numeric, NormalizeL2) {
  Vec2 x;
  x = 1, 2;
  double l2 = NormalizeL2(&x);
  EXPECT_DOUBLE_EQ(sqrt(5.0), l2);
  EXPECT_DOUBLE_EQ(1./sqrt(5.), x(0));
  EXPECT_DOUBLE_EQ(2./sqrt(5.), x(1));
}

}  // namespace
