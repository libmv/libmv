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

#include <cmath>

#include "libmv/image/convolve.h"

namespace libmv {

void ConvertByteImageToFloatImage(const ByteImage &byte_image,
                                  const FloatImage &float_image) {
(void)byte_image;
(void)float_image;
}

// Zero mean Gaussian.
double Gaussian(double x, double sigma) {
  return 1/sqrt(2*M_PI*sigma*sigma) * exp(-(x*x/2/sigma/sigma));
}
double GaussianDerivative(double x, double sigma) {
  return -x / sigma / sigma * Gaussian(x, sigma);
}
// Solve the inverse of the Gaussian for positive x.
double GaussianInversePositive(double y, double sigma) {
  return sqrt(-2 * sigma * sigma * log(y * sigma * sqrt(2*M_PI)));
}

void ComputeGaussianKernel(double sigma, Vec *kernel, Vec *derivative) {
  assert(sigma >= 0.0);

  // 0.004 implies a 3 pixel kernel with 1 pixel sigma.
  const float truncation_factor = 0.004f;

  // Calculate the kernel size based on sigma such that it is odd.
  float precisehalfwidth = GaussianInversePositive(truncation_factor, sigma);
  int width = lround(2*precisehalfwidth);
  if (width % 2 == 0) {
    width++;
  }
  // Calculate the gaussian kernel and its derivative.
  kernel->resize(width);
  derivative->resize(width);
  int halfwidth = width / 2;
  for (int i = -halfwidth; i <= halfwidth; ++i)  {
    (*kernel)(i + halfwidth) = Gaussian(i, sigma);
    (*derivative)(i + halfwidth) = GaussianDerivative(i, sigma);
  }
  // Since images should not get brighter or darker, normalize.
  NormalizeL1(kernel);
  // Normalize the derivative differently. See
  // www.cs.duke.edu/courses/spring03/cps296.1/handouts/Image%20Processing.pdf
  double factor = 0.;
  for (int i = -halfwidth; i <= halfwidth; ++i)  {
    factor -= i*(*derivative)(i+halfwidth);
  }
  *derivative /= factor;
}

}  // namespace libmv
