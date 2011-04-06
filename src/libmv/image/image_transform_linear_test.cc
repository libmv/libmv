// Copyright (c) 2011 libmv authors.
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

#define _USE_MATH_DEFINES
#include <math.h>

#include "libmv/image/image_drawing.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_transform_linear.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"

using namespace libmv;

/** 
 * Methods to test:
 * 
 * - ComputeBoundingBox TODO(julien) Add a unit test
 * - RotateImage        TODO(julien) Correct bug that fails the unit test
 * - WarpImage          TODO(julien) Add a unit test
 * - WarpImageBlend     TODO(julien) Add a unit test
 */

// Assert that pixels was drawn at the good place
TEST(ImageTransform, RotateImage90) {

  const int w = 10, h = 10;
  FloatImage image(h,w);
  image.Fill(0);

  // Draw a horizontal line
  //  __________
  //  |         |
  //  |__here___|
  //  |         |
  //  |_________|
  const int y = 5;
  DrawLine(0, y, w-1, y, 255, &image);
  //WriteImage(image, "i.png");

  FloatImage image_rot(h,w);
  image_rot.Fill(0);
  
  bool adapt_img_size = false;
  double angle90 = M_PI/2.0;
  RotateImage(image, angle90, &image_rot, adapt_img_size);
  //WriteImage(image_rot, "r90.png");
  
  // Expect a vertical line
  //  __________
  //  |    h|   |
  //  |    e|   |
  //  |    r|   |
  //  |____e|___|
  for (int i = 0; i < h; ++i){
    EXPECT_EQ(image_rot(i,y), 255);
  }
  
  image_rot.Fill(0);
  angle90 = -M_PI/2.0;
  RotateImage(image, angle90, &image_rot, adapt_img_size);
  WriteImage(image_rot, "r-90.png");

  for (int i = 0; i < h; ++i) {
    EXPECT_EQ(image_rot(i,y), 255);
  }
}

// Assert that pixels was drawn at the good place
TEST(ImageTransform, TranslateImage) {

  const int w = 10, h = 10;
  FloatImage image(h,w);
  image.Fill(0);

  // Draw a diagonal line
  //  _____
  //  |\  |
  //  | \ |
  //  |__\|
  DrawLine(0, 0, w-1, h-1, 255, &image);

  FloatImage image_trans(h,w);
  image_trans.Fill(0);
  
  bool adapt_img_size = false;
  int dx = 2, dy = 3;
  TranslateImage(image, dx, dy, &image_trans, adapt_img_size);
  
  // Expect a translated diagonal line
  int j = 0;
  for (int i = 2; i < w; ++i) {
    j = i-dx+dy;
    if (j < h)
      EXPECT_EQ(image_trans(j,i), 255);
  }
}
// Lines with a given angle +/-45°
// Assert that pixels was drawn at the good place
TEST(ImageTransform, RotateImage45) {

  const int w = 10, h = 10;
  FloatImage image(h,w);
  image.Fill(0);

  // Draw a horizontal line
  //  __________
  //  |         |
  //  |__here___|
  //  |         |
  //  |_________|
  const int y = 5;
  DrawLine(0, y, w-1, y, 255, &image);

  FloatImage image_rot(h,w);
  image_rot.Fill(0);
  
  bool adapt_img_size = false;
  double angle45 = M_PI/4.0;
  RotateImage(image, angle45, &image_rot, adapt_img_size);
  //WriteImage(image_rot, "r45.png");
    
  for (int i = 1; i < w-1; ++i)
    EXPECT_EQ(image_rot(i,i), 255);
}

// Assert that the image size is good
TEST(ImageTransform, RescaleImageTranslation) {

  const int w = 10, h = 10;
  FloatImage image(h,w);
  Vec2u image_size;
  image_size << w, h;
  
  FloatImage image_rs(h,w);
  Mat3 H;
  Mat3 Hreg;
  Vec4i bbox;
  
  int dx = std::rand(), dy = std::rand();
  H << 1, 0, dx,
       0, 1, dy,
       0, 0, 1;
  // TODO(julien) Test with random affine transformations
  ResizeImage(image_size, H, &image_rs, &Hreg, &bbox);
  
  EXPECT_EQ(image_rs.Width(),  w + dx);
  EXPECT_EQ(image_rs.Height(), h + dy);
  EXPECT_EQ(image_rs.Depth(), 1);
  
  EXPECT_EQ(bbox(0), 0);
  EXPECT_EQ(bbox(1), w + dx);
  EXPECT_EQ(bbox(2), 0);
  EXPECT_EQ(bbox(3), h + dy);
  
  EXPECT_MATRIX_EQ(Hreg, H);
}
