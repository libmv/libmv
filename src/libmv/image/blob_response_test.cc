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
#include "libmv/image/image_io.h"
#include "libmv/image/blob_response.h"
#include "libmv/image/convolve.h"
#include "libmv/image/non_maximal_suppression.h"
#include "testing/testing.h"

using namespace libmv;

namespace {

TEST(SimpleCase, OnePeak) {
  Array3Df peak(21, 21);
  Array3Df integral_image;
  Array3Df response;
  peak.Fill(0.0);
  peak(10, 10) = 1.0;
  IntegralImage(peak, &integral_image);
  // These resize and fills are to prevent valgrind from complaining about the
  // uninitialized borders which are hit when printing. When border
  // initialization is fixed, remove these (throughout).
  response.ResizeLike(peak);
  response.Fill(0.0);
  BlobResponse(integral_image, 3, 1, &response);
  PrintArray(response);
  EXPECT_TRUE(IsLocalMax2D(response, 3, 10, 10));
}

TEST(SimpleCase, TwoPeaks) {
  Array3Df peak(21, 21);
  Array3Df integral_image;
  Array3Df response;
  peak.Fill(0.0);
  peak(5, 5) = 1.0;
  peak(11, 11) = 1.0;
  IntegralImage(peak, &integral_image);
  response.ResizeLike(peak);
  response.Fill(0.0);
  BlobResponse(integral_image, 3, 1, &response);
  PrintArray(response);
  EXPECT_TRUE(IsLocalMax2D(response, 3, 5, 5));
  EXPECT_TRUE(IsLocalMax2D(response, 3, 11, 11));
  EXPECT_EQ(21, response.rows());
  EXPECT_EQ(21, response.cols());
}

TEST(SimpleCase, OnePeakScaledDown) {
  Array3Df peak(14, 14);
  Array3Df integral_image;
  Array3Df response;
  peak.Fill(0.0);
  peak(9, 8)  = 1.0; // Making a single spike doesn't work 
  peak(9, 9)  = 1.0; // well in the scaled down case, so make
  peak(10, 8) = 1.0; // the blob a 2x2 block.
  peak(10, 9) = 1.0;
  response.resize(7, 7);
  response.Fill(0.0);
  IntegralImage(peak, &integral_image);
  BlobResponse(integral_image, 3, 2, &response);
  PrintArray(response);
  EXPECT_TRUE(IsLocalMax2D(response, 3,  4,  4));
  EXPECT_EQ(7, response.rows());
  EXPECT_EQ(7, response.cols());
}

TEST(SimpleCase, TwoPeaksScaled) {
  Array3Df peak(40, 40);
  Array3Df integral_image;
  Array3Df response;
  peak.Fill(0.0);

  peak(20, 22) = 1.0;  // 2x2 peak like above.
  peak(20, 23) = 1.0;
  peak(21, 22) = 1.0;
  peak(21, 23) = 1.0;

  peak(10, 14) = 1.0;
  peak(10, 15) = 1.0;
  peak(11, 14) = 1.0;
  peak(11, 15) = 1.0;

  response.resize(20, 20);
  response.Fill(0.0);
  IntegralImage(peak, &integral_image);
  BlobResponse(integral_image, 3, 2, &response);
  PrintArray(response);
  EXPECT_TRUE(IsLocalMax2D(response, 3, 10, 11));
  EXPECT_TRUE(IsLocalMax2D(response, 3,  5,  7));
  EXPECT_EQ(20, response.rows());
  EXPECT_EQ(20, response.cols());
}

}  // namespace
