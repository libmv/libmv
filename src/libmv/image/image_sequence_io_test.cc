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

#include <string>
#include <vector>

#include "libmv/image/cached_image_sequence.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_sequence_io.h"
#include "testing/testing.h"

using libmv::Image;
using libmv::ImageCache;
using libmv::ImageSequence;
using libmv::ImageSequenceFromFiles;
using libmv::Array3Df;
using std::string;

namespace {

TEST(ImageSequenceIO, FromFiles) {
  Array3Df image1(1, 2);
  Array3Df image2(1, 2);
  image1(0,0) = 1.f;
  image1(0,1) = 0.f;
  image2(0,0) = 0.f;
  image2(0,1) = 1.f;

  string image1_fn = string(THIS_SOURCE_DIR) + "/1.pgm";
  string image2_fn = string(THIS_SOURCE_DIR) + "/2.pgm";
  WritePnm(image1, image1_fn.c_str());
  WritePnm(image2, image2_fn.c_str());

  std::vector<std::string> files;
  files.push_back(image1_fn);
  files.push_back(image2_fn);
  ImageCache cache;
  ImageSequence *sequence = ImageSequenceFromFiles(files, &cache);
  EXPECT_EQ(2, sequence->Length());

  Array3Df *image = sequence->GetFloatImage(0);
  ASSERT_TRUE(image);
  EXPECT_EQ(2, image->Width());
  EXPECT_EQ(1, image->Height());
  EXPECT_EQ(1, image->Depth());
  EXPECT_EQ((*image)(0,0), 1.f);
  EXPECT_EQ((*image)(0,1), 0.f);

  image = sequence->GetFloatImage(1);
  ASSERT_TRUE(image);
  EXPECT_EQ(2, image->Width());
  EXPECT_EQ(1, image->Height());
  EXPECT_EQ(1, image->Depth());
  EXPECT_EQ((*image)(0,0), 0.f);
  EXPECT_EQ((*image)(0,1), 1.f);

  sequence->Unpin(0);
  sequence->Unpin(1);

  unlink(image1_fn.c_str());
  unlink(image2_fn.c_str());
}

}  // namespace
