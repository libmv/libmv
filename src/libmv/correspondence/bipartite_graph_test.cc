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

TEST(BipartiteGraph, MultipleInsertions) {
  BipartiteGraph<int, int> x;
  x.Insert(1, 2, 10);
  x.Insert(2, 2, 20);
  x.Insert(1, 4, 30);
  x.Insert(2, 3, 40);
  EXPECT_EQ(*x.Edge(1, 2), 10);
  EXPECT_EQ(*x.Edge(2, 2), 20);
  EXPECT_EQ(*x.Edge(1, 4), 30);
  EXPECT_EQ(*x.Edge(2, 3), 40);
}

TEST(BipartiteGraph, EdgeIterator) {
  BipartiteGraph<int, int> x;
  x.Insert(1, 2, 10);
  x.Insert(1, 4, 30);
  x.Insert(2, 2, 20);

  BipartiteGraph<int, int>::Range r = x.All();
  ASSERT_TRUE(r);
  EXPECT_EQ(1,  r.left());
  EXPECT_EQ(2,  r.right());
  EXPECT_EQ(10, r.edge());
  ++r;

  ASSERT_TRUE(r);
  EXPECT_EQ(1,  r.left());
  EXPECT_EQ(4,  r.right());
  EXPECT_EQ(30, r.edge());
  ++r;

  ASSERT_TRUE(r);
  EXPECT_EQ(2,  r.left());
  EXPECT_EQ(2,  r.right());
  EXPECT_EQ(20, r.edge());
  ++r;

  ASSERT_FALSE(r);
}

typedef BipartiteGraph<int, char> TestGraph;

TEST(BipartiteGraph, ScanEdgesForRightNodeOneItemHit) {
  TestGraph x;
  x.Insert(1, 2, 'a');
  TestGraph::Range r = x.ToRight(2);
  EXPECT_TRUE(r);
  EXPECT_EQ(1, r.left());
  EXPECT_EQ(2, r.right());
  EXPECT_EQ('a', r.edge());
  ++r;
  EXPECT_FALSE(r);
}

struct Entry {
  int left, right;
  char edge;
};

void CheckIteratorOutput(TestGraph::Range it,
                         const Entry *expected) {
  int i = 0;
  for (; it; ++it) {
    ASSERT_TRUE(expected[i].left);
    EXPECT_EQ(expected[i].left,  it.left());
    EXPECT_EQ(expected[i].right, it.right());
    EXPECT_EQ(expected[i].edge,  it.edge());
    ++i;
  }
  EXPECT_EQ(0, expected[i].left);  // Zero terminated.
}

TEST(BipartiteGraph, ScanEdgesForRightNode) {
  TestGraph x;
  x.Insert(1, 1, 'a');
  x.Insert(1, 2, 'c');
  x.Insert(2, 2, 'd');
  x.Insert(2, 3, 'e');
  x.Insert(3, 2, 'f');

  Entry kExpected[] = {
    { 1, 2, 'c' },
    { 2, 2, 'd' },
    { 3, 2, 'f' },
    { 0, 0,  0  }
  };

  CheckIteratorOutput(x.ToRight(2), kExpected);
}

TEST(BipartiteGraph, ScanEdgesForLeftNode) {
  TestGraph x;
  x.Insert(1, 1, 'a');
  x.Insert(2, 2, 'c');
  x.Insert(2, 3, 'd');
  x.Insert(2, 4, 'e');
  x.Insert(3, 5, 'a');

  Entry kExpected[] = {
    { 2, 2, 'c' },
    { 2, 3, 'd' },
    { 2, 4, 'e' },
    { 0, 0,  0, }
  };

  CheckIteratorOutput(x.ToLeft(2), kExpected);
}

}  // namespace
