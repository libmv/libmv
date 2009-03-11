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

#ifndef LIBMV_CORRESPONDENCE_BIPARTITE_GRAPH_H_
#define LIBMV_CORRESPONDENCE_BIPARTITE_GRAPH_H_

#include <map>
#include <set>
#include <cassert>

namespace libmv {

// A bipartite graph with labelled edges.
template<class LeftT, class EdgeT, class RightT>
class BipartiteGraph {
 public:
  typedef std::pair<LeftT, RightT> LeftRight;
  typedef std::map<EdgeT, LeftRight> EdgeMap;
  typedef std::map<LeftT,  std::map<RightT, EdgeT> > LeftToRightMap;
  typedef std::map<RightT, std::map<LeftT,  EdgeT> > RightToLeftMap;
  typedef BipartiteGraph<LeftT, EdgeT, RightT> GraphT;

  int NumEdges()      const { return edges_.size(); }
  int NumLeftNodes()  const { return left_to_right_.size(); }
  int NumRightNodes() const { return right_to_left_.size(); }

  void Insert(const LeftT left, const EdgeT edge, const RightT right) {
    // TODO(keir): Consider making non-unique edges illegal.
    edges_[edge] = LeftRight(left, right);
    left_to_right_[left][right] = edge;
    right_to_left_[right][left] = edge;
  }

  // Delete the edge connecting left and right.
  void DeleteEdge(const LeftT left, const RightT right) {
    EdgeT edge;
    assert(GetEdge(left, right, &edge));
    edges_.erase(edge);
    left_to_right_[left].erase(right);
    right_to_left_[right].erase(left);
  }
  void DeleteRight(const RightT right) { assert(0); } // XXX
  void DeleteLeft(const LeftT left) { assert(0); } // XXX

  bool GetEdge(const LeftT left, const RightT right, EdgeT *edge) const {
    typename LeftToRightMap::const_iterator lt = left_to_right_.find(left);
    if (lt == left_to_right_.end()) {
      return false;
    }
    const std::map<RightT, EdgeT> &right_to_edge = lt->second;
    const typename std::map<RightT, EdgeT>::const_iterator rt =
        right_to_edge.find(right);
    if (rt == right_to_edge.end()) {
      return false;
    }
    *edge = rt->second;
    return true;
  }
  
  class LeftEdgeIterator;
  class RightEdgeIterator;
  
  // Iterate over all the edges in the graph.
  // TODO(keir): Is this iterator even useful?
  class EdgeIterator {
   friend class BipartiteGraph<LeftT, EdgeT, RightT>;
   public:
    LeftT         left()       const { return iter_->second.first; }
    RightT        right()      const { return iter_->second.second; }
    EdgeT         edge()       const { return iter_->first; }
    EdgeIterator  operator++()       { return iter_++; }
    bool operator==(const EdgeIterator &other) {
      return iter_ == other.iter_;
    }
   private:
    EdgeIterator(typename EdgeMap::const_iterator iter) : iter_(iter) {}
    typename EdgeMap::const_iterator iter_;
  };
  EdgeIterator EdgesBegin() const { return EdgeIterator(edges_.begin()); }
  EdgeIterator EdgesEnd()   const { return EdgeIterator(edges_.end()); }

  // For iterating over the left nodes.
  template<typename NodeT, typename IteratorT, typename EdgeIteratorT>
  class NodeIterator {
   friend class BipartiteGraph<LeftT, EdgeT, RightT>;
   public:
    NodeT operator*()  const { return iter_->first; }
    void operator++()        { iter_++; }
    bool operator!=(const NodeIterator &it) const { return iter_ != it.iter_; }

    EdgeIteratorT begin() const {
      return EdgeIteratorT(**this, iter_->second.begin());
    }
    EdgeIteratorT end() const {
      return EdgeIteratorT(**this, iter_->second.end());
    }

   private:
    NodeIterator(IteratorT it) : iter_(it) {}
    IteratorT iter_;
  };

  // For iterating over the left nodes.
  typedef NodeIterator<LeftT,
                       typename LeftToRightMap::const_iterator,
                       LeftEdgeIterator> LeftIterator;

  // Iterates over the edges incident to a left node.
  class LeftEdgeIterator {
   friend class NodeIterator<LeftT,
                             typename LeftToRightMap::const_iterator,
                             LeftEdgeIterator>;
   public:
    LeftT  left()        const { return left_;         }
    RightT right()       const { return iter_->first;  }
    EdgeT  edge()        const { return iter_->second; }
    void operator++() { iter_++; }
    bool operator==(const LeftEdgeIterator &other) const {
      return iter_ == other.iter_;
    }
    bool operator!=(const LeftEdgeIterator &other) const {
      return iter_ != other.iter_;
    }
   private:
    LeftEdgeIterator(LeftT left,
                     typename std::map<RightT, EdgeT>::const_iterator iter)
        : left_(left), iter_(iter) {}
    LeftT left_;
    typename std::map<RightT, EdgeT>::const_iterator iter_;
  };

  LeftIterator LeftBegin() const {
    return LeftIterator(left_to_right_.begin());
  }
  LeftIterator LeftEnd() const {
    return LeftIterator(left_to_right_.end());
  }
  LeftIterator FindLeft(const LeftT &left_node) {
    return LeftIterator(left_to_right_.find(left_node));
  }

  // For iterating over the right nodes.
  typedef NodeIterator<RightT,
                       typename RightToLeftMap::const_iterator,
                       RightEdgeIterator> RightIterator;

  RightIterator RightBegin() const {
    return RightIterator(right_to_left_.begin());
  }
  RightIterator RightEnd() const {
    return RightIterator(right_to_left_.end());
  }
  RightIterator FindRight(const RightT &right_node) {
    return RightIterator(right_to_left_.find(right_node));
  }

  // Iterates over the edges incident to a right node.
  class RightEdgeIterator {
   friend class NodeIterator<RightT,
                             typename RightToLeftMap::const_iterator,
                             RightEdgeIterator>;
   public:
    RightT right()     const { return right_;        }
    LeftT  left()      const { return iter_->first;  }
    EdgeT  edge()      const { return iter_->second; }
    void operator++()        { iter_++;              }
    bool operator==(const RightEdgeIterator &other) const {
      return iter_ == other.iter_;
    }
    bool operator!=(const RightEdgeIterator &other) const {
      return iter_ != other.iter_;
    }
   private:
    RightEdgeIterator(RightT right,
                      typename std::map<LeftT, EdgeT>::const_iterator iter)
        : right_(right), iter_(iter) {}
    RightT right_;
    typename std::map<LeftT, EdgeT>::const_iterator iter_;
  };

 private:
  EdgeMap edges_;
  LeftToRightMap left_to_right_;
  RightToLeftMap right_to_left_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_BIPARTITE_GRAPH_H_
