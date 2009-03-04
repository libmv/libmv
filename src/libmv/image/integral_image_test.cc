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
#include "libmv/image/integral_image.h"
#include "testing/testing.h"
#include "libmv/logging/logging.h"

using namespace libmv;

namespace {

TEST(IntegralImage, SimpleCase) {
  Mat image(10, 20);
  Mat integral_image;
  image.setZero();
  image(5, 10) = 1.0;
  IntegralImage(image, &integral_image);
  LOG(INFO) << integral_image;
  Mat resid;
  resid.set(integral_image.block<5,10>(0, 0));
  EXPECT_MATRIX_NEAR_ZERO(resid, 1e-15);
  resid.set(integral_image.block<5,10>(5, 0));
  EXPECT_MATRIX_NEAR_ZERO(resid, 1e-15);
  resid.set(integral_image.block<5,10>(0, 10));
  EXPECT_MATRIX_NEAR_ZERO(resid, 1e-15);
  resid.set(integral_image.block<5,10>(5, 10).cwise() - 1.0);
  EXPECT_MATRIX_NEAR_ZERO(resid, 1e-15);
}

TEST(BoxIntegral, ThreeByThree) {
  Mat image(9, 9);
  Mat integral_image;
  image.setZero();
  image.block<5,5>(2, 2).setConstant(1.0);
  IntegralImage(image, &integral_image);
  LOG(INFO) << "\n" << image;
  LOG(INFO) << "\n" << integral_image;
  EXPECT_EQ(25, BoxIntegral(integral_image, 0, 0, 9, 9));
  EXPECT_EQ(25, BoxIntegral(integral_image, 2, 2, 5, 5));
  EXPECT_EQ(9,  BoxIntegral(integral_image, 3, 3, 3, 3));
  EXPECT_EQ(0,  BoxIntegral(integral_image, 0, 0, 2, 9));
  EXPECT_EQ(0,  BoxIntegral(integral_image, 0, 0, 9, 2));
  EXPECT_EQ(0,  BoxIntegral(integral_image, 7, 7, 2, 2));
  EXPECT_EQ(0,  BoxIntegral(integral_image, 0, 7, 9, 2));
  EXPECT_EQ(0,  BoxIntegral(integral_image, 7, 0, 2, 9));
}

}  // namespace
