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

namespace libmv {


// A simple Kd-tree for fast approximate nearest neighbor search.
template <typename Point, typename Scalar=double>
class KdTree {

  struct KdNode {
    int axis;
    Scalar value;
    Point *begin;
    Point *end;
    KdNode *left_child;
    KdNode *right_child;

    bool IsLeaf() {
      return left_child == NULL;
    }
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
  void Build(Point *points, int n, int k) {
    nodes_.clear();
    k_ = k;
    CreateNode(points, points + n);
  }

  int NumNodes() {
    return nodes_.size();
  }

  void PrintNodes() {
    for (int i = 0; i < nodes_.size(); ++i) {
      printf("Node %d: axis=%d  value=%g  num_points=%d\n",
             i, nodes_[i].axis, nodes_[i].value, nodes_[i].end - nodes_[i].begin);
    }
  }


 private:
  KdNode *CreateNode(Point *begin, Point *end) {
    int num_points = end - begin;

    if (num_points <= 0) { return NULL; }

    // Create the node.
    nodes_.resize(nodes_.size() + 1);
    KdNode &node = nodes_[nodes_.size() - 1];
    node.begin = begin;
    node.end = end;

    if (num_points == 1) {
      node.left_child = node.right_child = NULL; 
    } else {
      // Partition points.
      node.axis = MoreVariantAxis(begin, end);
      AxisComparison comp(node.axis);
      Point *pivot = begin + num_points / 2;
      std::nth_element(begin, pivot, end, comp);
      node.value = (*pivot)[node.axis];

      // Create sub-trees.
      node.left_child = CreateNode(begin, pivot);
      node.right_child = CreateNode(pivot, end);
    } 
    return &node;
  }

  int MoreVariantAxis(Point *begin, Point *end) {
    // Compute variances.
    Vec mean = Vec::Zero(k_);
    Vec mean2 = Vec::Zero(k_);
    for (Point *p = begin; p < end; ++p) {
      for (int i = 0; i < k_; ++i) {
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

 private:
  std::vector<KdNode> nodes_;
  int k_;
};


}  // namespace libmv


#endif // LIBMV_CORRESPONDENCE_KDTREE_H_
