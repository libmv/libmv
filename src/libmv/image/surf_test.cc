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

#include "libmv/numeric/numeric.h"
#include "libmv/image/array_nd.h"
#include "libmv/image/surf.h"
#include "testing/testing.h"

using namespace libmv;

namespace {

TEST(RefineMaxima3D, SimpleRefinement) {
  Array3Df f(5, 5, 5);
  f.Fill(0.0);
  for (int x = 0; x < 5; ++x) {
    for (int y = 0; y < 5; ++y) {
      for (int z = 0; z < 5; ++z) {
        int p = x - 2;
        int q = y - 2;
        int r = z - 2;
        f(x, y, z) = p*p + q*q + r*r;
      }
    }
  }
  for (int dx = -1; dx <= 1; ++dx) {
    for (int dy = -1; dy <= 1; ++dy) {
      for (int dz = -1; dz <= 1; ++dz) {
        Vec3 delta = RefineMaxima3D(f, 2 + dx, 2 + dy, 2 + dz);
        EXPECT_EQ(-dx, delta(0));
        EXPECT_EQ(-dy, delta(1));
        EXPECT_EQ(-dz, delta(2));
        LOG(INFO) << "\n" << delta;
      }
    }
  }
}

}  // namespace
