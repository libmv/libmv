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

namespace libmv {
namespace detector {

typedef xy* (*FastDetectorCall)(
    const unsigned char *, int, int, int, int, int *);

class FastDetector : public Detector {
 public:
  virtual ~FastDetector() {}
  FastDetector(FastDetectorCall detector, int threshold, int size,
              bool bRotationInvariant)
    : threshold_(threshold), size_(size), detector_(detector),
    bRotationInvariant_(bRotationInvariant) {}

  virtual void Detect(const Image &image,
                      vector<Feature *> *features,
                      DetectorData **data) {
    int num_corners = 0;
    ByteImage *byte_image = image.AsArray3Du();
    if (byte_image) {
      xy* detections = detector_(byte_image->Data(),
          byte_image->Width(), byte_image->Height(), byte_image->Width(),
          threshold_, &num_corners);

      for (int i = 0; i < num_corners; ++i) {
        PointFeature *f = new PointFeature(detections[i].x, detections[i].y);
        f->scale = 1.0;
        f->orientation = 0.0;
        features->push_back(f);
      }
      free( detections );

      if (bRotationInvariant_) {
        fastRotationEstimation(*byte_image,*features);
        //gradientBoxesRotationEstimation(*byte_image,*features);
      }
    }
    else  {
      LOG(ERROR) << "Invalid input image type for FAST detector";
    }

    // FAST doesn't have a corresponding descriptor, so there's no extra data
    // to export.
    if (data) {
      *data = NULL;
    }
  }

 private:
  int threshold_;
  int size_;  // In pixels.
  FastDetectorCall detector_;
  bool bRotationInvariant_;
};

Detector *CreateFastDetector(int size, int threshold,
                              bool bRotationInvariant) {
  FastDetectorCall detector = NULL;
  if (size ==  9) detector =  fast9_detect_nonmax;
  if (size == 10) detector = fast10_detect_nonmax;
  if (size == 11) detector = fast11_detect_nonmax;
  if (size == 12) detector = fast12_detect_nonmax;
  if (!detector) {
    LOG(FATAL) << "Invalid size for FAST detector: " << size;
  }
  return new FastDetector(detector, threshold, size, bRotationInvariant);
}

}  // namespace detector
}  // namespace libmv
