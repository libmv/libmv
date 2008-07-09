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
#include "libmv/image/convolve.h"
#include "libmv/image/image.h"
#include "testing/testing.h"

#include <iostream>
using namespace libmv;

namespace {

TEST(Convolve, ComputeGaussianKernel) {
  Vec kernel, derivative;
  ComputeGaussianKernel(1, &kernel, &derivative);
  EXPECT_EQ(7, kernel.length());
  // TODO(keir): Put in a more thorough test here!
}

TEST(Convolve, ConvolveGaussian) {
  FloatImage im(10,10);
  im.Fill(1);
  FloatImage blured;
  ConvolveGaussian(im, 3, &blured);
  EXPECT_NEAR(im(5,5), 1, 1e-7);
}

TEST(Convolve, BoxFilterHorizontal) {
  FloatImage im(10,10), convolved, filtered;
  im.Fill(1);
  BoxFilterHorizontal(im,3,&filtered);
  Vec kernel(3);
  kernel = 1.;
  ConvolveHorizontal(im,kernel,&convolved);
  EXPECT_EQ(filtered(5,5), 3);
  EXPECT_TRUE(filtered == convolved);
}

}  // namespace
