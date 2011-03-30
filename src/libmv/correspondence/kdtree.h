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

// A simple priority cue used to sort the nodes to explore for A-knn.  Low
// priority values are popped first.
// TODO(pau): if we are going to use this anywhere else, put it on a separate
// file.
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
template<typename Scalar, typename Id>
class KnnSortedList {
 public:
  KnnSortedList(int k) : k_(k) {}

  // Adds a point into the sorted list of neighbors
  void AddNeighbor(const Id &p, Scalar distance) {
    if (!Full()) {
      ids_.push_back(p);
      distances_.push_back(distance);
    } else if (distance < distances_.back()) {
      ids_.back() = p;
      distances_.back() = distance;
    }

    int i = Size() - 2;
    while (i >= 0 && distance < distances_[i]) {
      std::swap(ids_[i], ids_[i + 1]);
      std::swap(distances_[i], distances_[i + 1]);
      --i;
    }
  }

  int K() { return k_; }
  int Size() { return ids_.size(); }
  bool Full() { return Size() >= K(); }
  Id Neighbor(int i) { return ids_[i]; }
  Scalar Distance(int i) { return distances_[i]; }
  Scalar FarthestDistance() { return Distance(Size() - 1); }

 private:
  int k_;
  std::vector<Id> ids_;
  std::vector<Scalar> distances_;
};


// A simple Kd-tree for fast approximate nearest neighbor search.
template <typename Scalar, typename Id = int>
class KdTree {
 public:
  typedef KnnSortedList<Scalar, Id> SearchResults;
 private:
  struct Point {
    Point(const Scalar *d, Id i) : data(d), id(i) {}
    Scalar operator[](int i) const { return data[i]; }
    const Scalar *data;
    Id id;
  };

  struct KdNode {
    int axis;
    Scalar cut_value;
    Scalar min_value;
    Scalar max_value;
    Point *begin;
    Point *end;
  };

  struct AxisComparison {
    AxisComparison(int axis) : axis_(axis) {}
    bool operator()(const Point &x, const Point &y) const {
      return x[axis_] < y[axis_];
    }
   private:
    int axis_;
  };

 public:

  void SetDimensions(int num_dims) {
    num_dims_ = num_dims;
  }

  /**
   * Add a point to the tree with a given id.
   *
   *  \param data A pointer to the raw point data.
   *  \param id   The id of the point.  Used to identify the search results.
   *
   * Points can not be added once the tree is built.
   */
  void AddPoint(const Scalar *data, Id id) {
    assert(nodes_.size() == 0);
    points_.push_back(Point(data, id));
  }

  /**
   * Build the tree.  This function uses the points that have been added using
   * AddPoint.
   */
  void Build(int max_levels) {
    // Compute the number of levels such that any leaf has at least 1 point.
    // This is num_leafs <= num_points, with num_leafs = 2**(num_levels - 1).
    int l = int(floor(log((double)points_.size()) / log(2.))) + 1;
    num_levels_ = std::min(l, max_levels);

    // Allocate room for all the nodes at once.
    nodes_.resize((1 << num_levels_) - 1);

    // Recursively create the nodes.
    CreateNode(0, &points_.front(), &points_.back() + 1);
  }

  int NumNodes() const { return nodes_.size(); }
  int NumLeafs() const { return (NumNodes() + 1) / 2; }
  int NumLevels() const { return num_levels_; }
  int NumDimension() const { return num_dims_; }

  void PrintNodes() const {
    for (int i = 0; i < nodes_.size(); ++i) {
      printf("Node %d: axis=%d  value=%g  num_points=%d\n", i, nodes_[i].axis,
             nodes_[i].cut_value, nodes_[i].end - nodes_[i].begin);
    }
  }

  /**
   * Finds the nearest neighbors of query using the best bin first strategy.
   * It stops searching when it's found, or when it has explored max_leafs
   * leafs.  Returns the number of explored leafs.
   */
  int ApproximateNearestNeighborBestBinFirst(const Scalar *query,
                                             int max_leafs,
                                             Id *nearest_neigbor_id,
                                             Scalar *distance) const {
    SearchResults knn(1);
    int leafs = ApproximateKnnBestBinFirst(query, max_leafs, &knn);
    *nearest_neigbor_id = knn.Neighbor(0);
    *distance = knn.Distance(0);
    return leafs;
  }

