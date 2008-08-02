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

#include "libmv/image/image_pyramid.h"
#include "libmv/image/mock_image_sequence.h"
#include "libmv/image/pyramid_sequence.h"
#include "testing/testing.h"

using libmv::Array3Df;
using libmv::ImagePyramid;
using libmv::ImageCache;
using libmv::MockImageSequence;
using libmv::PyramidSequence;

namespace {

// TODO(keir): Make this pass valgrind clean!
TEST(FilteredSequence, TwoLevelFilters) {
  ImageCache cache;
  MockImageSequence source(&cache);
  Array3Df image0(16, 16);
  Array3Df image1(32, 32);

  image0.Fill(1);
  image1.Fill(9);
  source.Append(&image0);
  source.Append(&image1);

  PyramidSequence *pyramid_sequence = MakePyramidSequence(&source, 2, 1.0);

  // TODO(keir): Add helpers to make it easier to compare arrays in tests.
  ImagePyramid *pyramid0 = pyramid_sequence->Pyramid(0);
  const Array3Df &imageP0L0 = pyramid0->Level(0);
  ASSERT_EQ(16, imageP0L0.Height());
  ASSERT_EQ(16, imageP0L0.Width());
  ASSERT_EQ( 3, imageP0L0.Depth());
  EXPECT_NEAR(1.0, imageP0L0(4, 4, 0), 1e-9);  // Blurred.
  EXPECT_NEAR(0.0, imageP0L0(4, 4, 1), 1e-9);  // Gradient x.
  EXPECT_NEAR(0.0, imageP0L0(4, 4, 2), 1e-9);  // Gradient y.

  const Array3Df &imageP0L1 = pyramid0->Level(1);
  ASSERT_EQ( 8, imageP0L1.Height());
  ASSERT_EQ( 8, imageP0L1.Width());
  ASSERT_EQ( 3, imageP0L1.Depth());
  EXPECT_NEAR(1.0, imageP0L1(4, 4, 0), 1e-9);  // Blurred.
  EXPECT_NEAR(0.0, imageP0L1(4, 4, 1), 1e-9);  // Gradient x.
  EXPECT_NEAR(0.0, imageP0L1(4, 4, 2), 1e-9);  // Gradient y.

  // TODO(keir): Add helpers to make it easier to compare arrays in tests.
  ImagePyramid *pyramid1 = pyramid_sequence->Pyramid(1);
  const Array3Df &imageP1L0 = pyramid1->Level(0);
  ASSERT_EQ(32, imageP1L0.Height());
  ASSERT_EQ(32, imageP1L0.Width());
  ASSERT_EQ( 3, imageP1L0.Depth());
  EXPECT_NEAR(9.0, imageP1L0(4, 4, 0), 1e-9);  // Blurred.
  EXPECT_NEAR(0.0, imageP1L0(4, 4, 1), 1e-9);  // Gradient x.
  EXPECT_NEAR(0.0, imageP1L0(4, 4, 2), 1e-9);  // Gradient y.

  const Array3Df &imageP1L1 = pyramid1->Level(1);
  ASSERT_EQ(16, imageP1L1.Height());
  ASSERT_EQ(16, imageP1L1.Width());
  ASSERT_EQ( 3, imageP1L1.Depth());
  EXPECT_NEAR(9.0, imageP1L1(4, 4, 0), 1e-9);  // Blurred.
  EXPECT_NEAR(0.0, imageP1L1(4, 4, 1), 1e-9);  // Gradient x.
  EXPECT_NEAR(0.0, imageP1L1(4, 4, 2), 1e-9);  // Gradient y.
}

}  // namespace
