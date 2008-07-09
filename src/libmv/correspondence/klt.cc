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

#include <cassert>
#include <vector>

#include "libmv/numeric/numeric.h"
#include "libmv/correspondence/klt.h"
#include "libmv/image/image.h"
#include "libmv/image/convolve.h"

using std::vector;

namespace libmv {

void KltContext::DetectGoodFeatures(const FloatImage &image,
                                    vector<DetectedFeature> *features) {
  assert(image.Depth() == 1);

  // TODO(keir): These should probably be passed in, because the image
  // derivatives are needed by many other functions.
  FloatImage gradient_x, gradient_y;
  ImageDerivatives(image, 0.9, &gradient_x, &gradient_y);
  WritePgm(gradient_x, "gradient_x.pgm");
  WritePgm(gradient_y, "gradient_y.pgm");

  // Compute second derivatives.
  FloatImage gradient_xx, gradient_xy, gradient_yy;
  MultiplyElements(gradient_x, gradient_y, &gradient_xy);
  MultiplyElements(gradient_x, gradient_x, &gradient_xx);
  MultiplyElements(gradient_y, gradient_y, &gradient_yy);
  WritePgm(gradient_xx, "gradient_xx.pgm");
  WritePgm(gradient_xy, "gradient_xy.pgm");
  WritePgm(gradient_yy, "gradient_yy.pgm");

  // Sum the second derivatives over tracking window for each pixel.
  FloatImage gxx, gxy, gyy;
  BoxFilter(gradient_xx, 7, &gxx);
  BoxFilter(gradient_xy, 7, &gxy);
  BoxFilter(gradient_yy, 7, &gyy);

  WritePgm(gradient_xx, "gxx.pgm");
  WritePgm(gradient_xy, "gxy.pgm");
  WritePgm(gradient_yy, "gyy.pgm");

  FloatImage trackness;
  ComputeTrackness(gxx, gxy, gyy, &trackness);

  WritePgm(trackness, "trackerness.pgm");

  // Non-maxima suppresion.
  FindLocalMaxima(trackness, features);
}

// Compute trackness of every pixel given the gradient matrix.
void KltContext::ComputeTrackness(const FloatImage &gxx,
                                  const FloatImage &gxy,
                                  const FloatImage &gyy,
                                  FloatImage *trackness_pointer) {
  FloatImage &trackness = *trackness_pointer;
  trackness.ResizeLike(gxx);
  for (int i = 0; i < trackness.Height(); ++i) {
    for (int j = 0; j < trackness.Width(); ++j) {
      trackness(i,j) = MinEigenValue(gxx(i, j), gxy(i, j), gyy(i, j));
    }
  }
}

// TODO(keir): Add option to drop found features that are to close. Also drop
// features that are below a certain threshold in 'distinctiveness' regardless
// if they are a local maxima.
void KltContext::FindLocalMaxima(const FloatImage &trackness,
                                 vector<DetectedFeature> *points) {
  for (int i = 1; i < trackness.Height()-1; ++i) {
    for (int j = 1; j < trackness.Width()-1; ++j) {
      if (   trackness(i,j) > trackness(i-1, j-1)
          && trackness(i,j) > trackness(i-1, j  )
          && trackness(i,j) > trackness(i-1, j+1)
          && trackness(i,j) > trackness(i  , j-1)
          && trackness(i,j) > trackness(i  , j+1)
          && trackness(i,j) > trackness(i+1, j-1)
          && trackness(i,j) > trackness(i+1, j  )
          && trackness(i,j) > trackness(i+1, j+1) ) {
        DetectedFeature p;
        p.int_y = i;
        p.int_x = j;
        p.trackness = trackness(i,j);
        points->push_back(p);
      }
    }
  }
}

}  // namespace libmv
