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

using namespace libmv;

namespace {

TEST(PriorityQueue, SortAList) {
  PriorityQueue<int, float> queue;
  queue.Push(2, 2.0f);
  queue.Push(1, 1.0f);
  queue.Push(4, 4.0f);
  queue.Push(3, 3.0f);

  EXPECT_EQ(4, queue.Size());
  EXPECT_EQ(1, queue.Top());
  EXPECT_EQ(1.0f, queue.TopPriority());

  int sorted[4];
  sorted[0] = queue.Pop();
  sorted[1] = queue.Pop();
  sorted[2] = queue.Pop();
  sorted[3] = queue.Pop();
  EXPECT_EQ(1, sorted[0]);
  EXPECT_EQ(2, sorted[1]);
  EXPECT_EQ(3, sorted[2]);
  EXPECT_EQ(4, sorted[3]);
}


TEST(KnnSortedList, Size) {
  KnnSortedList<int, float> knn(9);
  knn.AddNeighbor(1, 1.0f);
  EXPECT_EQ(1, knn.Size());
  EXPECT_EQ(9, knn.K());
}

TEST(KnnSortedList, FarthestDistance) {
  KnnSortedList<int, float> knn(9);
  knn.AddNeighbor(1, 1.0f);
  knn.AddNeighbor(3, 3.0f);
  knn.AddNeighbor(2, 2.0f);
  EXPECT_EQ(3.0f, knn.FarthestDistance());
}

TEST(KnnSortedList, SortAList) {
  KnnSortedList<int, float> knn(3);
  knn.AddNeighbor(2, 2.0f);
  knn.AddNeighbor(1, 1.0f);
  knn.AddNeighbor(4, 4.0f);
  knn.AddNeighbor(3, 3.0f);
  EXPECT_EQ(3, knn.Size());
  EXPECT_EQ(3, knn.K());
  EXPECT_EQ(1,    knn.Neighbor(0));
  EXPECT_EQ(1.0f, knn.Distance(0));
  EXPECT_EQ(2,    knn.Neighbor(1));
  EXPECT_EQ(2.0f, knn.Distance(1));
  EXPECT_EQ(3,    knn.Neighbor(2));
  EXPECT_EQ(3.0f, knn.Distance(2));
}


TEST(KdTree, Build) {
  Vec2 points[4];
  points[0] << 4, 4;
  points[1] << 3, 3;
  points[2] << 2, 2;
  points[3] << 1, 1;

  KdTree<Vec2> tree;
  tree.Build(points, 4, 2, 10);

  EXPECT_EQ(7, tree.NumNodes());
  EXPECT_EQ(1, points[0][0]);
  EXPECT_EQ(1, points[0][1]);
  EXPECT_EQ(2, points[1][0]);
  EXPECT_EQ(2, points[1][1]);
  EXPECT_EQ(3, points[2][0]);
  EXPECT_EQ(3, points[2][1]);
  EXPECT_EQ(4, points[3][0]);
  EXPECT_EQ(4, points[3][1]);
}

TEST(KdTree, MoreVariantAxis) {
  Vec2 points[4];
  points[0] << 40, 1;
  points[1] << 30, 2;
  points[2] << 20, 3;
  points[3] << 10, 4;

  KdTree<Vec2> tree;
  tree.Build(points, 4, 2, 10);

  EXPECT_EQ(7, tree.NumNodes());
  EXPECT_EQ(10, points[0][0]);
  EXPECT_EQ( 4, points[0][1]);
  EXPECT_EQ(20, points[1][0]);
  EXPECT_EQ( 3, points[1][1]);
  EXPECT_EQ(30, points[2][0]);
  EXPECT_EQ( 2, points[2][1]);
  EXPECT_EQ(40, points[3][0]);
  EXPECT_EQ( 1, points[3][1]);
}

TEST(KdTree, ApproximateNearestNeighborBestBinFirst) {
  Vec2 points[4];
  points[0] << 1, 1;
  points[1] << 1, 2;
  points[2] << 2, 1;
  points[3] << 2, 2;

  KdTree<Vec2> tree;
  tree.Build(points, 4, 2, 10);

  Vec2 query;
  query << 1.1, 1.2;

  KnnSortedList<Vec2 *> neighbors(1);
  Vec2 *nn;
  double distance;
  tree.ApproximateNearestNeighborBestBinFirst(query, 1000, &nn, &distance);
  EXPECT_EQ(1, (*nn)[0]);
  EXPECT_EQ(1, (*nn)[1]);
  EXPECT_NEAR(0.1 * 0.1 + 0.2 * 0.2, distance, 1e-10);
}

TEST(KdTree, ApproximateKnnBestBinFirstSmall) {
  Vec2 points[4];
  points[0] << 1, 1;
  points[1] << 1, 2;
  points[2] << 2, 1;
  points[3] << 2, 2;

  KdTree<Vec2> tree;
  tree.Build(points, 4, 2, 10);

  Vec2 queries[4];
  queries[0] << 1.1, 1.2;
  queries[1] << 1.3, 2;
  queries[2] << 2, 1.4;
  queries[3] << 1.8, 1.7;

  KnnSortedList<Vec2 *> neighbors0(1);
  KnnSortedList<Vec2 *> neighbors1(1);
  KnnSortedList<Vec2 *> neighbors2(1);
  KnnSortedList<Vec2 *> neighbors3(1);
  tree.ApproximateKnnBestBinFirst(queries[0], 1000, &neighbors0);
  tree.ApproximateKnnBestBinFirst(queries[1], 1000, &neighbors1);
  tree.ApproximateKnnBestBinFirst(queries[2], 1000, &neighbors2);
  tree.ApproximateKnnBestBinFirst(queries[3], 1000, &neighbors3);
  Vec2 p0 = *neighbors0.Neighbor(0);
  Vec2 p1 = *neighbors1.Neighbor(0);
  Vec2 p2 = *neighbors2.Neighbor(0);
  Vec2 p3 = *neighbors3.Neighbor(0);

  EXPECT_EQ(1, p0[0]);
  EXPECT_EQ(1, p0[1]);
  EXPECT_EQ(1, p1[0]);
  EXPECT_EQ(2, p1[1]);
  EXPECT_EQ(2, p2[0]);
  EXPECT_EQ(1, p2[1]);
  EXPECT_EQ(2, p3[0]);
  EXPECT_EQ(2, p3[1]);
}

TEST(KdTree, ApproximateKnnBestBinFirstBig) {
  const int N = 20;
  Vec2 points[N * N];
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      points[i * N + j] << i, j;
    }
  }
  KdTree<Vec2> tree;
  tree.Build(points, N * N, 2, 10);
  
  Vec2 query;
  query << 5.4, 5.2;
  KnnSortedList<Vec2 *> knn(4);
  int num_explored_leafs
      = tree.ApproximateKnnBestBinFirst(query, tree.NumLeafs(), &knn);
  Vec2 p0 = *knn.Neighbor(0);
  Vec2 p1 = *knn.Neighbor(1);
  Vec2 p2 = *knn.Neighbor(2);
  Vec2 p3 = *knn.Neighbor(3);

  EXPECT_EQ(5, p0[0]);
  EXPECT_EQ(5, p0[1]);
  EXPECT_EQ(6, p1[0]);
  EXPECT_EQ(5, p1[1]);
  EXPECT_EQ(5, p2[0]);
  EXPECT_EQ(6, p2[1]);
  EXPECT_EQ(6, p3[0]);
  EXPECT_EQ(6, p3[1]);
  EXPECT_LE(num_explored_leafs, 9);
                             // 9 has been found by testing the code itself :(
}

}  // namespace
