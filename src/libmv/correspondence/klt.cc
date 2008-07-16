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
#include "libmv/image/sample.h"

using std::vector;

namespace libmv {

void KltContext::DetectGoodFeatures(const ImagePyramid &pyramid,
                                    FeatureList *features) {

  FloatImage gxx, gxy, gyy;
  ComputeGradientMatrix(pyramid.GradientX(0), pyramid.GradientY(0),
                        &gxx, &gxy, &gyy);

  FloatImage trackness;
  double trackness_mean;
  ComputeTrackness(gxx, gxy, gyy, &trackness, &trackness_mean);
  min_trackness_ = trackness_mean;

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

  // Sum the gradient matrix over tracking window for each pixel.
  BoxFilter(gradient_xx, WindowSize(), gxx);
  BoxFilter(gradient_xy, WindowSize(), gxy);
  BoxFilter(gradient_yy, WindowSize(), gyy);
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

// TODO(keir): Use Stan's neat trick of using a 'punch-out' array to detect
// too-closes features.
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
                              Feature *feature2_pointer) {
  const int highest_level = pyramid1.NumLevels() - 1;

  Vec2 position1, position2;
  position2(0) = feature1.position(0) / pow(2., highest_level + 1);
  position2(1) = feature1.position(1) / pow(2., highest_level + 1);

  for (int i = highest_level; i >= 0; --i) {
    position1(0) = feature1.position(0) / pow(2., i);
    position1(1) = feature1.position(1) / pow(2., i);
    position2(0) *= 2;
    position2(1) *= 2;

    TrackFeatureOneLevel(pyramid1.Level(i),
                         position1,
                         pyramid2.Level(i),
                         pyramid2.GradientX(i),
                         pyramid2.GradientY(i),
                         &position2);
    printf("level %i position2 %g %g\n", i, position2(0), position2(1));
  }
  feature2_pointer->position = position2;
}

void KltContext::TrackFeatureOneLevel(const FloatImage &image1,
                                      const Vec2 &position1,
                                      const FloatImage &image2,
                                      const FloatImage &image2_gx,
                                      const FloatImage &image2_gy,
                                      Vec2 *position2_pointer) {
  Vec2 &position2 = *position2_pointer;

  for (int i = 0; i < max_iterations_; ++i) {
    // Compute gradient matrix and error vector.
    float gxx, gxy, gyy, ex, ey;
    ComputeTrackingEquation(image1, image2, image2_gx, image2_gy,
                            position1, position2,
                            &gxx, &gxy, &gyy, &ex, &ey);
    // Solve the linear system for deltad.
    float dx, dy;
    SolveTrackingEquation(gxx, gxy, gyy, ex, ey, &dx, &dy);
    // Update feature2 position.
    position2(0) += dx;
    position2(1) += dy;

    if (Square(dx) + Square(dy) < min_update_distance2_) {
      break;
    }
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
                                       float *dx, float *dy) {
  float det = gxx * gyy - gxy * gxy;
  if (det < min_determinant_) {
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
