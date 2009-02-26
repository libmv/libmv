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

#ifndef LIBMV_CORRESPONDENCE_KDTREE_H_
#define LIBMV_CORRESPONDENCE_KDTREE_H_


#include <vector>
#include <algorithm>
#include <cmath>
#include <cassert>

#include "libmv/numeric/numeric.h"

namespace libmv {

// A simple priority cue used to sort the nodes to explore for A-knn.
// Low priority values are poped first.
// TODO(pau): if we are going to use this anywhere else, put it on a separate
//            file.
template<typename Value, typename Priority>
class PriorityQueue {
 public:
  struct Node {
    Value value;
    Priority priority;
    Node(Value v, Priority p) : value(v), priority(p) {}
    bool operator<(const Node &b) {
      return priority > b.priority;
    }
  };

  bool IsEmpty() const { return nodes_.empty(); }
  size_t Size() const { return nodes_.size(); }

  Value Top() { return nodes_.front().value; }
  Priority TopPriority() { return nodes_.front().priority; }
  
  void Push(Value v, Priority p) {
    nodes_.push_back(Node(v,p));
    std::push_heap(nodes_.begin(), nodes_.end());
  }

  Value Pop() {
    Value top = Top();
    std::pop_heap(nodes_.begin(), nodes_.end());
    nodes_.pop_back();
    return top;
  }

 private:
  std::vector<Node> nodes_;
};


// A sorted list of points and their distances to the query.
template<typename Point, typename Scalar=double>
class KnnSortedList {
 public:
  KnnSortedList(int k) : k_(k) {}

  // Adds a point into the sorted list of neighbors
  void AddNeighbor(const Point &p, Scalar distance) {
    if (!Full()) {
      points_.push_back(p);
      distances_.push_back(distance);
    } else if (distance < distances_.back()) {
      points_.back() = p;
      distances_.back() = distance;
    }

    int i = Size() - 2;
    while (i >= 0 && distance < distances_[i]) {
      std::swap(points_[i], points_[i + 1]);
      std::swap(distances_[i], distances_[i + 1]);
      --i;
    }
  }

  int K() { return k_; }
  int Size() { return points_.size(); }
  bool Full() { return Size() >= K(); }
  Point Neighbor(int i) { return points_[i]; }
  Scalar Distance(int i) { return distances_[i]; }
  Scalar FarthestDistance() { return Distance(Size() - 1); }

 private:
  int k_;
  std::vector<Point> points_;
  std::vector<Scalar> distances_;
};


// A simple Kd-tree for fast approximate nearest neighbor search.
template <typename Point, typename Scalar=double>
class KdTree {

  struct KdNode {
    int axis;
    Scalar value;
    Point *begin;
    Point *end;
  };

  struct AxisComparison {
    AxisComparison(int axis) : axis_(axis) {};
    bool operator()(const Point &x, const Point &y) {
      return x[axis_] < y[axis_];
    }
   private:
    int axis_;
  };

 public:
  void Build(Point *points, int num_points, int num_dims, int max_levels) {
    num_dims_ = num_dims;

    // Compute the number of levels such that any leafs has at least 1 point.
    // This is num_leafs < num_points, with num_leafs = 2**(num_levels - 1).
    int l = int(floor(log(num_points) / log(2))) + 1;
    num_levels_ = std::min(l, max_levels);

    // Allocate room for all the nodes at once.
    nodes_.resize((1 << num_levels_) - 1);
    
    // Recursively create the nodes.
    CreateNode(0, points, points + num_points);
  }

  int NumNodes() const { return nodes_.size(); }
  int NumLeafs() const { return (NumNodes() + 1) / 2; }
  int NumLevels() const { return num_levels_; }

  void PrintNodes() const {
    for (int i = 0; i < nodes_.size(); ++i) {
      printf("Node %d: axis=%d  value=%g  num_points=%d\n", i, nodes_[i].axis,
             nodes_[i].value, nodes_[i].end - nodes_[i].begin);
    }
  }

