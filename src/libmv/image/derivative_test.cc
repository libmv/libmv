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

#include "libmv/image/array_nd.h"
#include "libmv/image/derivative.h"
#include "testing/testing.h"

using namespace libmv;

namespace {

TEST(Derivatives3D, SimpleCase) {
  Array3Df f(5, 5, 5);
  for (int x = 0; x < 5; ++x) {
    for (int y = 0; y < 5; ++y) {
      for (int z = 0; z < 5; ++z) {
        f(x, y, z) = x + y*y + 5*z*z;
      }
    }
  }
  Vec3 g = Gradient3D(f, 2, 2, 2);
  EXPECT_EQ(1, g(0));
  EXPECT_EQ(4, g(1));
  EXPECT_EQ(20, g(2));

  Mat3 H = Hessian3D(f, 2, 2, 2);
  Mat3 H_expected;
  H_expected << 0, 0,  0,
                0, 2,  0,
                0, 0, 10;
  EXPECT_MATRIX_NEAR(H_expected, H, 1e-8);
}

TEST(Derivatives3D, CrossTerms) {
  Array3Df f(5, 5, 5);
  for (int x = 0; x < 5; ++x) {
    for (int y = 0; y < 5; ++y) {
      for (int z = 0; z < 5; ++z) {
        f(x, y, z) = x + y*y + 5*z*z + 3*x*y + 2*x*z + 7*y*z;
      }
    }
  }
  Mat3 H = Hessian3D(f, 2, 2, 2);
  Mat3 H_expected;
  H_expected << 0, 3,  2,
                3, 2,  7,
                2, 7, 10;
  EXPECT_MATRIX_NEAR(H_expected, H, 1e-8);
}

}  // namespace
