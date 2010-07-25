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


#include "libmv/base/vector.h"
#include "libmv/correspondence/feature.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/orientation_detector.h"
#include "libmv/image/image.h"
#include "third_party/MserDetector/cvMserDetector.h"
#include "libmv/numeric/numeric.h"
#include <Eigen/QR>
#include "libmv/image/image_io.h"

namespace libmv {
namespace detector {

class MserDetector : public Detector {
 public:
  MserDetector(bool bRotationInvariant):bRotationInvariant_(bRotationInvariant) {}
  virtual ~MserDetector() {}

  virtual void Detect(const Image &image,
                      vector<Feature *> *vec_features,
                      DetectorData **data) {

    ByteImage *byte_image = image.AsArray3Du();

    vector<sMserProperties> vec_ellipses;
    detectPoints(*byte_image, vec_ellipses);

    // Build the output Keypoints :
    for (size_t i = 0; i < vec_ellipses.size(); ++i)  {
      const sMserProperties & ellipse = vec_ellipses[i];
      PointFeature *f = new PointFeature(ellipse._x, ellipse._y);
      //Use square approximation since we do not have affine PointFeature.
      f->scale = sqrt(ellipse._l1*ellipse._l2);
      if (bRotationInvariant_)
        f->orientation = ellipse._alpha;
      else
        f->orientation = 0.0f;
      vec_features->push_back(f);
    }

    // MSER doesn't have a corresponding descriptor, so there's no extra data
    // to export.
    if (data) {
      *data = NULL;
    }
  }

 private:
 bool bRotationInvariant_;
};

Detector *CreateMserDetector(bool bRotationInvariant = false)
{
  return new MserDetector(bRotationInvariant);
}

} //namespace detector
} //namespace libmv
