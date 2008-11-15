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

#include "libmv/image/image.h"
#include "libmv/image/image_pyramid.h"
#include "libmv/image/convolve.h"
#include "libmv/correspondence/klt.h"
#include "testing/testing.h"

using std::vector;
using namespace libmv;

namespace {

TEST(KLTContext, DetectGoodFeaturesSimple) {
  Array3Df image(51, 51);
  image.Fill(0);
  image(25, 25) = 1.f;

  Array3Df derivatives;
  BlurredImageAndDerivativesChannels(image, 3.0, &derivatives);

  KLTContext klt;
  KLTContext::FeatureList features;
  klt.DetectGoodFeatures(derivatives, &features);

  EXPECT_EQ(features.size(), 1u);
  EXPECT_EQ(features.back()->position(0), 25);
  EXPECT_EQ(features.back()->position(1), 25);
  delete features.back();
}

TEST(KLTContext, TrackFeatureOneLevel) {
  Array3Df image1(51, 51);
  image1.Fill(0);
  Array3Df image2(image1);

  int x0 = 25, y0 = 25;
  int dx = 2, dy = 1;
  image1(y0, x0) = 1.0f;
  image2(y0 + dy, x0 + dx) = 1.0f;

  Array3Df derivatives1, derivatives2;
  BlurredImageAndDerivativesChannels(image1, 0.9, &derivatives1);
  BlurredImageAndDerivativesChannels(image2, 0.9, &derivatives2);

  KLTContext klt;
  Vec2 position1, position2;
  position1 << x0, y0;
  position2 << x0, y0;
  klt.TrackFeatureOneLevel(derivatives1, position1, derivatives2, &position2);

  EXPECT_NEAR(position2(0), x0 + dx, 0.001);
  EXPECT_NEAR(position2(1), y0 + dy, 0.001);
}

// TODO(pau) test TrackFeatures.

TEST(KLTContext, TrackFeature) {
  Array3Df image1(128, 64);
  image1.Fill(0);
  Array3Df image2(128, 64);
  image2.Fill(0);

  int x0 = 32, y0 = 64;
  int dx = 3, dy = 5;
  image1(y0,      x0     ) = 1.0f;
  image2(y0 + dy, x0 + dx) = 1.0f;

  int pyramid_levels = 3;
  ImagePyramid *pyramid1 = MakeImagePyramid(image1, pyramid_levels, 0.9);
  ImagePyramid *pyramid2 = MakeImagePyramid(image2, pyramid_levels, 0.9);

  KLTContext klt;
  KLTPointFeature feature1, feature2;
  feature1.position << x0, y0;
  feature2.position << x0, y0;
  klt.TrackFeature(pyramid1, feature1, pyramid2, &feature2);

  EXPECT_NEAR(feature2.position(0), x0 + dx, 0.001);
  EXPECT_NEAR(feature2.position(1), y0 + dy, 0.001);

  delete pyramid1;
  delete pyramid2;
}

}  // namespace
