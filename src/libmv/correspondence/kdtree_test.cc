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

#include "testing/testing.h"
#include "libmv/numeric/numeric.h"
#include "libmv/correspondence/kdtree.h"

using libmv::KdTree;
using libmv::Vec2;

namespace {

TEST(KdTree, Build) {
  Vec2 points[3];
  points[0] << 3, 3;
  points[1] << 2, 2;
  points[2] << 1, 1;

  KdTree<Vec2> tree;
  tree.Build(points, 3, 2);

  EXPECT_EQ(5, tree.NumNodes());
  EXPECT_EQ(1, points[0][0]);
  EXPECT_EQ(1, points[0][1]);
  EXPECT_EQ(2, points[1][0]);
  EXPECT_EQ(2, points[1][1]);
  EXPECT_EQ(3, points[2][0]);
  EXPECT_EQ(3, points[2][1]);
}

}  // namespace
