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
#include "libmv/reconstruction/image_selection.h"

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
} // namespace libmv
