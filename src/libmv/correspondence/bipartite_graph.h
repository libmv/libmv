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
// TODO(keir): Finish this class.
template<class LeftNode, class Edge, class RightNode>
class BipartiteGraph {
 public:
  typedef std::pair<LeftNode, RightNode> LeftToRight;
  typedef LeftToRight EdgeKey;
  typedef std::map<EdgeKey, Edge> EdgeMap;

  typedef std::pair<RightNode, Edge> RightEdge;
  typedef std::pair<LeftNode, Edge> LeftEdge;

  typedef std::map<LeftNode, std::set<RightEdge> > LeftToRightMap;
  typedef std::map<RightNode, std::set<LeftEdge> > RightToLeftMap;

  int NumEdges() const {
    return edges_.size();
  }
  int NumLeftNodes() const {
    return left_to_right_.size();
  }
  int NumRightNodes() const {
    return right_to_left_.size();
  }
  void Insert(const LeftNode left, const RightNode right, const Edge edge) {
    edges_[EdgeKey(left, right)] = edge;
    left_to_right_[left].insert(RightEdge(right, edge));
    right_to_left_[right].insert(LeftEdge(left, edge));
  }
  Edge GetEdge(const LeftNode left, const RightNode right) const {
    EdgeKey key(left, right);
    typename EdgeMap::const_iterator it = edges_.find(key);
    assert(it != edges_.end());
    return it->second;
  }

  class EdgeIterator {
    friend class BipartiteGraph<LeftNode, Edge, RightNode>;
   public:
    LeftNode left() const {
      return iter_->first.first;
    }
    RightNode right() const {
      return iter_->first.second;
    }
    Edge edge() const {
      return iter_->second;
    }
    bool Done() {
      return iter_ == edges_->end();
    }
    void Next() {
      ++iter_;
    }
   private:
    typename EdgeMap::const_iterator iter_;
    const EdgeMap *edges_;
  };

  EdgeIterator EdgesIterator() const {
    EdgeIterator iterator;
    iterator.iter_ = edges_.begin();
    iterator.edges_ = &edges_;
    return iterator;
  }

  /*
  class RightToLeftIterator {
    friend class BipartiteGraph<LeftNode, Edge, RightNode>;
   public:
    LeftNode left() const {
      return iter_->first.first;
    }
    RightNode right() const {
      return iter_->first.second;
    }
    Edge edge() const {
      return iter_->second;
    }
    bool Done() {
      return iter_ == edgemap_.end();
    }
    void Next() {
      ++iter_;
    }
   private:
    typename EdgeMap::const_iterator iter_;
    EdgeMap &edgemap_;
  };

  RightIterator EdgesIterator() const {
    EdgeIterator iterator;
    iterator.iter_ = edges_.begin();
    iterator.edgemap_ = edgemap_;
    return iterator;
  }
  */

  // TODO(keir): Write iterator over left nodes
  // TODO(keir): Write iterator over right nodes
  // TODO(keir): Write iterator over left-nodes-for-one-right-node.
  // TODO(keir): Write iterator over right-nodes-for-one-left-node.
 private:

  EdgeMap edges_;
  LeftToRightMap left_to_right_;
  RightToLeftMap right_to_left_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_BIPARTITE_GRAPH_H_
