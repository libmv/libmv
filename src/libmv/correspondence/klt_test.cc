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
#include "libmv/correspondence/klt.h"
#include "testing/testing.h"

using std::vector;
using libmv::KltContext;
using libmv::FloatImage;
using libmv::ByteImage;

namespace {

/*
TEST(KltContext, DetectGoodFeatures) {
  FloatImage image(200, 100);
  KltContext klt;
  std::vector<KltContext::DetectedFeature> features;
  klt.DetectGoodFeatures(image, &features);
}

*/
TEST(KltContext, DetectGoodFeaturesLenna) {
  ByteImage byte_image;
  FloatImage float_image;
  string lenna_filename = string(THIS_SOURCE_DIR) + "/klt_test/Lenna.pgm";
  EXPECT_NE(0, ReadPgm(lenna_filename.c_str(), &byte_image));
  ConvertByteImageToFloatImage(byte_image, &float_image);

  KltContext klt;
  vector<KltContext::DetectedFeature> features;
  klt.DetectGoodFeatures(float_image, &features);

  printf("found %d features\n", features.size());
}

/*
TEST(KltContext, DetectGoodFeaturesSimple) {
  FloatImage image(51, 51);
  for (int i = 0; i < 51; ++i) {
    for (int j = 0; j < 51; ++j) {
      image(i,j) = 0;
    }
  }

  image(25, 25) = 1.f;

  KltContext klt;
  vector<KltContext::DetectedFeature> features;
  klt.DetectGoodFeatures(image, &features);

  printf("found %d features\n", features.size());
  for (size_t i = 0; i < features.size(); ++i) {
    printf("%d,%d\n", features[i].int_x, features[i].int_y);
  }
}
*/

}  // namespace
