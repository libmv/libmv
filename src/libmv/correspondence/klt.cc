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
#include "libmv/image/image_io.h"
#include "libmv/image/convolve.h"

using std::vector;

namespace libmv {

void KltContext::DetectGoodFeatures(const FloatImage &image,
                                    FeatureList *features) {
  assert(image.Depth() == 1);

  // TODO(keir): These should probably be passed in, because the image
  // derivatives are needed by many other functions.
  FloatImage gradient_x, gradient_y;
  ImageDerivatives(image, 0.9, &gradient_x, &gradient_y);
  WritePnm(gradient_x, "gradient_x.pgm");
  WritePnm(gradient_y, "gradient_y.pgm");

  FloatImage gxx, gxy, gyy;
  ComputeGradientMatrix(gradient_x, gradient_y, &gxx, &gxy, &gyy);

  FloatImage trackness;
  double trackness_mean;
  ComputeTrackness(gxx, gxy, gyy, &trackness, &trackness_mean);
  min_trackness_ = trackness_mean;
  WritePnm(trackness, "trackerness.pgm");

  FindLocalMaxima(trackness, features);

  RemoveTooCloseFeatures(features);
}

void KltContext::ComputeGradientMatrix(const FloatImage &gradient_x,
                                       const FloatImage &gradient_y,
                                       FloatImage *gxx,
                                       FloatImage *gxy,
                                       FloatImage *gyy ) {
  FloatImage gradient_xx, gradient_xy, gradient_yy;
  MultiplyElements(gradient_x, gradient_y, &gradient_xy);
  MultiplyElements(gradient_x, gradient_x, &gradient_xx);
  MultiplyElements(gradient_y, gradient_y, &gradient_yy);

  WritePnm(gradient_xx, "gradient_xx.pgm");
  WritePnm(gradient_xy, "gradient_xy.pgm");
  WritePnm(gradient_yy, "gradient_yy.pgm");

  // Sum the gradient matrix over tracking window for each pixel.
  BoxFilter(gradient_xx, WindowSize(), gxx);
  BoxFilter(gradient_xy, WindowSize(), gxy);
  BoxFilter(gradient_yy, WindowSize(), gyy);

  WritePnm(*gxx, "gxx.pgm");
  WritePnm(*gxy, "gxy.pgm");
  WritePnm(*gyy, "gyy.pgm");
}

void KltContext::ComputeTrackness(const FloatImage &gxx,
                                  const FloatImage &gxy,
                                  const FloatImage &gyy,
                                  FloatImage *trackness_pointer,
                                  double *trackness_mean) {
  FloatImage &trackness = *trackness_pointer;
  trackness.ResizeLike(gxx);
  *trackness_mean = 0;
  for (int i = 0; i < trackness.Height(); ++i) {
    for (int j = 0; j < trackness.Width(); ++j) {
      double t = MinEigenValue(gxx(i, j), gxy(i, j), gyy(i, j));
      trackness(i,j) = t;
      *trackness_mean += t;
    }
  }
  *trackness_mean /= trackness.Size();
}

void KltContext::FindLocalMaxima(const FloatImage &trackness,
                                 FeatureList *features) {
  for (int i = 1; i < trackness.Height()-1; ++i) {
    for (int j = 1; j < trackness.Width()-1; ++j) {
      if (   trackness(i,j) >= min_trackness_
          && trackness(i,j) >= trackness(i-1, j-1)
          && trackness(i,j) >= trackness(i-1, j  )
          && trackness(i,j) >= trackness(i-1, j+1)
          && trackness(i,j) >= trackness(i  , j-1)
          && trackness(i,j) >= trackness(i  , j+1)
          && trackness(i,j) >= trackness(i+1, j-1)
          && trackness(i,j) >= trackness(i+1, j  )
          && trackness(i,j) >= trackness(i+1, j+1)) {
        Feature p;
        p.position(1) = i;
        p.position(0) = j;
        p.trackness = trackness(i,j);
        features->push_back(p);
      }
    }
  }
}

static double dist2(const Vec2 &x, const Vec2 &y) {
  double a = x(0) - y(0);
  double b = x(1) - y(1);
  return a * a + b * b;
}

void KltContext::RemoveTooCloseFeatures(FeatureList *features) {
  double treshold = min_feature_dist_ * min_feature_dist_;

  FeatureList::iterator i = features->begin();
  while (i != features->end()) {
    bool i_deleted = false;
    FeatureList::iterator j = i;
    ++j;
    while (j != features->end() && !i_deleted) {
      if (dist2(i->position, j->position) < treshold) {
        FeatureList::iterator to_delete;
        if (i->trackness < j->trackness) {
          to_delete = i;
          ++i;
          i_deleted = true;
        } else {
          to_delete = j;
          ++j;
        }
        features->erase(to_delete);
      } else {
        ++j;
      }
    }
    if (!i_deleted) {
      ++i;
    }
  }
}

void KltContext::TrackFeature(const ImagePyramid &pyramid1,
                              const Feature &feature1,
                              const ImagePyramid &pyramid2,
                              const ImagePyramid &pyramid2_gx,
                              const ImagePyramid &pyramid2_gy,
                              Feature *feature2_pointer) {
  feature2_pointer->position = feature1.position;
  for (int i = pyramid1.NumLevels(); i >= 0; --i) {
    TrackFeatureOneLevel(pyramid1.Level(i), feature1,
                         pyramid2.Level(i),
                         pyramid2_gx.Level(i),
                         pyramid2_gy.Level(i),
                         feature2_pointer);
  }
}

void KltContext::TrackFeatureOneLevel(const FloatImage &image1,
                                      const Feature &feature1,
                                      const FloatImage &image2,
                                      const FloatImage &image2_gx,
                                      const FloatImage &image2_gy,
                                      Feature *feature2_pointer) {
  Feature &feature2 = *feature2_pointer;

  const int max_iteration_ = 10;
  for (int i = 0; i < max_iteration_; ++i) {
    // Compute gradient matrix and error vector.
    float gxx, gxy, gyy, ex, ey;
    ComputeTrackingEquation(image1, image2, image2_gx, image2_gy,
                            feature1.position, feature2.position,
                            &gxx, &gxy, &gyy, &ex, &ey);
    // Solve the linear system for deltad.
    float dx, dy;
    SolveTrackingEquation(gxx, gxy, gyy, ex, ey, 1e-6, &dx, &dy);
    // Update feature2 position.
    feature2.position(0) += dx;
    feature2.position(1) += dy;
  }
}

void KltContext::ComputeTrackingEquation(const FloatImage &image1,
                                         const FloatImage &image2,
                                         const FloatImage &image2_gx,
                                         const FloatImage &image2_gy,
                                         const Vec2 &position1,
                                         const Vec2 &position2,
                                         float *gxx,
                                         float *gxy,
                                         float *gyy,
                                         float *ex,
                                         float *ey) {
  int half_width = HalfWindowSize();
  *gxx = 0;
  *gxy = 0;
  *gyy = 0;
  *ex = 0;
  *ey = 0;
  for (int i = -half_width; i <= half_width; ++i) {
    for (int j = -half_width; j <= half_width; ++j) {
      float x1 = position1(0) + j;
      float y1 = position1(1) + i;
      float x2 = position2(0) + j;
      float y2 = position2(1) + i;
      // TODO(pau): should do boundary checking outside this loop, and call here
      // a sampler that does not boundary checking.
      float I = SampleLinear(image1, y1, x1);
      float J = SampleLinear(image2, y2, x2);
      float gx = SampleLinear(image2_gx, y2, x2);
      float gy = SampleLinear(image2_gy, y2, x2);
      *gxx += gx * gx;
      *gxy += gx * gy;
      *gyy += gy * gy;
      *ex += (I - J) * gx;
      *ey += (I - J) * gy;
    }
  }
}
  
bool KltContext::SolveTrackingEquation(float gxx, float gxy, float gyy,
                                       float ex, float ey,
                                       float small_determinant_threshold,
                                       float *dx, float *dy) {
  float det = gxx * gyy - gxy * gxy;
  if (det < small_determinant_threshold) {
    *dx = 0;
    *dy = 0;
    return false;
  }
  *dx = (gyy * ex - gxy * ey) / det;
  *dy = (gxx * ey - gxy * ex) / det;
  return true;
}

void KltContext::DrawFeatureList(const FeatureList &features,
                                 const Vec3 &color,
                                 FloatImage *image) {
  for (FeatureList::const_iterator i = features.begin();
       i != features.end(); ++i) {
    DrawFeature(*i, color, image);
  }
}
        
void KltContext::DrawFeature(const Feature &feature,
                             const Vec3 &color,
                             FloatImage *image) {
  assert(image->Depth() == 3);
  
  const int cross_width = 5;
  int x = lround(feature.position(0));
  int y = lround(feature.position(1));
  if (!image->Contains(y,x)) {
    return;
  }

  // Draw vertical line.
  for (int i = max(0, y - cross_width);
       i < min(image->Height(), y + cross_width + 1); ++i) {
    for (int k = 0; k < 3; ++k) {
      (*image)(i, x, k) = color(k);
    }
  }
  // Draw horizontal line.
  for (int j = max(0, x - cross_width);
       j < min(image->Width(), x + cross_width + 1); ++j) {
    for (int k = 0; k < 3; ++k) {
      (*image)(y, j, k) = color(k);
    }
  }
}
        
}  // namespace libmv