  /**
   * Finds the k nearest neighbors of query using the best bin first strategy.
   * It stops searching when the knn are found, or when it has explored
   * max_leafs leafs.  Returns the number of explored leafs.
   */
  // TODO(pau): put a reference to Mount's paper.
  int ApproximateKnnBestBinFirst(const Scalar *query,
                                 int max_leafs,
                                 SearchResults *neighbors) const {
    int num_explored_leafs = 0;
    PriorityQueue<int, Scalar> queue;
    queue.Push(0, 0); // Push root node.

    while (!queue.IsEmpty() && num_explored_leafs < max_leafs) {
      // Stop if best node is farther than worst neighbor found so far.
      float old_distance = queue.TopPriority();
      if (neighbors->Full() && old_distance >= neighbors->FarthestDistance()) {
        break;
      }

      int i = queue.Pop();

      // Go down to leaf.
      while (!IsLeaf(i)) {
        int axis = nodes_[i].axis;
        Scalar cut_value = nodes_[i].cut_value;
        Scalar min_value = nodes_[i].min_value;
        Scalar max_value = nodes_[i].max_value;

        Scalar new_offset = query[axis] - cut_value;
        if (new_offset < 0) {
          Scalar old_offset = std::min(Scalar(0.0), query[axis] - min_value);
          Scalar new_distance = old_distance - old_offset * old_offset
                              + new_offset * new_offset;
          queue.Push(RightChild(i), new_distance);
          i = LeftChild(i);
        } else {
          Scalar old_offset = std::max(Scalar(0.0), query[axis] - max_value);
          Scalar new_distance = old_distance - old_offset * old_offset
                              + new_offset * new_offset;
          queue.Push(LeftChild(i), new_distance);
          i = RightChild(i);
        }
      }
      // Explore leaf.
      for (Point *p = nodes_[i].begin; p < nodes_[i].end; ++p) {
        Scalar distance = L2Distance2(p->data, query);
        neighbors->AddNeighbor(p->id, distance);
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
      node.axis = MoreVariantAxis(begin, end, &node.min_value, &node.max_value);

      AxisComparison comp(node.axis);
      Point *pivot = begin + num_points / 2;
      std::nth_element(begin, pivot, end, comp);
      node.cut_value = (*pivot)[node.axis];

      // Create sub-trees.
      CreateNode(LeftChild(i), begin, pivot);
      CreateNode(RightChild(i), pivot, end);
    }
  }

  int MoreVariantAxis(Point *begin, Point *end,
                      Scalar *min_val, Scalar *max_val) {

    // Compute variances.
    Vec mean = Vec::Zero(num_dims_);
    Vec mean2 = Vec::Zero(num_dims_);
    std::vector<Scalar> min_values(num_dims_,
                                   std::numeric_limits<Scalar>::max());
    std::vector<Scalar> max_values(num_dims_,
                                   -std::numeric_limits<Scalar>::max());
    for (Point *p = begin; p < end; ++p) {
      for (int i = 0; i < num_dims_; ++i) {
        Scalar x = (*p)[i];
        mean[i] += x;
        mean2[i] += x * x;
        if (x < min_values[i]) min_values[i] = x;
        if (x > max_values[i]) max_values[i] = x;
      }
    }

    int num_points = end - begin;
    mean /= num_points;
    mean2 /= num_points;
    Vec variance = mean2.array() - mean.array().square();

    // Find the largest.
    int more_variant_axis;
    variance.maxCoeff(&more_variant_axis);
    *min_val = min_values[more_variant_axis];
    *max_val = max_values[more_variant_axis];
    return more_variant_axis;
  }

  Scalar L2Distance2(const Scalar *p, const Scalar *q) const {
    Scalar distance = 0;
    for (int i = 0; i < num_dims_; ++i) {
      Scalar diff = p[i] - q[i];
      distance += diff * diff;
    }
    return distance;
  }

 private:
  std::vector<KdNode> nodes_;
  std::vector<Point> points_;
  int num_dims_;
  int num_levels_;
};

}  // namespace libmv

#endif // LIBMV_CORRESPONDENCE_KDTREE_H_
