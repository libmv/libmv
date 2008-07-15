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

#include "libmv/image/image.h"
#include "libmv/image/convolve.h"

namespace libmv {

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

// Compute a Gaussian kernel and derivative, such that you can take the
// derivitave of an image by convolving with the kernel horizontally then the
// derivative vertically to get (eg) the y derivative.
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
  *kernel = 0.0;
  *derivative = 0.0;
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

void ConvolveHorizontal(const FloatImage &in,
                        const Vec &kernel,
                        FloatImage *out_pointer) {
  int halfwidth = kernel.length() / 2;
  int num_columns = in.Width();
  int num_rows = in.Height();
  FloatImage &out = *out_pointer;
  out.ResizeLike(in);

  assert(kernel.length() % 2 == 1);
  assert(&in != out_pointer);

  for (int j = 0; j < num_rows; ++j)  {
    for (int i = 0; i < num_columns; ++i)  {
      double sum = 0.0;
      int l = 0;
      for (int k = kernel.length()-1; k >= 0; --k, ++l) {
        int ii = i - halfwidth + l;
        if (0 <= ii && ii < num_columns) {
          sum += in(j, ii) * kernel(k);
        }
      }
      out(j, i) = sum;
    }
  }
}

// This could certainly be accelerated.
void ConvolveVertical(const FloatImage &in,
                      const Vec &kernel,
                      FloatImage *out_pointer) {
  int halfwidth = kernel.length() / 2;
  int num_columns = in.Width();
  int num_rows = in.Height();
  FloatImage &out = *out_pointer;
  out.ResizeLike(in);

  assert(kernel.length() % 2 == 1);
  assert(&in != out_pointer);

  for (int i = 0; i < num_columns; ++i)  {
    for (int j = 0; j < num_rows; ++j)  {
      double sum = 0.0;
      int l = 0;
      for (int k = kernel.length()-1; k >= 0; --k, ++l) {
        int jj = j - halfwidth + l;
        if (0 <= jj && jj < num_rows) {
          sum += in(jj, i) * kernel(k);
        }
      }
      out(j, i) = sum;
    }
  }
}

void ConvolveGaussian(const FloatImage &in,
                      double sigma,
                      FloatImage *out_pointer) {
  Vec kernel, derivative;
  ComputeGaussianKernel(sigma, &kernel, &derivative);

  FloatImage tmp;
  ConvolveVertical(in, kernel, &tmp);
  ConvolveHorizontal(tmp, kernel, out_pointer);
}

void ImageDerivatives(const FloatImage &in,
                      double sigma,
                      FloatImage *gradient_x,
                      FloatImage *gradient_y) {
  Vec kernel, derivative;
  ComputeGaussianKernel(sigma, &kernel, &derivative);

  // Compute first derivative in x.
  FloatImage tmp;
  ConvolveHorizontal(in, derivative, &tmp);
  ConvolveVertical(tmp, kernel, gradient_x);

  // Compute first derivative in y.
  ConvolveHorizontal(in, kernel, &tmp);
  ConvolveVertical(tmp, derivative, gradient_y);
}

void FirstOrderGaussianJet(const FloatImage &in,
                           double sigma,
                           FloatImage *blured_image,
                           FloatImage *gradient_x,
                           FloatImage *gradient_y) {
  Vec kernel, derivative;
  ComputeGaussianKernel(sigma, &kernel, &derivative);
  FloatImage tmp;

  // Compute convolved image.
  ConvolveVertical(in, kernel, &tmp);
  ConvolveHorizontal(tmp, kernel, blured_image);

  // Compute first derivative in x.
  ConvolveHorizontal(in, derivative, gradient_x);

  // Compute first derivative in y.
  ConvolveHorizontal(in, kernel, &tmp);
  ConvolveVertical(tmp, derivative, gradient_y);
}

void BoxFilterHorizontal(const FloatImage &in,
                         int window_size,
                         FloatImage *out_pointer) {
  assert(in.Depth() == 1);

  FloatImage &out = *out_pointer;
  out.ResizeLike(in);
  int half_width = (window_size - 1) / 2;

  for (int i=0; i<in.Height(); ++i) {
    float sum = 0;
    // Init sum.
    for (int j=0; j<half_width; ++j) {
      sum += in(i,j);
    }
    // Fill left border.
    for (int j=0; j < half_width + 1; ++j) {
      sum += in(i, j + half_width);
      out(i,j) = sum;
    }
    // Fill interior.
    for (int j = half_width + 1; j<in.Width()-half_width; ++j) {
      sum -= in(i, j - half_width - 1);
      sum += in(i, j + half_width);
      out(i,j) = sum;
    }
    // Fill right border.
    for (int j = in.Width() - half_width; j<in.Width(); ++j) {
      sum -= in(i, j - half_width - 1);
      out(i,j) = sum;
    }
  }
}

void BoxFilterVertical(const FloatImage &in,
                       int window_size,
                       FloatImage *out_pointer) {
  // TODO(pau) this should be done faster without calling Convolve.
  Vec kernel(window_size);
  for (int i = 0; i < window_size; ++i) kernel(i) = 1;
  ConvolveVertical(in, kernel, out_pointer);
}

void BoxFilter(const FloatImage &in,
               int box_width,
               FloatImage *out) {
  FloatImage tmp;
  BoxFilterHorizontal(in, box_width, &tmp);
  BoxFilterVertical(tmp, box_width, out);
};

}  // namespace libmv