  // Finds the nearest neighbors of query using the best bin first strategy.
  // It stops searching when it's found, or when it has explored max_leafs leafs.
  // Returns the number of explored leafs.
  int ApproximateNearestNeighborBestBinFirst(const Point &query,
                                             int max_leafs,
                                             size_t *nearest_neigbor_index,
                                             Scalar *distance) const {
    KnnSortedList<Point *, Scalar> knn(1);
    int leafs = ApproximateKnnBestBinFirst(query, max_leafs, &knn);
    *nearest_neigbor_index = knn.Neighbor(0) - nodes_[0].begin;
    *distance = knn.Distance(0);
    return leafs;
  }

  // Finds the k nearest neighbors of query using the best bin first strategy.
  // It stops searching when the knn are found, or when it has explored
  // max_leafs leafs.  Returns the number of explored leafs.
  int ApproximateKnnBestBinFirst(const Point &query,
                                 int max_leafs,
                                 KnnSortedList<Point *, Scalar> *neighbors) const {
    int num_explored_leafs = 0;
    PriorityQueue<int, Scalar> queue;
    queue.Push(0, 0); // Push root node.

    while (!queue.IsEmpty() && num_explored_leafs < max_leafs) {
      // Stop if best node is farther than worst neighbor found so far.
      if (neighbors->Full()
          && queue.TopPriority() >= neighbors->FarthestDistance()) {
        break;
      }
      
      int i = queue.Pop();

      // Go down to leaf.
      while (!IsLeaf(i)) {
        Scalar margin = query[nodes_[i].axis] - nodes_[i].value;
        if (margin < 0) {
          queue.Push(RightChild(i), -margin);
          i = LeftChild(i);
        } else {
          queue.Push(LeftChild(i), margin);
          i = RightChild(i);
        }
      }

      // Explore leaf.
      for (Point *p = nodes_[i].begin; p < nodes_[i].end; ++p) {
        Scalar distance = L2Distance2(*p, query);
        neighbors->AddNeighbor(p, distance);
      }
      num_explored_leafs++;
    }
    return num_explored_leafs;
  }


 private:
  int LeftChild(int i) const {
    return 2 * i + 1;
  }
  int RightChild(int i) const {
    return 2 * i + 2;
  }
  int Parent(int i) const {
    return (i - 1) / 2;
  }
  bool IsLeaf(int i) const {
    return i >= NumNodes() / 2; // Note NumNodes() is odd.
  }

  void CreateNode(int i, Point *begin, Point *end) {
    int num_points = end - begin;
    assert(num_points > 0);

    // Create the node.
    KdNode &node = nodes_[i];
    node.begin = begin;
    node.end = end;

    if (!IsLeaf(i)) {
      assert(num_points >= 2);

      // Partition points.
      node.axis = MoreVariantAxis(begin, end);
      AxisComparison comp(node.axis);
      Point *pivot = begin + num_points / 2;
      std::nth_element(begin, pivot, end, comp);
      node.value = (*pivot)[node.axis];

      // Create sub-trees.
      CreateNode(LeftChild(i), begin, pivot);
      CreateNode(RightChild(i), pivot, end);
    } 
  }

  int MoreVariantAxis(Point *begin, Point *end) {
    // Compute variances.
    Vec mean = Vec::Zero(num_dims_);
    Vec mean2 = Vec::Zero(num_dims_);
    for (Point *p = begin; p < end; ++p) {
      for (int i = 0; i < num_dims_; ++i) {
        Scalar x = (*p)[i];
        mean[i] += x;
        mean2[i] += x * x;
      }
    }
    
    int num_points = end - begin;
    mean /= num_points;
    mean2 /= num_points;
    Vec variance = mean2 - mean.cwise().square();

    // Find the largest.
    int more_variant_axis;
    variance.maxCoeff(&more_variant_axis);
    return more_variant_axis;
  }


  Scalar L2Distance2(const Point &p, const Point &q) const {
    Scalar distance = 0;
    for (int i = 0; i < num_dims_; ++i) {
      Scalar diff = p[i] - q[i];
      distance += diff * diff;
    }
    return distance;
  }

 private:
  std::vector<KdNode> nodes_;
  int num_dims_;
  int num_levels_;
};


}  // namespace libmv


#endif // LIBMV_CORRESPONDENCE_KDTREE_H_
