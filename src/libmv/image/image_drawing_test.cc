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

#include "libmv/image/image.h"
#include "libmv/image/image_drawing.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"

using namespace libmv;

namespace {

// Horizontal / Vertical scanlines
// Assert that pixels was drawn at the good place
TEST(ImageDrawing, Scanlines) {

  const int w = 10, h = 10;
  Array3Du image(h,w);
  image.Fill(0);

  // horizontal scanline
  //  __________
  //  |         |
  //  |__here___|
  //  |         |
  //  |_________|
  const int y = 5;
  DrawLine( 0, y, w-1, y, 255, &image);
  for(int i=0; i < w; ++i)
    EXPECT_EQ( image(y,i), 255);

  image.Fill(0);

  // Vertical scanline
  //  __________
  //  |    h|   |
  //  |    e|   |
  //  |    r|   |
  //  |____e|___|
  const int x = 5;
  DrawLine( x, 0, x, h-1, 255, &image);
  for (int i = 0; i < h; ++i)
    EXPECT_EQ(image(i,y), 255);
}

// Lines with a given angle +/-45°
// Assert that pixels was drawn at the good place
TEST(ImageDrawing, Lines45) {

  const int w = 10, h = 10;
  Array3Du image(h,w);
  image.Fill(0);

  //  _____
  //  |\  |
  //  | \ |
  //  |__\|

  DrawLine(0, 0, w-1, h-1, 255, &image);
  for (int i = 0; i < w; ++i)
    EXPECT_EQ(image(i,i), 255);

  image.Fill(0);

  //  _____
  //  |  / |
  //  | /  |
  //  |/___|_
  DrawLine(0, h-1, w-1, 0, 255, &image);
  for (int i = 0; i < h; ++i)
    EXPECT_EQ(image(h-1-i,i), 255);
}

// Draw a circle in an image and assert that all the points are
// at a distance equal to the radius.
TEST(ImageDrawing, Circle) {

  Array3Du image(10,10);
  image.Fill(0);

  const int radius = 3;
  const int x = 5, y = 5;

  DrawCircle(x, y, radius, (unsigned char)255, &image);

  // Distance checking :
  for ( int j = 0; j < image.Height(); ++j)
  for ( int i = 0; i < image.Width(); ++i) {
    if (image(j, i) == 255)  {
      const float distance =  sqrt((float)((j-y)*(j-y) + (i-x)*(i-x)));
      EXPECT_NEAR(radius, distance, 1.0f);
      // Due to discretisation we cannot expect better precision
    }
  }
}

}
