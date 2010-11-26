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

#include "libmv/multiview/reconstruction_image_order_selection.h"
#include "libmv/multiview/robust_homography.h"

namespace libmv {

void SelectEfficientImageOrder(
    const Matches &matches, 
    std::list<vector<Matches::ImageID> >*connected_graph_list) {
  typedef std::pair<Matches::ImageID, Matches::ImageID> ImagesTypePairs;
  typedef vector<ImagesTypePairs > ImagesPairs;
  double h, score, max_error_h = 1;
  Mat3 H;  
 
  // TODO(julien) Find connected graph and do the rest of every graph
  {
    // Selects the two views that are not too closed but share common tracks
    size_t i = 0;
    double score_max_1 = 0;
    ImagesTypePairs image_pair_max_1;
    vector<Mat2X> xs(matches.NumImages());
    std::set<Matches::ImageID>::const_iterator image_iter1 =
      matches.get_images().begin();
    std::set<Matches::ImageID>::const_iterator image_iter2;
    image_pair_max_1 = ImagesTypePairs(*matches.get_images().begin(),
                                       *(++matches.get_images().begin()));
    vector<Mat> xs2;
    // HACK(julien) we force to keep the first image 
    // TODO(julien) implements a better selection
    //for (;image_iter1 != matches.get_images().end(); ++image_iter1) {
    {
      image_iter2 = image_iter1;
      image_iter2++;
      for (;image_iter2 != matches.get_images().end(); ++image_iter2) {
        TwoViewPointMatchMatrices(matches, *image_iter1, *image_iter2, &xs2);
        if (xs2[0].cols() >= 4) {
          h = HomographyFromCorrespondences4PointRobust(xs2[0], xs2[1], 
                                                        max_error_h, 
                                                        &H, NULL, 1e-2);
          // the score is homography x number of matches
          //TODO(julien) Actually it should be the median of the homography...
          score = h * xs2[0].cols();
          i++;
          VLOG(1)   << "Score["<<i<<"] = " << score <<"\n";
          if (score > score_max_1) {
            score_max_1 = score;
            image_pair_max_1 = ImagesTypePairs(*image_iter1, *image_iter2);
            VLOG(1)   << " max score found !\n";
          }
        }
      }
    }
    vector<Matches::ImageID> v(matches.NumImages());
    if (score_max_1 != 0) {
      v[0] = image_pair_max_1.first;
      v[1] = image_pair_max_1.second;
      i = 2;
    } else {
      i = 0;
    }
    // Fill the rest of images (not ordered)
    // TODO(julien) maybe we can do better than a non ordered list here?
    image_iter1 = matches.get_images().begin();
    for (;image_iter1 != matches.get_images().end(); ++image_iter1) {
      if (score_max_1 == 0 || (*image_iter1 != v[0] && *image_iter1 != v[1])) {
        v[i] = *image_iter1;
        ++i;
      }
    }
    connected_graph_list->push_back(v);
  }
}
} // namespace libmv
