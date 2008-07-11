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

#include "libmv/correspondence/bipartite_graph.h"
#include "testing/testing.h"

using libmv::BipartiteGraph;

namespace {

TEST(BipartiteGraph, Empty) {
  BipartiteGraph<int, int, int> x;
  EXPECT_EQ(x.NumEdges(), 0);
  EXPECT_EQ(x.NumLeftNodes(), 0);
  EXPECT_EQ(x.NumRightNodes(), 0);
}

TEST(BipartiteGraph, InsertEdges) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 2, 3);
  EXPECT_EQ(x.NumEdges(), 1);
  x.Insert(1, 2, 3);
  EXPECT_EQ(x.NumEdges(), 1);
  x.Insert(2, 2, 3);
  EXPECT_EQ(x.NumEdges(), 2);
}

TEST(BipartiteGraph, NumLeftRight) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 2, 3);
  EXPECT_EQ(x.NumLeftNodes(), 1);
  EXPECT_EQ(x.NumRightNodes(), 1);
  EXPECT_EQ(x.NumEdges(), 1);
  x.Insert(2, 2, 3);
  EXPECT_EQ(x.NumLeftNodes(), 2);
  EXPECT_EQ(x.NumRightNodes(), 1);
  EXPECT_EQ(x.NumEdges(), 2);
}

TEST(BipartiteGraph, MultipleInsertions) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 2, 10);
  x.Insert(2, 2, 20);
  x.Insert(1, 4, 30);
  x.Insert(2, 3, 40);
  EXPECT_EQ(x.GetEdge(1, 2), 10);
  EXPECT_EQ(x.GetEdge(2, 2), 20);
  EXPECT_EQ(x.GetEdge(1, 4), 30);
  EXPECT_EQ(x.GetEdge(2, 3), 40);
}

}  // namespace
