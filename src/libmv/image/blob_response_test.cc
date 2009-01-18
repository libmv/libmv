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
  Array3Df integral_image(51, 51);
  Array3Df response(51, 51);
  peak.Fill(0.0);
  peak(10, 10) = 1.0;
  IntegralImage(peak, &integral_image);
  BlobResponse(integral_image, 3, 1, &response);
  PrintArray(response);
  EXPECT_TRUE(IsLocalMax2D(response, 3, 10, 10));
}

TEST(SimpleCase, TwoPeaks) {
  Array3Df peak(21, 21);
  Array3Df integral_image(51, 51);
  Array3Df response(51, 51);
  peak.Fill(0.0);
  peak(5, 5) = 1.0;
  peak(11, 11) = 1.0;
  IntegralImage(peak, &integral_image);
  BlobResponse(integral_image, 3, 1, &response);
  PrintArray(response);
  EXPECT_TRUE(IsLocalMax2D(response, 3, 5, 5));
  EXPECT_TRUE(IsLocalMax2D(response, 3, 11, 11));
}

}  // namespace
