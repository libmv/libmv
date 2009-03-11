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

#include "libmv/correspondence/bipartite_graph_new.h"
#include "testing/testing.h"

using libmv::BipartiteGraph;

namespace {

TEST(BipartiteGraph, Empty) {
  BipartiteGraph<int, int, int> x;
  EXPECT_EQ(0, x.NumEdges());
  EXPECT_EQ(0, x.NumLeftNodes());
  EXPECT_EQ(0, x.NumRightNodes());
}

TEST(BipartiteGraph, InsertEdges) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 3, 2); EXPECT_EQ(1, x.NumEdges());
  x.Insert(1, 3, 2); EXPECT_EQ(1, x.NumEdges());
  x.Insert(2, 4, 2); EXPECT_EQ(2, x.NumEdges());
}

TEST(BipartiteGraph, InsertDuplicateEdges) {
  // Since edges should be unique when inserted, the following behaviour, while
  // surprising, is expected. There is an argument that the Insert() function
  // should assert fail if an edge is inserted twice; for now leaving as is.
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 3, 2); EXPECT_EQ(1, x.NumEdges());
  x.Insert(2, 3, 2); EXPECT_EQ(1, x.NumEdges());
  x.Insert(4, 3, 1); EXPECT_EQ(1, x.NumEdges());
}

TEST(BipartiteGraph, NumLeftRight) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 3, 2);
  EXPECT_EQ(x.NumLeftNodes(), 1);
  EXPECT_EQ(x.NumRightNodes(), 1);
  EXPECT_EQ(x.NumEdges(), 1);
  x.Insert(2, 4, 2);
  EXPECT_EQ(x.NumLeftNodes(), 2);
  EXPECT_EQ(x.NumRightNodes(), 1);
  EXPECT_EQ(x.NumEdges(), 2);
}

TEST(BipartiteGraph, MultipleInsertions) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 10, 2);
  x.Insert(2, 20, 2);
  x.Insert(1, 30, 4);
  x.Insert(2, 40, 3);
  int edge;
  EXPECT_TRUE(x.GetEdge(1, 2, &edge)); EXPECT_EQ(10, edge);
  EXPECT_TRUE(x.GetEdge(2, 2, &edge)); EXPECT_EQ(20, edge);
  EXPECT_TRUE(x.GetEdge(1, 4, &edge)); EXPECT_EQ(30, edge);
  EXPECT_TRUE(x.GetEdge(2, 3, &edge)); EXPECT_EQ(40, edge);
}

TEST(BipartiteGraph, EdgeIterator) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 10, 2);
  x.Insert(1, 30, 4);
  x.Insert(2, 20, 2);

  // The edge iterator returns sorted edges.
  BipartiteGraph<int, int, int>::EdgeIterator it = x.EdgesBegin();
  ASSERT_FALSE(it == x.EdgesEnd());
  EXPECT_EQ(1, it.left());
  EXPECT_EQ(2, it.right());
  EXPECT_EQ(10, it.edge());
  ++it;

  ASSERT_FALSE(it == x.EdgesEnd());
  EXPECT_EQ(2, it.left());
  EXPECT_EQ(2, it.right());
  EXPECT_EQ(20, it.edge());
  ++it;

  ASSERT_FALSE(it == x.EdgesEnd());
  EXPECT_EQ(1, it.left());
  EXPECT_EQ(4, it.right());
  EXPECT_EQ(30, it.edge());
  ++it;

  ASSERT_TRUE(it == x.EdgesEnd());
}

TEST(BipartiteGraph, LeftIterator) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 10, 2);
  x.Insert(1, 30, 4);
  x.Insert(2, 20, 2);

  BipartiteGraph<int, int, int>::LeftIterator it = x.LeftBegin();
  ASSERT_TRUE(it != x.LeftEnd()); EXPECT_EQ(1, *it); ++it;
  ASSERT_TRUE(it != x.LeftEnd()); EXPECT_EQ(2, *it); ++it;
  ASSERT_FALSE(it != x.LeftEnd());
}

TEST(BipartiteGraph, LeftRightEdgeIterators) {
  BipartiteGraph<int, int, int> x;
  int expected_ler[] = {
    1, 10, 2,
    1, 30, 4,
    1, 40, 5,
    2, 20, 5,
    5, 50, 5,
    0,
  };
  for (int i = 0; expected_ler[i]; i += 3) {
    x.Insert(expected_ler[i + 0], expected_ler[i + 1], expected_ler[i + 2]);
  }

  // Over left nodes, then incoming edges.
  int i = 0;
  for (BipartiteGraph<int, int, int>::LeftIterator it = x.LeftBegin();
       it != x.LeftEnd(); ++it) {
    for (BipartiteGraph<int, int, int>::LeftEdgeIterator et = it.begin();
         et != it.end(); ++et) {
      EXPECT_EQ(expected_ler[i + 0], et.left());
      EXPECT_EQ(expected_ler[i + 1], et.edge());
      EXPECT_EQ(expected_ler[i + 2], et.right());
      i += 3;
    }
  }
  EXPECT_EQ(i, 15);
  
  // Over right nodes, then incoming edges.
  i = 0;
  for (BipartiteGraph<int, int, int>::RightIterator it = x.RightBegin();
       it != x.RightEnd(); ++it) {
    for (BipartiteGraph<int, int, int>::RightEdgeIterator et = it.begin();
         et != it.end(); ++et) {
      EXPECT_EQ(expected_ler[i + 0], et.left());
      EXPECT_EQ(expected_ler[i + 1], et.edge());
      EXPECT_EQ(expected_ler[i + 2], et.right());
      i += 3;
    }
  }
  EXPECT_EQ(i, 15);
}

}  // namespace
