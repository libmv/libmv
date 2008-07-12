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

#ifndef LIBMV_CORRESPONDENCE_KLT_H_
#define LIBMV_CORRESPONDENCE_KLT_H_

#include <cassert>
#include <list>

#include "libmv/image/image.h"
#include "libmv/image/image_pyramid.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

class KltContext {
 public:
  struct Feature {
    Vec2 position;
    int half_window_size;
    float trackness;
  };
  typedef std::list<Feature> FeatureList;

  KltContext()
      : half_window_size_(3),
        min_trackness_(0.1),
        min_feature_dist_(10) {
  }

  void DetectGoodFeatures(const FloatImage &image,
                          FeatureList *features);

  // Compute the gradient matrix noted by Z in Good Features to Track.
  // Z = [gxx gxy; gxy gyy]
  // This function computes the matrix for every pixel.
  void ComputeGradientMatrix(const FloatImage &gradient_x,
                             const FloatImage &gradient_y,
                             FloatImage *gxx,
                             FloatImage *gxy,
                             FloatImage *gyy);

  // Compute trackness of every pixel given the gradient matrix.
  // This is done as described in the Good Features to Track paper.
  void ComputeTrackness(const FloatImage &gxx,
                        const FloatImage &gxy,
                        const FloatImage &gyy,
                        FloatImage *trackness_pointer,
                        double *trackness_mean);

  void FindLocalMaxima(const FloatImage &trackness,
                       FeatureList *features);

  void RemoveTooCloseFeatures(FeatureList *features_pointer);

  void TrackFeature(const ImagePyramid &pyramid1,
                    const Feature &feature1,
                    const ImagePyramid &pyramid2,
                    const ImagePyramid &pyramid2_gx,
                    const ImagePyramid &pyramid2_gy,
                    Feature *feature2_pointer);

  void TrackFeatureOneLevel(const FloatImage &image1,
                            const Feature &feature1,
                            const FloatImage &image2,
                            const FloatImage &image2_gx,
                            const FloatImage &image2_gy,
                            Feature *feature2_pointer);

  // Compute the gradient matrix noted by Z and the error vector e.
  // See Good Features to Track.
  void ComputeTrackingEquation(const FloatImage &image1,
                               const FloatImage &image2,
                               const FloatImage &image2_gx,
                               const FloatImage &image2_gy,
                               const Vec2 &position1,
                               const Vec2 &position2,
                               float *gxx,
                               float *gxy,
                               float *gyy,
                               float *ex,
                               float *ey);
  
  // Solve the tracking equation
  //  [gxx gxy] [dx] = [ex]
  //  [gxy gyy] [dy] = [ey]
  // for dx and dy.
  // Borrowed from Stan Birchfield's KLT implementation.
  static bool SolveTrackingEquation(float gxx, float gxy, float gyy,
                                    float ex, float ey,
                                    float small_determinant_threshold,
                                    float *dx, float *dy);
                                    
  // Given the three distinct elements of the symmetric 2x2 matrix
  //                     [gxx gxy]
  //                     [gxy gyy],
  // return the minimum eigenvalue of the matrix.
  // Borrowed from Stan Birchfield's KLT implementation.
  static float MinEigenValue(float gxx, float gxy, float gyy) {
    return (gxx + gyy - sqrt((gxx - gyy) * (gxx - gyy) + 4 * gxy * gxy)) / 2.0f;
  }
                                    
  void DrawFeatureList(const FeatureList &features,
                       const Vec3 &color,
                       FloatImage *image);
  void DrawFeature(const Feature &feature,
                   const Vec3 &color,
                   FloatImage *image);

  int HalfWindowSize() { return half_window_size_; }
  int WindowSize() { return 2 * HalfWindowSize() + 1; }

 protected:
  int half_window_size_;
  double min_trackness_;
  double min_feature_dist_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_KLT_H_
