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

static void FindLocalMaxima(const FloatImage &trackness,
                            float min_trackness,
                            KLTContext::FeatureList *features) {
  for (int i = 1; i < trackness.Height()-1; ++i) {
    for (int j = 1; j < trackness.Width()-1; ++j) {
      if (   trackness(i,j) >= min_trackness
          && trackness(i,j) >= trackness(i-1, j-1)
          && trackness(i,j) >= trackness(i-1, j  )
          && trackness(i,j) >= trackness(i-1, j+1)
          && trackness(i,j) >= trackness(i  , j-1)
          && trackness(i,j) >= trackness(i  , j+1)
          && trackness(i,j) >= trackness(i+1, j-1)
          && trackness(i,j) >= trackness(i+1, j  )
          && trackness(i,j) >= trackness(i+1, j+1)) {
        KLTPointFeature *p = new KLTPointFeature;
        p->position(1) = i;
        p->position(0) = j;
        p->trackness = trackness(i,j);
        features->push_back(p);
      }
    }
  }
}

// Compute the gradient matrix noted by Z in Good Features to Track.
//
//   Z = [gxx gxy; gxy gyy]
//
// This function computes the matrix for every pixel.
static void ComputeGradientMatrix(const Array3Df &image_and_gradients,
                                       int window_size,
                                       Array3Df *gradient_matrix) {
  Array3Df gradients;
  gradients.ResizeLike(image_and_gradients);
  for (int j = 0; j < image_and_gradients.Height(); ++j) {
    for (int i = 0; i < image_and_gradients.Width(); ++i) {
      float gx = image_and_gradients(j, i, 1);
      float gy = image_and_gradients(j, i, 2);
      gradients(j, i, 0) = gx * gx;
      gradients(j, i, 1) = gx * gy;
      gradients(j, i, 2) = gy * gy;
    }
  }
  // Sum the gradient matrix over tracking window for each pixel.
  BoxFilter(gradients, window_size, gradient_matrix);
}

// Given the three distinct elements of the symmetric 2x2 matrix
//
//                     [gxx gxy]
//                     [gxy gyy],
//
// return the minimum eigenvalue of the matrix.
// Borrowed from Stan Birchfield's KLT implementation.
static float MinEigenValue(float gxx, float gxy, float gyy) {
  return (gxx + gyy - sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy)) / 2.0f;
}

// Compute trackness of every pixel given the gradient matrix.
// This is done as described in the Good Features to Track paper.
static void ComputeTrackness(const Array3Df gradient_matrix,
                             Array3Df *trackness_pointer,
                             double *trackness_mean) {
  Array3Df &trackness = *trackness_pointer;
  trackness.Resize(gradient_matrix.Height(), gradient_matrix.Width());
  *trackness_mean = 0;
  for (int i = 0; i < trackness.Height(); ++i) {
    for (int j = 0; j < trackness.Width(); ++j) {
      double t = MinEigenValue(gradient_matrix(i, j, 0),
                               gradient_matrix(i, j, 1),
                               gradient_matrix(i, j, 2));
      trackness(i, j) = t;
      *trackness_mean += t;
    }
  }
  *trackness_mean /= trackness.Size();
}

static double dist2(const Vec2f &x, const Vec2f &y) {
  double a = x(0) - y(0);
  double b = x(1) - y(1);
  return a * a + b * b;
}

