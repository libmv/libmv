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
  EXPECT_EQ(0, x.NumEdges());
  EXPECT_EQ(0, x.NumLeftNodes());
  EXPECT_EQ(0, x.NumRightNodes());
}

TEST(BipartiteGraph, InsertEdges) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 2, 3);
  EXPECT_EQ(1, x.NumEdges());
  x.Insert(1, 2, 3);
  EXPECT_EQ(1, x.NumEdges());
  x.Insert(2, 2, 3);
  EXPECT_EQ(2, x.NumEdges());
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

TEST(BipartiteGraph, EdgeIterator) {
  BipartiteGraph<int, int, int> x;
  x.Insert(1, 2, 10);
  x.Insert(1, 4, 30);
  x.Insert(2, 2, 20);

  BipartiteGraph<int, int, int>::Iterator it = x.ScanAllEdges();
  ASSERT_FALSE(it.Done());
  EXPECT_EQ(1, it.left());
  EXPECT_EQ(2, it.right());
  EXPECT_EQ(10, it.edge());
  it.Next();

  ASSERT_FALSE(it.Done());
  EXPECT_EQ(1, it.left());
  EXPECT_EQ(4, it.right());
  EXPECT_EQ(30, it.edge());
  it.Next();

  ASSERT_FALSE(it.Done());
  EXPECT_EQ(2, it.left());
  EXPECT_EQ(2, it.right());
  EXPECT_EQ(20, it.edge());
  it.Next();

  ASSERT_TRUE(it.Done());
}

typedef BipartiteGraph<int, char, float> TestBipartiteGraph;

TEST(BipartiteGraph, ScanEdgesForRightNodeOneItemHit) {
  TestBipartiteGraph x;
  x.Insert(1, 1.5f, 'a');
  TestBipartiteGraph::Iterator it = x.ScanEdgesForRightNode(1.5f);
  EXPECT_FALSE(it.Done());
  EXPECT_EQ(1, it.left());
  EXPECT_EQ(1.5f, it.right());
  EXPECT_EQ('a', it.edge());
  it.Next();
  EXPECT_TRUE(it.Done());
}

void CheckIteratorOutput(TestBipartiteGraph::Iterator it,
                         const TestBipartiteGraph::Entry *expected,
                         int expected_size) {
  int i = 0;
  for (; !it.Done(); it.Next()) {
    ASSERT_TRUE(i < expected_size);
    EXPECT_EQ(expected[i].left, it.left());
    EXPECT_EQ(expected[i].right, it.right());
    EXPECT_EQ(expected[i].edge, it.edge());
    ++i;
  }
  EXPECT_EQ(expected_size, i);
}

TEST(BipartiteGraph, ScanEdgesForRightNode) {
  TestBipartiteGraph x;
  x.Insert(1,  1.5f, 'a');
  x.Insert(1, 30.5f, 'c');
  x.Insert(2, 30.5f, 'd');
  x.Insert(2,  0.5f, 'e');
  x.Insert(3, 30.5f, 'f');

  TestBipartiteGraph::Entry kExpected[] = {
    { 1, 'c', 30.5f },
    { 2, 'd', 30.5f },
    { 3, 'f', 30.5f },
  };

  CheckIteratorOutput(x.ScanEdgesForRightNode(30.5f), kExpected, 3);
}

TEST(BipartiteGraph, ScanEdgesForLeftNode) {
  TestBipartiteGraph x;
  x.Insert(1,  1.5f, 'a');
  x.Insert(2, 30.5f, 'c');
  x.Insert(2, 40.5f, 'd');
  x.Insert(2, 50.5f, 'e');
  x.Insert(3,  2.5f, 'a');

  TestBipartiteGraph::Entry kExpected[] = {
    { 2, 'c', 30.5f },
    { 2, 'd', 40.5f },
    { 2, 'e', 50.5f },
  };

  CheckIteratorOutput(x.ScanEdgesForLeftNode(2), kExpected, 3);
}

}  // namespace
