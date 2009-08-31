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


#include "libmv/image/image_converter.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"

using namespace libmv;

TEST(Image_Converter, Rgb2Gray) {

  // Gray value must be kept
  for(double i = 0.0; i < 255.0; i += 1.0)
    EXPECT_NEAR( i , RGB2GRAY(i, i, i), 1e-2);

  // Assert validity of the formula
  // (Conversion function must give a better response for the Green channel)
  // (Human eye sensivity copy)
  // (Green > Red > Blue)
  unsigned char r = RGB2GRAY(255, 0, 0);
  unsigned char g = RGB2GRAY(0, 255, 0);
  unsigned char b = RGB2GRAY(0, 0, 255);
  EXPECT_EQ( true , g > r &&  r > b);
}

TEST(Image_Converter, ImaConvert_RGB2Uchar) {

  const int w = 10, h = 10;
  Array3Du imageGray;
  Array3Du imageColor(h, w, 3);

  imageColor.Fill(127);

  Rgb2Gray(imageColor, &imageGray);

  EXPECT_EQ(1 , imageGray.Depth());
  EXPECT_EQ(w , imageGray.Width());
  EXPECT_EQ(h , imageGray.Height());

  for(int j = 0; j < h; ++j)
  for(int i = 0; i < w; ++i)  {
    EXPECT_EQ(127, imageGray(j,i));
  }
  // Do not test color conversion code since it is yet tested.
}

TEST(Image_Converter, ImaConvert_RGBFloat_2_GrayUchar) {

  const int w = 10, h = 10;
  Array3Du imageGray;
  Array3Df imageColor(h, w, 3);

  imageColor.Fill(127);

  Rgb2Gray(imageColor, &imageGray);

  EXPECT_EQ(1 , imageGray.Depth());
  EXPECT_EQ(w , imageGray.Width());
  EXPECT_EQ(h , imageGray.Height());

  for(int j = 0; j < h; ++j)
  for(int i = 0; i < w; ++i)  {
    EXPECT_EQ(127, imageGray(j,i));
  }
  // Do not test color conversion code since it is yet tested.
}

// TODO(pmoulon): Add a real image case. Read back the image and assert
// that the depth is 1.
// Include that it will require
/* #include <string>
 #include "libmv/image/image_io.h"

TEST(Image_Converter, RealImage) {

  Array3Du imageColor;
  EXPECT_TRUE(ReadJpg( std::string("Color.jpg").c_str() , &imageColor));

  Array3Du imageGray;
  Rgb2Gray(imageColor, &imageGray);

  EXPECT_EQ(1 , imageGray.Depth());
  EXPECT_TRUE(WriteJpg(imageGray, std::string("Gray.jpg").c_str(), 100));
}*/