// TODO(keir): Use Stan's neat trick of using a 'punch-out' array to detect
// too-closes features. This is O(n^2)!
static void RemoveTooCloseFeatures(KLTContext::FeatureList *features,
                                   double mindist2) {

  KLTContext::FeatureList::iterator i = features->begin();
  while (i != features->end()) {
    bool i_deleted = false;
    KLTContext::FeatureList::iterator j = i;
    ++j;
    while (j != features->end() && !i_deleted) {
      if (dist2((*i)->position, (*j)->position) < mindist2) {
        KLTContext::FeatureList::iterator to_delete;
        if ((*i)->trackness < (*j)->trackness) {
          to_delete = i;
          ++i;
          i_deleted = true;
        } else {
          to_delete = j;
          ++j;
        }
        delete *to_delete;
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

void KLTContext::DetectGoodFeatures(const Array3Df &image_and_gradients,
                                    FeatureList *features) {
  Array3Df gradient_matrix;
  ComputeGradientMatrix(image_and_gradients, WindowSize(), &gradient_matrix);

  Array3Df trackness;
  double trackness_mean;
  ComputeTrackness(gradient_matrix, &trackness, &trackness_mean);
  min_trackness_ = trackness_mean;

  FindLocalMaxima(trackness, min_trackness_, features);

  RemoveTooCloseFeatures(features, min_feature_dist_ * min_feature_dist_);
}

// TODO(keir): Restore or delete these functions...
void KLTContext::TrackFeatures(ImagePyramid *pyramid1,
                               const FeatureList &features1,
                               ImagePyramid *pyramid2,
                               FeatureList *features2_pointer) {
  FeatureList &features2 = *features2_pointer;

  features2.clear();
  for (FeatureList::const_iterator i = features1.begin();
       i != features1.end(); ++i) {
    KLTPointFeature *tracked_feature = new KLTPointFeature;
    TrackFeature(pyramid1, **i, pyramid2, tracked_feature);
    features2.push_back(tracked_feature);
  }
}

void KLTContext::TrackFeature(ImagePyramid *pyramid1,
                              const KLTPointFeature &feature1,
                              ImagePyramid *pyramid2,
                              KLTPointFeature *feature2_pointer) {
  const int highest_level = pyramid1->NumLevels() - 1;

  Vec2 position1, position2;
  position2(0) = feature1.position(0) / pow(2., highest_level + 1);
  position2(1) = feature1.position(1) / pow(2., highest_level + 1);

  for (int i = highest_level; i >= 0; --i) {
    position1(0) = feature1.position(0) / pow(2., i);
    position1(1) = feature1.position(1) / pow(2., i);
    position2(0) *= 2;
    position2(1) *= 2;

    TrackFeatureOneLevel(pyramid1->Level(i),
                         position1,
                         pyramid2->Level(i),
                         &position2);
  }
  feature2_pointer->position = position2;
}

// Compute the gradient matrix noted by Z and the error vector e.
// See Good Features to Track.
static void ComputeTrackingEquation(const Array3Df &image_and_gradient1,
                                    const Array3Df &image_and_gradient2,
                                    const Vec2 &position1,
                                    const Vec2 &position2,
                                    int half_width,
                                    float *gxx,
                                    float *gxy,
                                    float *gyy,
                                    float *ex,
                                    float *ey) {
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
      float I = SampleLinear(image_and_gradient1, y1, x1, 0);
      float J = SampleLinear(image_and_gradient2, y2, x2, 0);
      float gx = SampleLinear(image_and_gradient2, y2, x2, 1);
      float gy = SampleLinear(image_and_gradient2, y2, x2, 2);
      *gxx += gx * gx;
      *gxy += gx * gy;
      *gyy += gy * gy;
      *ex += (I - J) * gx;
      *ey += (I - J) * gy;
    }
  }
}

// Solve the tracking equation
//
//   [gxx gxy] [dx] = [ex]
//   [gxy gyy] [dy] = [ey]
//
// for dx and dy.  Borrowed from Stan Birchfield's KLT implementation.
static bool SolveTrackingEquation(float gxx, float gxy, float gyy,
                                  float ex, float ey,
                                  float min_determinant,
                                  float *dx, float *dy) {
  float det = gxx * gyy - gxy * gxy;
//  printf("det=%f, min_det=%f, gxx=%f, gxy=%f, gyy=%f\n", det, min_determinant,
//      gxx, gxy, gyy);
  if (det < min_determinant) {
    *dx = 0;
    *dy = 0;
    return false;
  }
  *dx = (gyy * ex - gxy * ey) / det;
  *dy = (gxx * ey - gxy * ex) / det;
  return true;
}

void KLTContext::TrackFeatureOneLevel(const Array3Df &image_and_gradient1,
                                      const Vec2 &position1,
                                      const Array3Df &image_and_gradient2,
                                      Vec2 *position2_pointer) {
  Vec2 &position2 = *position2_pointer;

  int i;
  float dx=0, dy=0;
  max_iterations_ = 30;
  printf("disps = array([\n");
  for (i = 0; i < max_iterations_; ++i) {
    // Compute gradient matrix and error vector.
    float gxx, gxy, gyy, ex, ey;
    ComputeTrackingEquation(image_and_gradient1, image_and_gradient2,
                            position1, position2,
                            HalfWindowSize(),
                            &gxx, &gxy, &gyy, &ex, &ey);
    // Solve the linear system for deltad.
    if (!SolveTrackingEquation(gxx, gxy, gyy, ex, ey, min_determinant_,
                               &dx, &dy)) {
      // TODO(keir): drop feature.
      printf("dropped!\n");
    }

    // shrink the update
    dx *= 0.5;
    dy *= 0.5;

    // Update feature2 position.
    position2(0) += dx;
    position2(1) += dy;

    printf("  [%10f, %10f, %10f, %10f],\n", dx, dy, position2(0), position2(1));

    // TODO(keir): Handle other tracking failure conditions and pass the
    // reasons out to the caller. For example, for pyramid tracking a failure
    // at a coarse level suggests trying again at a finer level.
    if (Square(dx) + Square(dy) < min_update_distance2_) {
      printf("# distance too small: %f, %f\n", dx, dy);
      break;
    }
//    printf("dx=%f, dy=%f\n", dx, dy);
  }
  printf("])\n");
  if (i == max_iterations_) {
    printf("hit max iters. dx=%f, dy=%f\n", dx, dy);
  }
}


void KLTContext::DrawFeatureList(const FeatureList &features,
                                 const Vec3 &color,
                                 FloatImage *image) const {
  for (FeatureList::const_iterator i = features.begin();
       i != features.end(); ++i) {
    DrawFeature(**i, color, image);
  }
}

void KLTContext::DrawFeature(const KLTPointFeature &feature,
                             const Vec3 &color,
                             FloatImage *image) const {
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
