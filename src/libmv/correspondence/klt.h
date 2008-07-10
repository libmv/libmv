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
    float trackness;
  };
  typedef std::list<Feature> FeatureList;

  KltContext()
      : window_size_(7),
        min_trackness_(0.1),
        min_feature_dist_(10) {
  }

  void DetectGoodFeatures(const FloatImage &image,
                          FeatureList *features);

  // Compute trackness of every pixel given the gradient matrix.
  // This is done as described in the Good Features to Track paper.
  void ComputeGradientMatrix(const FloatImage &gradient_x,
                             const FloatImage &gradient_y,
                             FloatImage *gxx,
                             FloatImage *gxy,
                             FloatImage *gyy);

  // Compute trackness of every pixel given the gradient matrix.
  void ComputeTrackness(const FloatImage &gradient_xx,
                        const FloatImage &gradient_xy,
                        const FloatImage &gradient_yy,
                        FloatImage *trackness_pointer,
                        double *trackness_mean);

  void FindLocalMaxima(const FloatImage &trackness,
                       FeatureList *features);

  void RemoveTooCloseFeatures(FeatureList *features_pointer);

  void TrackFeature(const ImagePyramid &pyramid1,
                    const Feature &feature1,
                    const ImagePyramid &pyramid2,
                    Feature *feature2_pointer);
  void TrackFeatureOneLevel(const FloatImage &image1,
                            const Feature &feature1,
                            const FloatImage &image2,
                            Feature *feature2_pointer);
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

  int WindowSize() { return window_size_; }

 protected:
  int window_size_;
  double min_trackness_;
  double min_feature_dist_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_KLT_H_
