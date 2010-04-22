// Copyright (c) 2009 libmv authors.
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


#include "libmv/correspondence/feature.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/orientation_detector.h"
#include "libmv/image/image.h"
#include "libmv/logging/logging.h"
#include "third_party/fast/fast.h"
#include <utility>
#include <cassert>

namespace libmv {
namespace detector {

// Compute fast9 score for a list of keypoint.
// Return a vector of pair (pointer to the keypoint and associated score).
void fast9_scoreMap(const unsigned char* i, // Image byte array.
  int stride, // Step to go to the next line.
  xy* corners, // Corners coords.
  int num_corners, // Number of corners in corners array.
  int b, // Barrier (Threshold to determine darker/lighter)
  vector< std::pair<xy*,int> > * vec_ScorePerCoords // Filled output array.
  )
{
  assert(vec_ScorePerCoords!=NULL);
  // Build fast ring index
	int pixel[16];
  pixel[0] = 0 + stride * 3;
  pixel[1] = 1 + stride * 3;
  pixel[2] = 2 + stride * 2;
  pixel[3] = 3 + stride * 1;
  pixel[4] = 3 + stride * 0;
  pixel[5] = 3 + stride * -1;
  pixel[6] = 2 + stride * -2;
  pixel[7] = 1 + stride * -3;
  pixel[8] = 0 + stride * -3;
  pixel[9] = -1 + stride * -3;
  pixel[10] = -2 + stride * -2;
  pixel[11] = -3 + stride * -1;
  pixel[12] = -3 + stride * 0;
  pixel[13] = -3 + stride * 1;
  pixel[14] = -2 + stride * 2;
  pixel[15] = -1 + stride * 3;

  // Compute score associated to coordinates.
  vec_ScorePerCoords->resize(num_corners);
  for (int n = 0; n < num_corners; ++n) {
    (*vec_ScorePerCoords)[n].first = &corners[n];
    (*vec_ScorePerCoords)[n].second = fast9_corner_score(i + corners[n].y*stride
      + corners[n].x, pixel, b);
  }
}

// Allow to sort the an array of fast keypoint pair.
static bool compareFASTPairScore(const std::pair<xy*,int> & objA,
                            const std::pair<xy*,int> & objB){
  return (objA.second > objB.second);
}

class FastDetectorLimited : public Detector {
 public:
  virtual ~FastDetectorLimited() {}
  FastDetectorLimited(int threshold,
              bool bRotationInvariant, int expectedFeatureNumber)
    : threshold_(threshold),
    bRotationInvariant_(bRotationInvariant),
    expectedFeatureNumber_(expectedFeatureNumber) {}

  virtual void Detect(const Image &image,
                      vector<Feature *> *features,
                      DetectorData **data) {
    int num_corners = 0;
    ByteImage *byte_image = image.AsArray3Du();
    if (byte_image) {
      // Algorithm :
      // a. Detect.
      // b. ComputeScores.
      // c. NonMaxSuppresion.
      // d. KeepNBestFeatures.

      int ret_num_corners = 0;
      xy* corners = fast9_detect(byte_image->Data(),
        byte_image->Width(), byte_image->Height(),
        byte_image->Width(), threshold_, &num_corners);
      int * scores = fast9_score(byte_image->Data(),
        byte_image->Width(), corners, num_corners, threshold_);
      xy* nonmax = nonmax_suppression(corners, scores, num_corners,
        &ret_num_corners);

      vector< std::pair<xy*,int> > ptScores(ret_num_corners);
      fast9_scoreMap(byte_image->Data(),
        byte_image->Width(), nonmax, ret_num_corners, threshold_, & ptScores);

      std::sort(ptScores.begin(), ptScores.end(), compareFASTPairScore);

      free(scores);
      free(corners);

      for (int i = 0; i < std::min(expectedFeatureNumber_,ret_num_corners);
          ++i) {
        PointFeature *f = new PointFeature(ptScores[i].first->x,
                            ptScores[i].first->y);
        f->scale = 1.0;
        f->orientation = 0.0;
        features->push_back(f);
      }
      free( nonmax );

      if (bRotationInvariant_) {
        fastRotationEstimation(*byte_image,*features);
      }
    }
    else  {
      LOG(ERROR) << "Invalid input image type for FastDetectorLimited detector";
    }

    // FAST doesn't have a corresponding descriptor, so there's no extra data
    // to export.
    if (data) {
      *data = NULL;
    }
  }

 private:
  int threshold_; // Threshold called barrier in Fast paper (cf. [1]).
  bool bRotationInvariant_;
  int expectedFeatureNumber_; // Max number of detected feature
};

Detector *CreateFastDetectorLimited(int threshold,
                              bool bRotationInvariant,
                              int expectedFeatureNumber) {
  return new FastDetectorLimited(threshold, bRotationInvariant,
    expectedFeatureNumber);
}

}  // namespace detector
}  // namespace libmv
