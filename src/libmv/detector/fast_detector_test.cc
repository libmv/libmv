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

#include "libmv/detector/fast_detector.h"
#include "libmv/detector/detector.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/image/image.h"
#include "libmv/base/vector.h"
#include "libmv/correspondence/feature.h"
#include "testing/testing.h"

namespace libmv {
namespace {

TEST(FastDetector, DetectsFeatures) {
  // Does it blend? Stay tuned to find out!
}

TEST(FastDetector, Localisation) {
  Array3Du image(20,20);
  image.fill(0);

  // Draw a white corner inside it :

  for(int j=5; j <=15; ++j) {
    for(int i=5; i <=15; ++i) {
      image(j,i) = 255;
    }
  }
  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 20));

  vector<Feature *> features;
  Image im( new Array3Du(image) );
  detector->Detect( im, &features, NULL);

}

}  // namespace
}  // namespace libmv
