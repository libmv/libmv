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

#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/detector.h"
#include "libmv/image/image.h"
#include "testing/testing.h"

// Fast 16 ring.
static const int indX[16] = {3,3,2,1,0,-1,-2,-3,-3,-3,-2,-1,0,1,2,3};
static const int indY[16] = {0,1,2,3,3,3,2,1,0,-1,-2,-3,-3,-3,-2,-1};

namespace libmv {
namespace detector {
namespace {

void DrawRing(int x, int y, Array3Du *image) {
  // Draw a white "corner". The "corner" is a white circle in this case. Fast
  // detects arcs around a given point.
  for(int j = 0; j < 16; ++j) {
    (*image)(y + indY[j], x + indX[j]) = 255;
  }
}

TEST(FastDetector, Localisation) {
  Array3Du image(20,20);
  image.fill(0);

  int x = 15, y = 10;
  DrawRing(x, y, &image);

  scoped_ptr<Detector> detector(CreateFastDetector(9, 20));

  vector<Feature *> features;
  Image im(new Array3Du(image));

  detector->Detect(im, &features, NULL);

  ASSERT_EQ(1, features.size());
  PointFeature pt = *((PointFeature*)(features[0]));
  ASSERT_EQ(x, pt.x());
  ASSERT_EQ(y, pt.y());

  DeleteElements(&features);
}

TEST(FastDetector, Localisation2) {
  Array3Du image(20,20);
  image.fill(0);

  // Create two fast circles that intersect and assert that 4 points are found.
  int x1 = 15, y1 = 15;
  int x2 = 12, y2 = 12;
  DrawRing(x1, y1, &image);
  DrawRing(x2, y2, &image);

  scoped_ptr<Detector> detector(CreateFastDetector(9, 20));

  vector<Feature *> features;
  Image im( new Array3Du(image) );
  detector->Detect( im, &features, NULL);

  ASSERT_EQ(4, features.size());

  ASSERT_EQ(x2, ((PointFeature*)(features[0]))->x());
  ASSERT_EQ(y2, ((PointFeature*)(features[0]))->y());

  ASSERT_EQ(x1, ((PointFeature*)(features[1]))->x());
  ASSERT_EQ(y2, ((PointFeature*)(features[1]))->y());

  ASSERT_EQ(x2, ((PointFeature*)(features[2]))->x());
  ASSERT_EQ(y1, ((PointFeature*)(features[2]))->y());

  ASSERT_EQ(x1, ((PointFeature*)(features[3]))->x());
  ASSERT_EQ(y1, ((PointFeature*)(features[3]))->y());

  DeleteElements(&features);
}

}  // namespace
}  // namespace detector
}  // namespace libmv
