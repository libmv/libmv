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
#include "testing/testing.h"

using libmv::Image;
using libmv::ReadPgm;

namespace {

TEST(ImageSizes) {
  Image<int> im;

  Check(im.Height() == 0);
  Check(im.Width() == 0);
  Check(im.Depth() == 0);

  Check(im.Shape(0) == 0);
}

TEST(ImageParenthesis) {
  Image<int> im(1,2,3);
  im(0,1,0) = 3;

  Check(im(0,1) == 3);
}

TEST(ReadPgm) {
  Image<unsigned char> im;
  Check(ReadPgm(&im, "src/libmv/image/image_test/two_pixels.pgm"));
  Check(im(0,0) == (unsigned char)255);
  Check(im(0,1) == (unsigned char)0);

  Check(!ReadPgm(&im, "src/libmv/image/image_test/two_pixels.png"));
  Check(!ReadPgm(&im, "hopefully_unexisting_file"));
}

}  // namespace
