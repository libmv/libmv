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


#include "libmv/detector/detector.h"
#include "libmv/correspondence/feature.h"
#include "libmv/image/image.h"
#include "third_party/StarDetector/cvStarDetector.h"

namespace libmv {
namespace detector {

class StarDetector : public Detector {
 public:
  StarDetector() {}
  virtual ~StarDetector() {}

  virtual void Detect(const Image &image,
                      vector<Feature *> *features,
                      DetectorData **data) {

    ByteImage *byte_image = image.AsArray3Du();

    FloatImage responses( byte_image->Height(), byte_image->Width(), 1 );
    ShortImage sizes( byte_image->Height(), byte_image->Width(), 1 );

    int iBorder = icvStarDetectorComputeResponses( *byte_image, &responses, &sizes, 45 );
    if( iBorder >= 0 )
        icvStarDetectorSuppressNonmax( responses, sizes, features, iBorder, 5, 25);

    // STAR doesn't have a corresponding descriptor, so there's no extra data
    // to export.
    if (data) {
      *data = NULL;
    }
  }

};

/*  class StarDetector : public Detector {
 public:
  virtual ~StarDetector() {}
  StarDetector() {}

  virtual void Detect(const Image &image,
                      vector<Feature *> *features,
                      DetectorData **data) {

  }

 private:

};*/

Detector *CreateStarDetector()
{
  return new StarDetector;
}

} //namespace detector
} //namespace libmv
