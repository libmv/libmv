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
#include "libmv/correspondence/klt.h"
#include "testing/testing.h"

using std::vector;
using namespace libmv;

namespace {

TEST(KltContext, DetectGoodFeaturesSimple) {
  FloatImage image(51, 51);
  image.Fill(0);
  image(25, 25) = 1.f;
  ImagePyramid pyramid;
  pyramid.Init(image, 3);

  KltContext klt;
  KltContext::FeatureList features;
  klt.DetectGoodFeatures(pyramid, &features);

  EXPECT_EQ(features.size(), unsigned(1));
  EXPECT_EQ(features.back().position(0), 25);
  EXPECT_EQ(features.back().position(1), 25);
}

TEST(KltContext, TrackFeatureOneLevel) {
  FloatImage image1(51, 51);
  image1.Fill(0);
  image1(25, 25) = 1.0f;

  FloatImage image2;
  image2.ResizeLike(image1);
  image2.Fill(0);
  int d = 2;
  for (int i = d; i < image2.Height(); ++i) {
    for (int j = d; j < image2.Width(); ++j) {
      image2(i,j) = image1(i - d, j - d);
    }
  }
  FloatImage image1blur, image2blur, image2_gx, image2_gy;
  ConvolveGaussian(image1, 0.9, &image1blur);
  ConvolveGaussian(image2, 0.9, &image2blur);
  ImageDerivatives(image2, 0.9, &image2_gx, &image2_gy);


  KltContext klt;
  Vec2 position1, position2;
  position1 = 25, 25;
  position2 = 25, 25;
  klt.TrackFeatureOneLevel(image1blur, position1, image2blur,
                           image2_gx, image2_gy, &position2);

  EXPECT_NEAR(position2(0), 25 + d, 0.01);
  EXPECT_NEAR(position2(1), 25 + d, 0.01);
}

TEST(KltContext, TrackFeature) {
  FloatImage image1(51, 51);
  image1.Fill(0);
  image1(25, 25) = 1.0f;

  FloatImage image2;
  image2.ResizeLike(image1);
  image2.Fill(0);
  int d = 4;
  for (int i = d; i < image2.Height(); ++i) {
    for (int j = d; j < image2.Width(); ++j) {
      image2(i,j) = image1(i - d, j - d);
    }
  }
  ImagePyramid pyramid1(image1, 2);
  ImagePyramid pyramid2(image2, 2);

  KltContext klt;
  KltContext::Feature feature1, feature2;
  feature1.position = 25, 25;
  feature2.position = 25, 25;
  klt.TrackFeature(pyramid1, feature1,
                   pyramid2, &feature2);

  EXPECT_NEAR(feature2.position(0), 25 + d, 0.01);
  EXPECT_NEAR(feature2.position(1), 25 + d, 0.01);
}

TEST(KltContext, DetectGoodFeaturesLenna) {
  ByteImage byte_image;
  FloatImage image1;
  string lenna_filename = string(THIS_SOURCE_DIR) + "/klt_test/Lenna.pgm";
  EXPECT_NE(0, ReadPnm(lenna_filename.c_str(), &byte_image));
  ConvertByteImageToFloatImage(byte_image, &image1);

  ImagePyramid pyramid1(image1, 3);

  KltContext klt;
  KltContext::FeatureList features;
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
    if ( sqr(dx-2) + sqr(dy-2) < sqr(1e-2) ) {
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


}  // namespace
