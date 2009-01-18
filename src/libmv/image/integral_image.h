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
#include "third_party/glog/src/glog/logging.h"

// Create a summed area table: http://en.wikipedia.org/wiki/Summed_Area_Table
template <typename TImage, typename TIntegralImage>
inline void IntegralImage(const TImage &image, TIntegralImage *integral_image) {
  typedef typename TIntegralImage::Scalar Scalar;
  integral_image->resize(image.rows(), image.cols());

  // Split first row from the rest to avoid an if in the inner loop.
  Scalar row_sum = 0.;
  for (int c = 0; c < image.cols(); ++c) {
    row_sum += image(0, c);
    (*integral_image)(0, c) = row_sum;
  }

  // Each pixel is a sum of all the pixels to the left and up of that pixel.
  for (int r = 1; r < image.rows(); ++r) {
    row_sum = 0.;
    for (int c = 0; c < image.cols(); ++c) {
      row_sum += image(r, c);
      (*integral_image)(r, c) = row_sum + (*integral_image)(r - 1, c);
    }
  }
}

// The sum of the pixels in an area bounded by row, column, width, and height.
// If the bounding box exceeds the image, then the partial sum is returned (or
// zero if the box completely misses the image).
// This uses the integral image 'trick' to compute the sum:
//
//                 c1         c2
//     +------------+----------+-----+
//     |            |          |     |
//     |     A      |    B     |     |  The coordinate (r2, c2) is equal to
//     |            |          |     |  A + B + C + D in the integral image.
// r1  +------------+----------+-----+  In other words, II(r2, c2) is the 
//     |            |  target  |     |  integral of the areas A, B, C, D.
//     |     C      | integral |     |  By using the property that each point  
//     |            |   (D)    |     |  is the sum of all the pixels up and to 
// r2  +------------+----------+-----+  left of this one (including this one), 
//     |            |          |     |  then the integral of area D is:
//     |            |          |     |                                         
//     |            |          |     |    sum(D) =   II(r2,     c2)
//     |            |          |     |             - II(r2,     c1 - 1)
//     |            |          |     |             - II(r1 - 1, c2)
//     |            |          |     |             + II(r1 - 1, c1 - 1)
//     |            |          |     |
//     +------------+----------+-----+
//
template <typename TImage>
inline typename TImage::Scalar BoxIntegral(const TImage &integral_image,
                                  int row, int col,
                                  int rows, int cols) {
  // The subtraction by one for row/col is because row/col is inclusive.
  int r1 = std::min(row,          integral_image.rows()) - 1;
  int c1 = std::min(col,          integral_image.cols()) - 1;
  int r2 = std::min(row + rows,   integral_image.rows()) - 1;
  int c2 = std::min(col + cols,   integral_image.cols()) - 1;

  typedef typename TImage::Scalar T;
  T A(0); if (r1 >= 0 && c1 >= 0) A = integral_image(r1, c1);
  T B(0); if (r1 >= 0 && c2 >= 0) B = integral_image(r1, c2);
  T C(0); if (r2 >= 0 && c1 >= 0) C = integral_image(r2, c1);
  T D(0); if (r2 >= 0 && c2 >= 0) D = integral_image(r2, c2);

  T sum = A - B - C + D;
  assert (sum >= 0);
  return sum;
}
