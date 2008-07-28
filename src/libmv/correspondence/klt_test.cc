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

#include <cstdio>

#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
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
  BlurredImageAndDerivativesChannels(image, 3, &derivatives);

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
  image1(25, 25) = 1.0f;

  int d = 2;

  Array3Df image2;
  image2.ResizeLike(image1);
  image2.Fill(0);
  image2(25 + d, 25 + d) = 1.0f;

  Array3Df derivatives1;
  BlurredImageAndDerivativesChannels(image1, 1, &derivatives1);

  Array3Df derivatives2;
  BlurredImageAndDerivativesChannels(image2, 1, &derivatives2);

  KLTContext klt;
  Vec2 position1, position2;
  position1 = 25, 25;
  position2 = 25, 25;
  klt.TrackFeatureOneLevel(derivatives1, position1, derivatives2, &position2);

  EXPECT_NEAR(position2(0), 25 + d, 0.01);
  EXPECT_NEAR(position2(1), 25 + d, 0.01);
}

// TODO(pau) test TrackFeatures.

TEST(KLTContext, TrackFeature) {
  FloatImage image1(64, 64);
  image1.Fill(0);
  image1(32, 32) = 1.0f;

  Array3Df image2;
  image2.ResizeLike(image1);
  image2.Fill(0);
  int d = 4;
  image2(32 + d, 32 + d) = 1.0f;

  ImagePyramid pyramid1(image1, 3, 1.0);
  ImagePyramid pyramid2(image2, 3, 1.0);

  KLTContext klt;
  KLTPointFeature feature1, feature2;
  feature1.position = 32, 32;
  feature2.position = 32, 32;
  klt.TrackFeature(pyramid1, feature1, pyramid2, &feature2);

  EXPECT_NEAR(feature2.position(0), 32 + d, 0.01);
  EXPECT_NEAR(feature2.position(1), 32 + d, 0.01);
}

/*
TEST(KLTContext, DetectGoodFeaturesLenna) {
  ByteImage byte_image;
  FloatImage image1;
  string lenna_filename = string(THIS_SOURCE_DIR) + "/klt_test/Lenna.pgm";
  EXPECT_NE(0, ReadPnm(lenna_filename.c_str(), &byte_image));
  ConvertByteImageToFloatImage(byte_image, &image1);

  ImagePyramid pyramid1(image1, 3);

  KLTContext klt;
  KLTContext::FeatureList features;
  klt.DetectGoodFeatures(pyramid1, &features);

  FloatImage output_image(image1.Height(),image1.Width(),3);
  for (int i = 0; i < image1.Height(); ++i) {
    for (int j = 0; j < image1.Width(); ++j) {
      output_image(i,j,0) = image1(i,j);
      output_image(i,j,1) = image1(i,j);
      output_image(i,j,2) = image1(i,j);
    }
  }

  FloatImage image2;
  image2.ResizeLike(image1);
  int d = 2;
  for (int i = d; i < image2.Height(); ++i) {
    for (int j = d; j < image2.Width(); ++j) {
      image2(i,j) = image1(i - d, j - d);
    }
  }

  ImagePyramid pyramid2;
  pyramid2.Init(image2,3);
  KltContext::FeatureList features2;
  int inliers = 0, outliers = 0;
  for (KltContext::FeatureList::iterator i = features.begin();
       i != features.end(); ++i ) {
    KltContext::Feature &feature1 = *i;
    KltContext::Feature feature2;
    feature2.position = feature1.position;
    klt.TrackFeatureOneLevel(pyramid1.Level(0),
                             feature1.position,
                             pyramid2.Level(0),
                             pyramid2.GradientX(0),
                             pyramid2.GradientY(0),
                             &feature2.position);
    features2.push_back(feature2);

    float dx = feature2.position(0) - feature1.position(0);
    float dy = feature2.position(1) - feature1.position(1);
    if ( Square(dx-2) + Square(dy-2) < Square(1e-2) ) {
      inliers++;
    } else {
      outliers++;
    }
  }
  double inliers_proportion = float(inliers) / (inliers + outliers);
  EXPECT_GE(inliers_proportion, 0.8);

  Vec3 red, green;
  red = 1, 0, 0;
  green = 0, 1, 0;
  klt.DrawFeatureList(features, red, &output_image);
  klt.DrawFeatureList(features2, green, &output_image);
  WritePnm(output_image, "detected_features.ppm");
}
*/

}  // namespace
