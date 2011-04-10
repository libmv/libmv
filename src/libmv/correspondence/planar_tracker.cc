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

#include "planar_tracker.h"
#include "libmv/multiview/robust_homography.h"

using namespace libmv;
using namespace tracker;
 
bool PlanarTracker::Track(const Image &image1,
                        const Image &image2, 
                        FeaturesGraph *new_features_graph,
                        bool keep_single_feature) {
  bool is_track_ok = Tracker::Track(image1,
                                    image2, 
                                    new_features_graph,
                                    keep_single_feature);
  if (!is_track_ok)
    return is_track_ok;
  
  // We remove wrong matches using a homography filtering
  vector<Mat> x;
  vector<Matches::TrackID> tracks;
  vector<Matches::ImageID> image_indices;
  image_indices.push_back(0);
  image_indices.push_back(1);

  PointMatchMatrices(new_features_graph->matches_, 
                     image_indices, 
                     &tracks,
                     &x);
  
  vector<int> inliers;
  Mat3 H;
  Homography2DFromCorrespondences4PointRobust(x[0], x[1], 
                                              rms_threshold_inlier_,
                                              &H, &inliers);

  // We remove correspondences that are not inliers
  size_t max_num_track = new_features_graph->matches_.GetMaxTrackID()+1;
  for (size_t i = 0; i < tracks.size(); ++i) {
    bool is_inlier = false;
    if (inliers.size() > minimum_number_inliers_) {
      for (size_t j = 0; j < inliers.size(); ++j) {
        if (i == inliers[j]) {
          is_inlier = true;
          break;
        }
      }
    }
    if (!is_inlier) {
      const Feature * feature_to_remove =
       new_features_graph->matches_.Get(1, tracks[i]);
      new_features_graph->matches_.Remove(1, tracks[i]);
      if (keep_single_feature) {
        new_features_graph->matches_.Insert(1, max_num_track,
                                            feature_to_remove);
        max_num_track++;
      }
    }
  }
  return is_track_ok;
}

bool PlanarTracker::Track(const Image &image, 
                        const FeaturesGraph &known_features_graph, 
                        FeaturesGraph *new_features_graph,
                        Matches::ImageID *image_id,
                        bool keep_single_feature) {
 
  bool is_track_ok = Tracker::Track(image,
                                    known_features_graph,
                                    new_features_graph,
                                    image_id,
                                    keep_single_feature);
  if (!is_track_ok)
    return is_track_ok;
  
  // TODO(jmichot) Avoid the conversion std::set <-> vector
  // Avoid to compute the homography btw images that does not share tracks
  std::set<Matches::ImageID>::const_iterator iter_image =
   known_features_graph.matches_.get_images().begin();
  const std::set<Matches::TrackID> new_track_set =
   new_features_graph->matches_.get_tracks();
  for (; iter_image != known_features_graph.matches_.get_images().end();
      ++iter_image) {
    if (*image_id != *iter_image) {
      // We remove wrong matches using a homography filtering
      vector<Mat> x(2);
      vector<Matches::TrackID> tracks;

      std::set<Matches::TrackID>::const_iterator new_track_set_iter =
       new_track_set.begin();
      for (; new_track_set_iter != new_track_set.end(); ++new_track_set_iter) {
        const PointFeature *f1 = static_cast<const PointFeature *>(
         known_features_graph.matches_.Get(*iter_image,
                                           *new_track_set_iter));
        const PointFeature *f2 = static_cast<const PointFeature *>(
         new_features_graph->matches_.Get(*image_id, *new_track_set_iter));
        if (f1 && f2) {
          tracks.push_back(*new_track_set_iter);
        }
      }
      x[0].resize(2,tracks.size());
      x[1].resize(2,tracks.size());
      for (size_t i = 0; i < tracks.size(); ++i) {
        const PointFeature *f1 = static_cast<const PointFeature *>(
         known_features_graph.matches_.Get(*iter_image, tracks[i]));
        const PointFeature *f2 = static_cast<const PointFeature *>(
         new_features_graph->matches_.Get(*image_id, tracks[i]));
        if (f1 && f2) {
          x[0](0,i) = f1->x();
          x[0](1,i) = f1->y();          
          x[1](0,i) = f2->x();
          x[1](1,i) = f2->y();
        }
      }
      vector<int> inliers;
      Mat3 H;
      Homography2DFromCorrespondences4PointRobust(x[0], x[1], 
                                                  rms_threshold_inlier_, 
                                                  &H, &inliers);
      LOG(INFO) << "#inliers = "<< inliers.size() << std::endl;
      LOG(INFO) << "#outliers = "<< tracks.size()-inliers.size() << std::endl;
      // We remove correspondences that are not inliers
      size_t max_num_track = 1 +
       std::max(new_features_graph->matches_.GetMaxTrackID(),
                known_features_graph.matches_.GetMaxTrackID());
                
      for (size_t i = 0; i < tracks.size(); ++i) {
        bool is_inlier = false;
        if (inliers.size() > minimum_number_inliers_) {
          for (size_t j = 0; j < inliers.size(); ++j) {
            if (i == inliers[j]) {
              is_inlier = true;
              break;
            }
          }
        }
        if (!is_inlier) {
          const Feature * feature_to_remove =
          new_features_graph->matches_.Get(*image_id, tracks[i]);
          new_features_graph->matches_.Remove(*image_id, tracks[i]);
          if (keep_single_feature) {
            new_features_graph->matches_.Insert(*image_id, 
                                                max_num_track,
                                                feature_to_remove);
            max_num_track++;
          }
        }
      }
    }
  }
  return is_track_ok;
}
