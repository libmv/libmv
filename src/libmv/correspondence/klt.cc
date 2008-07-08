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

#include "libmv/correspondence/klt.h"
#include "libmv/image/convolve.h"
#include <cassert>

using namespace std;
using namespace libmv;

namespace libmv {

void KltContext::DetectGoodFeatures(const FloatImage &im,
		std::vector<DetectedFeature> *features) {
  assert(im.Depth() == 1);

  FloatImage gxx, gxy, gyy;
  ComputeGradientMatrix(im, &gxx, &gxy, &gyy);

  FloatImage trackness;
  ComputeTrackness(gxx, gxy, gyy, &trackness);

  // Non-maxima suppresion.
  FindLocalMaxima(trackness, features);
}

void KltContext::ComputeGradientMatrix(const FloatImage &im,
      	             FloatImage *gxx_pointer,
      		     FloatImage *gxy_pointer,
      		     FloatImage *gyy_pointer) {
  int height = im.Height();
  int width = im.Width();
  FloatImage gxx(height,width);
  FloatImage gxy(height,width);
  FloatImage gyy(height,width);

  for (int j = 0; j < width; ++j) {
    gxx(0,j) = 0;
    gxy(0,j) = 0;
    gyy(0,j) = 0;
  }
  for (int i = 1; i < height-1; ++i) {
    gxx(i,0) = 0;
    gxy(i,0) = 0;
    gyy(i,0) = 0;
    for (int j = 1; j < width-1; ++j) {
      float gx = im(i + 1, j) - im(i - 1, j); // No need to divide by two.
      float gy = im(i, j + 1) - im(i, j - 1);
      gxx(i,j) = gx * gx;
      gxy(i,j) = gx * gy;
      gyy(i,j) = gy * gy;
    }
    gxx(i,width-1) = 0;
    gxy(i,width-1) = 0;
    gyy(i,width-1) = 0;
  }
  for (int j = 0; j < width; ++j) {
    gxx(height-1,j) = 0;
    gxy(height-1,j) = 0;
    gyy(height-1,j) = 0;
  }

  IntegralImage(gxx, WindowSize(), gxx_pointer);
  IntegralImage(gxy, WindowSize(), gxy_pointer);
  IntegralImage(gyy, WindowSize(), gyy_pointer);
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
      trackness(i,j) = MinEigenValue(gxx(i,j), gxy(i,j), gyy(i,j));
    }
  }
}

void KltContext::FindLocalMaxima(const FloatImage &trackness,
		                 std::vector<DetectedFeature> *points) {
  for (int i = 1; i < trackness.Height()-1; ++i) {
    for (int j = 1; j < trackness.Width()-1; ++j) {
      if (  trackness(i,j) > trackness(i-1,j-1)
	 && trackness(i,j) > trackness(i-1,j  )
	 && trackness(i,j) > trackness(i-1,j+1)
	 && trackness(i,j) > trackness(i  ,j-1)
	 && trackness(i,j) > trackness(i  ,j+1)
	 && trackness(i,j) > trackness(i+1,j-1)
	 && trackness(i,j) > trackness(i+1,j  )
	 && trackness(i,j) > trackness(i+1,j+1) )
      {
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

