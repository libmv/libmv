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

  struct Entry {
    LeftNode left;
    Edge edge;
    RightNode right;
  };

  int NumEdges() const {
    return edges_.size();
  }

  int NumLeftNodes() const {
    return left_to_right_.size();
  }

  int NumRightNodes() const {
    return right_to_left_.size();
  }

  void Insert(const LeftNode left, const Edge edge, const RightNode right) {
    edges_[EdgeKey(left, right)] = edge;
    left_to_right_[left].insert(RightEdge(right, edge));
    right_to_left_[right].insert(LeftEdge(left, edge));
  }

  // Delete the edge connecting left and right.
  void DeleteEdge(const LeftNode left, const RightNode right) {
    assert(edges_.find(EdgeKey(left, right)) != edges_.end());
    Edge edge = edges_[EdgeKey(left, right)];
    edges_.erase(EdgeKey(left, right));
    left_to_right_[left].erase(RightEdge(right, edge));
    right_to_left_[right].erase(LeftEdge(left, edge));
    // TODO(keir): Figure out when to prune empty sets; erasing here could
    // invalidate iterators (calling Done() would crash).
    //
    //   if (left_to_right_[left].empty()) {
    //     left_to_right_.erase(left);
    //   }
    //   if (right_to_left_[right].empty()) {
    //     right_to_left_.erase(right);
    //   }
  }

  Edge GetEdge(const LeftNode left, const RightNode right) const {
    EdgeKey key(left, right);
    typename EdgeMap::const_iterator it = edges_.find(key);
    assert(it != edges_.end());
    return it->second;
  }

  // For iterating over the right nodes.
  // TODO(keir): Add a LeftIterator.
  class RightIterator {
    friend class BipartiteGraph<LeftNode, Edge, RightNode>;
    public:
    RightNode right() const { return right_iter_->first; }
    void Next()             { right_iter_++; }
    bool Done() const {
      return right_iter_ == graph_->right_to_left_.end();
    }
   private:
    typename RightToLeftMap::const_iterator right_iter_;
    BipartiteGraph<LeftNode, Edge, RightNode> *graph_;
  };

  enum IteratorType {
    OVER_LEFT_NODES,
    OVER_RIGHT_NODES,
    OVER_EDGES,
  };

  // Iterates over the edges in the graph. The iteration may be constrained to
  // the outgoing edges from nodes on either the left or the right.
  class Iterator {
    friend class BipartiteGraph<LeftNode, Edge, RightNode>;
   public:
    LeftNode left() const {
      return left_;
    }
    RightNode right() const {
      return right_;
    }
    Edge edge() const {
      return edge_;
    }
    bool Done() const {
      if (type_ == OVER_EDGES) {
        return iter_ == edges_->end();
      } else if (type_ == OVER_LEFT_NODES) {
        return left_iter_ == left_nodes_->end();
      } else if (type_ == OVER_RIGHT_NODES) {
        return right_iter_ == right_nodes_->end();
      } 
      assert(0);
      return true;
    }

    // Delete the current edge. Invalidates any other iterators pointing to
    // this particular edge. After a Delete() call, left(), right(), and edge()
    // have undefined results and calling Next() will advance the iterator to
    // the element after the deleted one. Example:
    //
    //   for(Iterator it = ...; !it.Done(); it.Next()) {
    //     ...
    //     // Do stuff with it.left(), it.right(), and it.edge() 
    //     ...
    //     if (delete_this_entry) {
    //       it.DeleteEdge();
    //       // it.left(), it.right(), and it.edge() are now undefined, until
    //       // the next iteration of the loop.
    //     }
    //   }
    //
    // This makes pruning edges in a graph straightforward.
    void DeleteEdge() {
      assert(!Done());
      assert(!deleted_);
      Advance();  // Preserves left_, right_, and edge_.
      graph_->DeleteEdge(left_, right_);
      deleted_ = true;
    }

    void Advance() {
      if (type_ == OVER_EDGES) {
        ++iter_;
      } else if (type_ == OVER_LEFT_NODES) {
        ++left_iter_;
      } else if (type_ == OVER_RIGHT_NODES) {
        ++right_iter_;
      } else {
        assert(0);
      }
    }

    void Next() {
      if (deleted_) {
        // Do nothing if the last element was deleted, because the iterators
        // have already been advanced by the delete.
        deleted_ = false;
      } else {
        Advance();
      }
      Assign();
    }
    void Assign() {
      if (type_ == OVER_EDGES) {
        left_ = iter_->first.first;
        right_ = iter_->first.second;
        edge_ = iter_->second;
      } else if (type_ == OVER_LEFT_NODES) {
        left_ = left_iter_->first;
        edge_ = left_iter_->second;
      } else if (type_ == OVER_RIGHT_NODES) {
        right_ = right_iter_->first;
        edge_ = right_iter_->second;
      } else {
        assert(0);
      }
    }
   private:

    // For iterating over all edges.
    typename EdgeMap::iterator iter_;
    const EdgeMap *edges_;

    // For iterating over edges connected to a particular left node.
    typename std::set<RightEdge>::iterator right_iter_;
    const std::set<RightEdge> *right_nodes_;
    
    // For iterating over edges connected to a particular right node.
    typename std::set<LeftEdge>::iterator left_iter_;
    const std::set<LeftEdge> *left_nodes_;
    
    IteratorType type_;
    LeftNode left_;
    RightNode right_;
    Edge edge_;

    // Was the last item deleted? If so, the iterator was already advanced and
    // it shouldn't be advanced at the next call to Next().
    bool deleted_;

    BipartiteGraph<LeftNode, Edge, RightNode> *graph_;
  };

  // TODO(keir): Define sensible constructors for the iterator.
  Iterator ScanAllEdges() {
    Iterator iterator;
    iterator.type_ = OVER_EDGES;
    iterator.iter_ = edges_.begin();
    iterator.Assign();
    iterator.edges_ = &edges_;
    iterator.deleted_ = false;
    iterator.graph_ = this;
    return iterator;
  }

  Iterator ScanEdgesForRightNode(const RightNode &right_node) {
    typename RightToLeftMap::iterator it = right_to_left_.find(right_node);
    assert(it != right_to_left_.end());
    Iterator iterator;
    iterator.type_ = OVER_LEFT_NODES;
    iterator.left_iter_ = it->second.begin();
    iterator.left_nodes_ = &it->second;
    iterator.right_ = right_node;
    iterator.deleted_ = false;
    iterator.Assign();
    iterator.graph_ = this;
    return iterator;
  }

  Iterator ScanEdgesForLeftNode(const LeftNode &left_node) {
    typename LeftToRightMap::const_iterator it = left_to_right_.find(left_node);
    assert(it != left_to_right_.end());
    Iterator iterator;
    iterator.type_ = OVER_RIGHT_NODES;
    iterator.right_iter_ = it->second.begin();
    iterator.right_nodes_ = &it->second;
    iterator.left_ = left_node;
    iterator.deleted_ = false;
    iterator.Assign();
    iterator.graph_ = this;
    return iterator;
  }

  RightIterator ScanRightNodes() {
    RightIterator iterator;
    iterator.right_iter_ = right_to_left_.begin();
    iterator.graph_ = this;
    return iterator;
  }

  // TODO(keir): Write iterator over left nodes
  // TODO(keir): Write iterator over right nodes
 private:

  EdgeMap edges_;
  LeftToRightMap left_to_right_;
  RightToLeftMap right_to_left_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_BIPARTITE_GRAPH_H_
