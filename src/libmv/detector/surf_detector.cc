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

#include "libmv/logging/logging.h"
#include "libmv/detector/detector.h"
#include "libmv/correspondence/feature.h"
#include "libmv/image/image.h"
#include "libmv/image/surf.h"

namespace libmv {
namespace detector {

class SurfDetector : public Detector {
 public:
  virtual ~SurfDetector() {}
  SurfDetector(int num_octaves, int num_intervals)
    :num_octaves_(num_octaves), num_intervals_(num_intervals) {}

  virtual void Detect(const Image &image,
                      vector<Feature *> *features,
                      DetectorData **data) {
    int num_corners = 0;
    ByteImage *byte_image = image.AsArray3Du();
    //TODO(pmoulon) Assert that byte_image is valid.

    Matu integral_image;
    IntegralImage(*byte_image, &integral_image);

    libmv::vector<PointFeature> detections;
    MultiscaleDetectFeatures(integral_image, num_octaves_, num_intervals_,
                           &detections);    

    for (int i = 0; i < detections.size(); ++i) {
      PointFeature *f = new PointFeature(detections[i].x(), detections[i].y());
      f->scale = detections[i].scale;
      f->orientation = detections[i].orientation;
      features->push_back(f);
    }   
    
    //data can contain the integral image that can be use for descriptor computation
    if (data) {
      *data = NULL;
    }
  }

 private:
  int num_octaves_;
  int num_intervals_;
};

Detector *CreateSURFDetector(int num_octaves, int num_intervals) {
  return new SurfDetector(num_octaves, num_intervals);
}

}  // namespace detector
}  // namespace libmv
