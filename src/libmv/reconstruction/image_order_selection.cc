// Copyright (c) 2010 libmv authors.
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

#include "libmv/multiview/conditioning.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/robust_homography.h"
#include "libmv/reconstruction/image_order_selection.h"

namespace libmv {

void FillPairwiseMatchesMatrix(const Matches &matches, 
                               Mat *m) {
  m->resize(matches.NumImages(),
            matches.NumImages());
  m->setZero();
  std::set<Matches::ImageID>::const_iterator image_iter1 =
    matches.get_images().begin();
  std::set<Matches::ImageID>::const_iterator image_iter2;
  vector<Mat> xs2;
  for (;image_iter1 != matches.get_images().end(); ++image_iter1) {
    image_iter2 = image_iter1;
    image_iter2++;
    for (;image_iter2 != matches.get_images().end(); ++image_iter2) {
      TwoViewPointMatchMatrices(matches, *image_iter1, *image_iter2, &xs2);
      (*m)(*image_iter1, *image_iter2) = xs2[0].cols();
    }
  }
}

void FillPairwiseMatchesHomographyMatrix(const Matches &matches, 
                                         Mat *m) {
  m->resize(matches.NumImages(),
            matches.NumImages());
  m->setZero();
  Mat3 H;
  vector<int> inliers;
  double max_error_h = 1;
  std::set<Matches::ImageID>::const_iterator image_iter1 =
    matches.get_images().begin();
  std::set<Matches::ImageID>::const_iterator image_iter2;
  vector<Mat> xs2;
  for (;image_iter1 != matches.get_images().end(); ++image_iter1) {
    image_iter2 = image_iter1;
    image_iter2++;
    for (;image_iter2 != matches.get_images().end(); ++image_iter2) {
      TwoViewPointMatchMatrices(matches, *image_iter1, *image_iter2, &xs2);
      (*m)(*image_iter1, *image_iter2) = xs2[0].cols();
      if (xs2[0].cols() >= 4) {
        HomographyFromCorrespondences4PointRobust(xs2[0], xs2[1], 
                                                  max_error_h, 
                                                  &H, &inliers, 1e-2);
        // TODO(julien) Put this in a function
        Vec3 p1;
        Vec2 e;
        vector<double> all_errors;
        all_errors.reserve(inliers.size());
        for (int i = 0; i < inliers.size(); ++i) {
          EuclideanToHomogeneous(xs2[0].col(inliers[i]), &p1);
          p1 = H * p1;
          HomogeneousToEuclidean(p1, &e);
          e -= xs2[0].col(inliers[i]);
          all_errors.push_back(e.norm());
        }
        std::sort(all_errors.begin(), all_errors.end());
        VLOG(1) << "H median:" << all_errors[round(inliers.size()/2)] 
                << "px.\n";
        (*m)(*image_iter1, *image_iter2) *= all_errors[round(inliers.size()/2)];
      }
    }
  }
}

bool AddIndex(int id, vector<uint> *id_ordered) {
  for (uint i = 0; i < id_ordered->size() ;++i) {
    if ((*id_ordered)[i] == id) {
      return false;
    }
  }
  id_ordered->push_back(id);
  return true;
}

void RecursivePairwiseHighScoresSearch(Mat &m, 
                                       const Vec2i seed, 
                                       vector<uint> *id_ordered) {   
  double val_c, val_r;
  Vec2i max_c, max_r;
  int nothing;
  // Set to zero (to avoid to get the same couple)
  m(seed[0], seed[1]) = 0;
  
  // Find the best score for the col
  val_c = m.col(seed[1]).maxCoeff(&max_c[0], &nothing);
  max_c[1] = seed[1];
  // Find the best score for the row
  val_r = m.row(seed[0]).maxCoeff(&nothing, &max_r[1]);
  max_r[0] = seed[0];
    
  if (val_c > 0) 
    m(max_c[0], max_c[1]) = 0;
  if (val_r > 0) 
    m(max_r[0], max_r[1]) = 0;
  
  if (val_c < val_r) {
    if (val_r > 0) {
      AddIndex(max_r[1], id_ordered);
      RecursivePairwiseHighScoresSearch(m, max_r, id_ordered);
    }
    if (val_c > 0) {
      AddIndex(max_c[0], id_ordered);
      RecursivePairwiseHighScoresSearch(m, max_c, id_ordered);
    }
  } else {
    if (val_c > 0) {
      AddIndex(max_c[0], id_ordered);
      RecursivePairwiseHighScoresSearch(m, max_c, id_ordered);
    }
    if (val_r > 0){
      AddIndex(max_r[1], id_ordered);
      RecursivePairwiseHighScoresSearch(m, max_r, id_ordered);
    }
  }
}

void RecoverOrderFromPairwiseHighScores(
   const Matches &matches,
   Mat &m, 
   std::list<vector<Matches::ImageID> > *connected_graph_list) {
  //connected_graph_list->clear();
  std::map<uint, Matches::ImageID> map_img_ids;
  std::set<Matches::ImageID>::const_iterator image_iter1 =
    matches.get_images().begin();
  uint i_img = 0;
  for (;image_iter1 != matches.get_images().end(); ++image_iter1, ++i_img) {
    map_img_ids[i_img] = *image_iter1;
  }
  
  Vec2i max;
  double val = 1;
  while (val > 0) {
    // Find the global best score
    val = m.maxCoeff(&max[0], &max[1]);
    //From this seed, find the second best score in the same col/row
    if (val > 0) {
      vector<uint> id_ordered;
      id_ordered.push_back(max[0]);
      id_ordered.push_back(max[1]);
      RecursivePairwiseHighScoresSearch(m, max, &id_ordered);      
      vector<Matches::ImageID> v_ids(id_ordered.size());
      for (i_img = 0; i_img < id_ordered.size(); ++i_img) {
        v_ids[i_img] = (map_img_ids[id_ordered[i_img]]);
      }
      connected_graph_list->push_back(v_ids);
    }
  }
}

void SelectEfficientImageOrder(
    const Matches &matches, 
    std::list<vector<Matches::ImageID> >*images_list) {
  Mat m;
  FillPairwiseMatchesHomographyMatrix(matches, &m);
  VLOG(1) << " M = " << m <<"\n";
  RecoverOrderFromPairwiseHighScores(matches, m, images_list);
}

void SelectKeyframes(
    const Matches &matches, 
    std::list<vector<Matches::ImageID> >*keyframes_list) {
  Mat m;
  Mat3 F, H;
  double f_err, max_error_f = 1;
  double h_err, max_error_h = 1;
  double outliers_prob = 1e-2;
  uint ni, ne = 0;
  vector<Mat> xs2;
  vector<int> inliers;
  vector<Matches::ImageID> keyframes;
  std::set<Matches::ImageID>::const_iterator image_iter =
    matches.get_images().begin();
  std::set<Matches::ImageID>::const_iterator prev_image_iter = image_iter;
  ni = matches.NumFeatureImage(*image_iter);
  image_iter++;
  for (;image_iter != matches.get_images().end(); ++image_iter) {
    TwoViewPointMatchMatrices(matches, *prev_image_iter, *image_iter, &xs2);
    if (xs2[0].cols() >= 7) {
      h_err = HomographyFromCorrespondences4PointRobust(xs2[0], xs2[1], 
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
        keyframes.push_back(*image_iter);
        prev_image_iter = image_iter;
        ni = ne;
      }
    } else {
      VLOG(1) << "[Warning] Tracking lost!" << std::endl;
      keyframes_list->push_back(keyframes);
      keyframes.clear();
      prev_image_iter = image_iter;
      ni = ne;
    }
  }
  keyframes_list->push_back(keyframes);
}
} // namespace libmv
