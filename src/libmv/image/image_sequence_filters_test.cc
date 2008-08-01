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
#include <vector>

#include "libmv/image/image_sequence_filters.h"
#include "libmv/image/mock_image_sequence.h"
#include "testing/testing.h"

using libmv::MockImageSequence;
using libmv::ImageCache;
using libmv::ImageSequence;
using libmv::Array3Df;

namespace {

// TODO(keir): Make this pass valgrind clean!
TEST(BlurAndDerivative, Simple) {
  ImageCache cache;
  MockImageSequence source(&cache);
  Array3Df image0(20,20);
  Array3Df image1(20,20);

  image0.Fill(1);
  image1.Fill(9);
  source.Append(&image0);
  source.Append(&image1);

  ImageSequence *filtered = BlurSequenceAndTakeDerivatives(&source, 0.9);

  const Array3Df filtered_image0 = *filtered->GetFloatImage(0);
  const Array3Df filtered_image1 = *filtered->GetFloatImage(1);
  EXPECT_NEAR(1.0, filtered_image0(10, 10), 1e-8);
  EXPECT_NEAR(0.0, filtered_image0(10, 10, 1), 1e-8);
  EXPECT_NEAR(0.0, filtered_image0(10, 10, 2), 1e-8);
  EXPECT_NEAR(9.0, filtered_image1(10, 10), 1e-8);
  EXPECT_NEAR(0.0, filtered_image1(10, 10, 1), 1e-8);
  EXPECT_NEAR(0.0, filtered_image1(10, 10, 2), 1e-8);
  filtered->Unpin(0);
  filtered->Unpin(0);

  delete filtered;
}

// TODO(keir): Make this pass valgrind clean!
TEST(DownsampleBy2, Simple) {
  ImageCache cache;
  MockImageSequence source(&cache);
  Array3Df image0(2,2);
  image0.Fill(0.);
  image0(0, 0) = 1.0;
  source.Append(&image0);

  ImageSequence *filtered = DownsampleSequenceBy2(&source);

  const Array3Df filtered_image = *filtered->GetFloatImage(0);
  ASSERT_EQ(1, filtered_image.Height());
  ASSERT_EQ(1, filtered_image.Width());
  ASSERT_EQ(1, filtered_image.Depth());
  EXPECT_EQ(0.25, filtered_image(0, 0));
  filtered->Unpin(0);

  delete filtered;
}

}  // namespace
