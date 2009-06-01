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


#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/robust_fundamental.h"
#include "ui/tvr/features.h"


void FindCandidateMatches(const SurfFeatureSet &left,
                          const SurfFeatureSet &right,
                          libmv::Correspondences *correspondences) {
  int max_track_number = 0;
  for (size_t i = 0; i < left.features.size(); ++i) {
    size_t j, k;
    float distance; 
    right.tree.ApproximateNearestNeighborBestBinFirst(left.features[i],
                                                      100, &j, &distance);
    left.tree.ApproximateNearestNeighborBestBinFirst(right.features[j],
                                                     100, &k, &distance);
    // Left image is image 0, right is 1 for now.
    if (i == k) {
      // Both kdtrees matched the same feature, so it is probably a match.
      correspondences->Insert(0, max_track_number, &left.features[i]);
      correspondences->Insert(1, max_track_number, &right.features[j]);
      max_track_number++;
    }
  }
}

//TODO(pau) Once stable, this has to move to libmv.
//TODO(pau) candidate should be const; we need const_iterator in Correspondence.
void ComputeFundamental(libmv::Correspondences &all_matches,
                        libmv::Mat3 *F,
                        libmv::Correspondences *consistent_matches) {
  using namespace libmv;

  // Construct matrices containing the matches.
  int n = all_matches.NumTracks();
  Mat x[2] = {Mat(2,n), Mat(2,n)};
  std::vector<TrackID> track_ids(n);

  int i = 0;
  for (Correspondences::TrackIterator t = all_matches.ScanAllTracks();
       !t.Done(); t.Next()) {
    PointCorrespondences::Iterator it =
        PointCorrespondences(&all_matches).ScanFeaturesForTrack(t.track());
    x[it.image()](0,i) = it.feature()->x();
    x[it.image()](1,i) = it.feature()->y();
    it.Next();
    x[it.image()](0,i) = it.feature()->x();
    x[it.image()](1,i) = it.feature()->y();
    track_ids[i] = t.track();
    i++;
  }
  VLOG(2) << "x1\n" << x[0] << "\nx2\n" << x[1] << "\n";

  // Compute Fundamental matrix and inliers.
  std::vector<int> inliers;
  // TODO(pau) Expose the threshold.
  FundamentalFromCorrespondences8PointRobust(x[0], x[1], 2, F, &inliers);
  VLOG(1) << inliers.size() << " inliers\n";
  if (inliers.size() < 8) {
    return;
  }

  // Build new correspondence graph containing only inliers.
  for (int j = 0; j < inliers.size(); ++j) {
    PointCorrespondences::Iterator it = PointCorrespondences(&all_matches)
        .ScanFeaturesForTrack(track_ids[inliers[j]]);
    consistent_matches->Insert(it.image(), j, it.feature());
    it.Next();
    consistent_matches->Insert(it.image(), j, it.feature());
  }
  
  // Compute Fundamental matrix using all inliers.
  {
    int n = consistent_matches->NumTracks();
    Mat x[2] = {Mat(2,n), Mat(2,n)};
    int i = 0;
    for (Correspondences::TrackIterator t = consistent_matches->ScanAllTracks();
       !t.Done(); t.Next()) {
      PointCorrespondences::Iterator it =
          PointCorrespondences(consistent_matches)
              .ScanFeaturesForTrack(t.track());
      x[it.image()](0,i) = it.feature()->x();
      x[it.image()](1,i) = it.feature()->y();
      it.Next();
      x[it.image()](0,i) = it.feature()->x();
      x[it.image()](1,i) = it.feature()->y();
      i++;
    }
    FundamentalFromCorrespondences8Point(x[0], x[1], F);
    NormalizeFundamental(*F, F);
    
    LOG(INFO) << "F:\n" << *F;
  }
}
