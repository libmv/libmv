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
namespace {

TEST(FastDetector, Localisation) {
  Array3Du image(20,20);
  image.fill(0);

  // Draw a white "corner" inside it :
  //  Here for testing purpose the corner is considered as a
  //    white circle. Fast detect arc around a given point.

  //Create a fast circle and assert that it is localized.
  const int x = 15,y = 15;
  for(int j=0; j < 16; ++j) {
    image(y+indY[j],x+indX[j]) = 255;
  }

  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 20));

  vector<Feature *> features;
  Image im( new Array3Du(image) );
  detector->Detect( im, &features, NULL);
  ASSERT_EQ(1, features.size());
  PointFeature pt = *((PointFeature*)(features[0]));
  // Check position
  ASSERT_EQ( x, pt.x());
  ASSERT_EQ( y, pt.y());

  DestroyDynamicDataArray(features);
}

TEST(FastDetector, Localisation2) {
  Array3Du image(20,20);
  image.fill(0);

  // Draw a white "corner" inside it :
  //  Here for testing purpose the corner is considered as a
  //    white circle. Fast detect arc around a given point.

  //Create two fast circle that intersect and assert that 4 points are found.
  const int x = 15,y = 15;
  for(int j=0; j < 16; ++j) {
    image(y+indY[j],x+indX[j]) = 255;
  }
  const int x2 = 12, y2 = 12;
  for(int j=0; j < 16; ++j) {
    image(y2+indY[j],x2+indX[j]) = 255;
  }

  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 20));

  vector<Feature *> features;
  Image im( new Array3Du(image) );
  detector->Detect( im, &features, NULL);
  ASSERT_EQ(4, features.size());
  // Check positions
  ASSERT_EQ( x2, ((PointFeature*)(features[0]))->x());
  ASSERT_EQ( y2, ((PointFeature*)(features[0]))->y());

  ASSERT_EQ( x, ((PointFeature*)(features[1]))->x());
  ASSERT_EQ( y2, ((PointFeature*)(features[1]))->y());

  ASSERT_EQ( x2, ((PointFeature*)(features[2]))->x());
  ASSERT_EQ( y, ((PointFeature*)(features[2]))->y());
  
  ASSERT_EQ( x, ((PointFeature*)(features[3]))->x());
  ASSERT_EQ( y, ((PointFeature*)(features[3]))->y());

  DestroyDynamicDataArray(features);
}

// TODO(pmoulon) implement and test FAST rotation invariance.
TEST(FastDetector, Orientation) {
  Array3Du image(7,7);
  image.fill(50);

  // Draw a white "corner" inside it :
  //  Here for testing purpose the corner is considered as a
  //    white circle. Fast detect arc around a given point.

  //Create two fast circle that intersect and assert that 4 points are found.
  const int x = 3,y = 3;
  for(int j=0; j < 16; ++j) {
    image(y+indY[j],x+indX[j]) = (j<8) ? 255 : 200;
  }

  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 20));

  vector<Feature *> features;
  Image im( new Array3Du(image) );
  detector->Detect( im, &features, NULL);
  ASSERT_EQ(1, features.size());
  // Check positions
  ASSERT_EQ( x, ((PointFeature*)(features[0]))->x());
  ASSERT_EQ( y, ((PointFeature*)(features[0]))->y());

  //fastRotationEstimation(image, features);

  //cout << ((PointFeature*)(features[0]))->orientation/3.14*180.0 << endl;

  DestroyDynamicDataArray(features);

}


}  // namespace
}  // namespace libmv
