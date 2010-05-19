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

#include "libmv/detector/orientation_detector.h"
#include "libmv/detector/detector.h"
#include "libmv/image/image.h"
#include "testing/testing.h"

namespace libmv {
namespace detector {
namespace {


TEST(OrientationDetector, CoterminalTest) {

  // Assert that Coterminal angle return angle between [0,2PI]

  // Test angle that are multiple of 60 degree.
  // 1. 60 must return 60.
  EXPECT_NEAR(getCoterminalAngle( M_PI/3.0), M_PI/3.0, 1e-5);
  // 2. -300 degree must return 60.
  EXPECT_NEAR(getCoterminalAngle( -(M_PI*2 - M_PI/3.0)), M_PI/3.0, 1e-5);
  // 3. 780 degree must return 60.
  EXPECT_NEAR(getCoterminalAngle( M_PI*2*2 + M_PI/3.0), M_PI/3.0, 1e-5);
}

TEST(OrientationDetector, FastRotEstimatorDegree0)  {
  Array3Du test(7,7);
  test.fill(0);
  //Fill the upper part with 255
  for(int j = 0; j < 3; ++j)  {
    for(int i = 0; i < 7; ++i)  {
      test(j,i) = 255;
    }
  }
  // Build a feature along the separation
  PointFeature ptFeat(3,3);
  vector<Feature *> vec_Feature;
  vec_Feature.push_back(&ptFeat);

  fastRotationEstimation(test, vec_Feature);
  // Angle must be 0 or PI
  EXPECT_NEAR( ptFeat.orientation, M_PI, 1e-5);
}

TEST(OrientationDetector, FastRotEstimatorDegree90)  {
  Array3Du test(7,7);
  test.fill(0);
  //Fill the left part with 255
  for(int j = 0; j < 7; ++j)  {
    for(int i = 0; i < 3; ++i)  {
      test(j,i) = 255;
    }
  }
  // Build a feature along the separation
  PointFeature ptFeat(3,3);
  vector<Feature *> vec_Feature;
  vec_Feature.push_back(&ptFeat);

  fastRotationEstimation(test, vec_Feature);
  // Angle must be PI/2 or 3PI/2
  EXPECT_NEAR( ptFeat.orientation, M_PI/2.0, 1e-5);
}

TEST(OrientationDetector, FastRotEstimatorDegree45)  {
  Array3Du test(7,7);
  test.fill(0);
  //Fill the diagonal upper part with 255
  for(int j = 0; j < 7; ++j)  {
    for(int i = 0 + j; i < 7; ++i)  {
      test(j,i) = 255;
    }
  }
  // Build a feature along the separation
  PointFeature ptFeat(3,3);
  vector<Feature *> vec_Feature;
  vec_Feature.push_back(&ptFeat);

  fastRotationEstimation(test, vec_Feature);
  // Angle must be PI/4 or 5PI/4
  EXPECT_NEAR( ptFeat.orientation, (5.0*M_PI)/4.0, 1e-5);
}

TEST(GradientOrientationDetector, GradientRotEstimatorUpperBlank)  {
  Array3Du test(7,7);
  test.fill(0);
  //Fill the upper part with 255
  for(int j = 0; j < 3; ++j)  {
    for(int i = 0; i < 7; ++i)  {
      test(j,i) = 255;
    }
  }
  // Build a feature along the separation
  PointFeature ptFeat(3,3);
  ptFeat.scale = 1.0;
  vector<Feature *> vec_Feature;
  vec_Feature.push_back(&ptFeat);

  gradientBoxesRotationEstimation(test, vec_Feature);
  // Angle must be PI/2 or PI+PI/2
  EXPECT_NEAR( ptFeat.orientation, M_PI+M_PI/2, 1e-5);
}

TEST(GradientOrientationDetector, GradientRotEstimatorLeftBlank)  {
  Array3Du test(7,7);
  test.fill(0);
  //Fill the left part with 255
  for(int j = 0; j < 7; ++j)  {
    for(int i = 0; i < 3; ++i)  {
      test(j,i) = 255;
    }
  }
  // Build a feature along the separation
  PointFeature ptFeat(3,3);
  ptFeat.scale = 1.0;
  vector<Feature *> vec_Feature;
  vec_Feature.push_back(&ptFeat);

  gradientBoxesRotationEstimation(test, vec_Feature);
  // Angle must be PI or -PI
  EXPECT_NEAR( ptFeat.orientation, M_PI, 1e-5);
}

TEST(GradientOrientationDetector, GradientRotEstimatorDegree45)  {
  Array3Du test(7,7);
  test.fill(0);
  //Fill the diagonal upper part with 255
  for(int j=0; j<7; ++j)  {
    for(int i=0+j; i<7; ++i)  {
      test(j,i) = 255;
    }
  }
  // Build a feature along the separation
  PointFeature ptFeat(3,3);
  ptFeat.scale = 1.0;
  vector<Feature *> vec_Feature;
  vec_Feature.push_back(&ptFeat);

  gradientBoxesRotationEstimation(test, vec_Feature);
  // Angle must be -PI/4 or 7PI/4
  EXPECT_NEAR( ptFeat.orientation, 7*M_PI/4.0, 1e-1);
}


} // namespace
} // namespace detector
} // namespace libmv
