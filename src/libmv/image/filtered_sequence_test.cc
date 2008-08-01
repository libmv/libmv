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

#include "libmv/image/image.h"
#include "libmv/image/filtered_sequence.h"
#include "libmv/image/mock_image_sequence.h"
#include "testing/testing.h"

using libmv::MockImageSequence;
using libmv::Filter;
using libmv::FilteredImageSequence;
using libmv::ImageCache;
using libmv::Array3Df;

namespace {

class AddOneFilter : public Filter {
  virtual ~AddOneFilter() {}
  void RunFilter(const Array3Df &source, Array3Df *destination) {
    destination->ResizeLike(source);

    for (int r = 0; r < source.Height(); ++r) {
      for (int c = 0; c < source.Width(); ++c) {
        for (int d = 0; d < source.Depth(); ++d) {
          (*destination)(r, c, d) = source(r, c, d) + 1.0;
        }
      }
    }
  }
};

// TODO(keir): Make this pass valgrind clean!
TEST(FilteredSequence, TwoLevelFilters) {
  ImageCache cache;
  MockImageSequence source(&cache);
  Array3Df image0(3,4);
  Array3Df image1(4,4);

  image0.Fill(1);
  image1.Fill(10);
  source.Append(&image0);
  source.Append(&image1);

  FilteredImageSequence filterA(&source, new AddOneFilter());
  FilteredImageSequence filterB(&filterA, new AddOneFilter());

  // TODO(keir): Add helpers to make it easier to compare arrays in tests.
  const Array3Df filteredA0 = *filterA.GetFloatImage(0);
  ASSERT_EQ(image0.Height(), filteredA0.Height());
  ASSERT_EQ(image0.Width(),  filteredA0.Width());
  ASSERT_EQ(image0.Depth(),  filteredA0.Depth());
  EXPECT_EQ(2.0, filteredA0(0,0));
  const Array3Df filteredA1 = *filterA.GetFloatImage(1);
  ASSERT_EQ(image1.Height(), filteredA1.Height());
  ASSERT_EQ(image1.Width(),  filteredA1.Width());
  ASSERT_EQ(image1.Depth(),  filteredA1.Depth());
  EXPECT_EQ(11.0, filteredA1(0,0));
  filterA.Unpin(0);
  filterA.Unpin(1);

  const Array3Df filteredB0 = *filterB.GetFloatImage(0);
  ASSERT_EQ(image0.Height(), filteredA0.Height());
  ASSERT_EQ(image0.Width(),  filteredA0.Width());
  ASSERT_EQ(image0.Depth(),  filteredA0.Depth());
  EXPECT_EQ(3.0, filteredB0(0,0));
  const Array3Df filteredB1 = *filterB.GetFloatImage(1);
  ASSERT_EQ(image1.Height(), filteredA1.Height());
  ASSERT_EQ(image1.Width(),  filteredA1.Width());
  ASSERT_EQ(image1.Depth(),  filteredA1.Depth());
  EXPECT_EQ(12.0, filteredB1(0,0));
  filterB.Unpin(0);
  filterB.Unpin(1);
}

}  // namespace
