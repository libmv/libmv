// Copyright (c) 2011 libmv authors.
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

#ifndef LIBMV_CORRESPONDENCE_PLANAR_TRACKER_H_
#define LIBMV_CORRESPONDENCE_PLANAR_TRACKER_H_

#include "libmv/correspondence/tracker.h"

namespace libmv {
namespace tracker {

// This class robustly tracks points that are in a plan.
class PlanarTracker : public Tracker {
 public:
  PlanarTracker(detector::Detector *detector, 
              descriptor::Describer *describer,
              correspondence::ArrayMatcher<float> *matcher) : 
               Tracker(detector, describer, matcher) {
    minimum_number_inliers_ = 4; // at least 4 points are needed
    rms_threshold_inlier_   = 0.3;
  }
  
  virtual ~PlanarTracker() {}
   
  // Tracks new features between two images.
  bool Track(const Image &image1, 
             const Image &image2, 
             FeaturesGraph *new_features_graph,
             bool keep_single_feature = true);
                     
  // Tracks all features in an image.
  bool Track(const Image &image, 
             const FeaturesGraph &known_features_graph, 
             FeaturesGraph *new_features_graph,
             Matches::ImageID *image_id,
             bool keep_single_feature = true); 
             
  void set_rms_threshold_inlier(double threshold) {
    rms_threshold_inlier_ = threshold;
  }
 protected:
  size_t  minimum_number_inliers_;
  // Maxmimum distance (px) to be an inlier
  double  rms_threshold_inlier_;
};

} // namespace tracker
} // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_PLANAR_TRACKER_H_
