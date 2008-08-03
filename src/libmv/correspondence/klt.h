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

#include "libmv/correspondence/feature.h"
#include "libmv/image/image.h"
#include "libmv/image/image_pyramid.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

struct KLTPointFeature : public PointFeature {
  // (x, y) position (not row, column).
  virtual const Vec2f &Point() {
    return position;
  }
  Vec2f position;
  int half_window_size;
  float trackness;
};

class KLTContext {
 public:
  typedef std::list<KLTPointFeature *> FeatureList;

  KLTContext()
      : half_window_size_(3),
        max_iterations_(10),
        min_trackness_(0.1),
        min_feature_dist_(10),
        min_determinant_(1e-6),
        min_update_distance2_(1e-6) {
  }

  void DetectGoodFeatures(const Array3Df &image_and_gradients,
                          FeatureList *features);

  bool TrackFeature(ImagePyramid *pyramid1,
                    const KLTPointFeature &feature1,
                    ImagePyramid *pyramid2,
                    KLTPointFeature *feature2_pointer);

  void TrackFeatures(ImagePyramid *pyramid1,
                     const FeatureList &features1,
                     ImagePyramid *pyramid2,
                     FeatureList *features2_pointer);

  bool TrackFeatureOneLevel(const FloatImage &image_and_gradient1,
                            const Vec2 &position1,
                            const FloatImage &image_and_gradient2,
                            Vec2 *position2_pointer);


  void DrawFeatureList(const FeatureList &features,
                       const Vec3 &color,
                       FloatImage *image) const;
  void DrawFeature(const KLTPointFeature &feature,
                   const Vec3 &color,
                   FloatImage *image) const;

  int HalfWindowSize() { return half_window_size_; }
  int WindowSize() { return 2 * HalfWindowSize() + 1; }

 private:
  int half_window_size_;
  int max_iterations_;
  double min_trackness_;
  double min_feature_dist_;
  double min_determinant_;
  double min_update_distance2_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_KLT_H_
