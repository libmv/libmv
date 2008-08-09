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
#include "libmv/image/image_sequence_io.h"
#include "testing/testing.h"

using libmv::Image;
using libmv::ImageCache;
using libmv::ImageSequence;
using libmv::ImageSequenceFromFiles;
using libmv::Array3Df;

namespace {

TEST(ImageSequenceIO, FromFiles) {
  std::vector<std::string> files;
  files.push_back(std::string(THIS_SOURCE_DIR) + "/image_test/two_pixels.pgm");
  files.push_back(std::string(THIS_SOURCE_DIR) + "/image_test/two_pixels_gray.pgm");
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
  EXPECT_EQ((*image)(0,0), 1.0f);
  EXPECT_NEAR((*image)(0,1), 0.5f, 0.002);

  sequence->Unpin(0);
  sequence->Unpin(1);
}

}  // namespace
