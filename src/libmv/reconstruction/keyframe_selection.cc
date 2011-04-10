// Copyright (c) 2010, 2011 libmv authors.
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

#include <algorithm>
#include <map>

#include "libmv/logging/logging.h"
#include "libmv/multiview/conditioning.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/robust_homography.h"
#include "libmv/correspondence/matches.h"
#include "libmv/reconstruction/keyframe_selection.h"

namespace libmv {

void SelectKeyframesBasedOnMatchesNumber(const Matches &matches, 
                                         vector<Matches::ImageID> *keyframes,
                                         float min_matches_pc,
                                         int min_num_matches) {  
  keyframes->reserve(matches.NumImages());
  std::set<Matches::ImageID>::const_iterator image_iter =
    matches.get_images().begin();
  Matches::ImageID prev_keyframe = *image_iter;
  Matches::ImageID prev_frame_good = *image_iter;
  // The first frame is selected as a keyframe
  keyframes->push_back(prev_keyframe);
  image_iter++;
  int num_features_to_keep = min_matches_pc * 
                             matches.NumFeatureImage(prev_keyframe);
  num_features_to_keep = std::max(num_features_to_keep, min_num_matches); 
  VLOG(3) << "# Features to keep: " << num_features_to_keep<< std::endl;
  vector<Matches::ImageID> two_images;
  two_images.push_back(prev_keyframe);
  for (;image_iter != matches.get_images().end(); ++image_iter) {
    two_images.push_back(*image_iter);
    // TODO(julien) make a new function that just count the common tracks!
    vector<Matches::TrackID> tracks;
    TracksInAllImages(matches, two_images, &tracks);
    VLOG(3) << "Shared tracks: " << tracks.size() << std::endl;
    // If the current frame share not enough common matches with the previous 
    // keyframe then we select the previous frame 
    // i.e. the one that has enough common matches
    if (tracks.size() < num_features_to_keep && 
        prev_frame_good != prev_keyframe) {
      VLOG(2) << "Keyframe Detected!" << std::endl;
      keyframes->push_back(prev_frame_good);
      prev_keyframe = prev_frame_good;
      num_features_to_keep = min_matches_pc * 
                             matches.NumFeatureImage(prev_keyframe);
      num_features_to_keep = std::max(num_features_to_keep, min_num_matches);
      VLOG(3) << "# Features to keep: " << num_features_to_keep<< std::endl;
      two_images.clear();
      two_images.push_back(prev_keyframe);
    } else {
      prev_frame_good = *image_iter;
      two_images.pop_back();
    }
  }
}

/*
 * Not yet finished 
 * TODO(julien) It really doesn't work: Finish this function!
 * */
void SelectKeyframesBasedOnFAndH(
   const Matches &matches, 
   vector<Matches::ImageID> *keyframes) {
  keyframes->reserve(matches.NumImages());
  Mat m;
  Mat3 F, H;
  double f_err, max_error_f = 1;
  double h_err, max_error_h = 1;
  double outliers_prob = 1e-2;
  uint ni, ne = 0;
  vector<Mat> xs2;
  vector<int> inliers;
  std::set<Matches::ImageID>::const_iterator image_iter =
    matches.get_images().begin();
  std::set<Matches::ImageID>::const_iterator prev_image_iter = image_iter;
  ni = matches.NumFeatureImage(*image_iter);
  image_iter++;
  for (;image_iter != matches.get_images().end(); ++image_iter) {
    TwoViewPointMatchMatrices(matches, *prev_image_iter, *image_iter, &xs2);
    if (xs2[0].cols() >= 7) {
      h_err = Homography2DFromCorrespondences4PointRobust(xs2[0], xs2[1], 
                                                          max_error_h, 
                                                          &H, &inliers, 
                                                          outliers_prob);
      h_err /= inliers.size();
      f_err = FundamentalFromCorrespondences7PointRobust(xs2[0], xs2[1], 
                                                         max_error_f, 
                                                         &F, &inliers, 
                                                         outliers_prob);
      f_err /= inliers.size();
      VLOG(1) << "H error:" << h_err << "px" << std::endl;
      VLOG(1) << "F error:" << f_err << "px" << std::endl;
      VLOG(1) << "ni:" << ni << " ne:" << ne << std::endl;
      // TODO(julien) no sure the ni and ne are the good ones.
      // read Pollefeys'03 and Torr98
      ne = inliers.size();
      if (f_err < h_err && ni >= 0.9 * ne) {
        VLOG(1) << "Keyframe detected: " << *image_iter << std::endl;
        keyframes->push_back(*image_iter);
        prev_image_iter = image_iter;
        ni = ne;
      }
    } else {
      prev_image_iter = image_iter;
      ni = ne;
    }
  }
}

} // namespace libmv
