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
  typedef std::pair<RightNode, LeftNode> RightToLeft;
  typedef LeftToRight EdgeKey;
	typedef std::map<EdgeKey, Edge> EdgeMap;

	int NumEdges() {
		return edges_.size();
	}
	int NumLeftNodes() {
		return CountMultiMapKeys(left_to_right_);
	}
	int NumRightNodes() {
		return CountMultiMapKeys(right_to_left_);
	}
	void Insert(const LeftNode left, const RightNode right, const Edge edge) {
		edges_[EdgeKey(left, right)] = edge;
		left_to_right_.insert(LeftToRight(left, right));
		right_to_left_.insert(RightToLeft(right, left));
	}
	Edge GetEdge(const LeftNode left, const RightNode right) {
		EdgeKey key(left, right);
		assert(edges_.count(key) == 1);
		return edges_[key];
	}
  // TODO(keir): Write iterator over edges.
  // TODO(keir): Write iterator over left nodes
  // TODO(keir): Write iterator over right nodes
  // TODO(keir): Write iterator over left-nodes-for-one-right-node.
  // TODO(keir): Write iterator over right-nodes-for-one-left-node.
private:
	template<typename Z,typename X>
	int CountMultiMapKeys(std::multimap<Z,X> &ZtoX) {
    // TODO(keir): Fix this painfully stupid implementation which I shamefully
    // wrote..
		std::set<Z> tmp;
		typename std::multimap<Z,X>::iterator p;
		for (p = ZtoX.begin(); p != ZtoX.end(); ++p) {
			tmp.insert(p->first);
		}
		return tmp.size();
	}

	EdgeMap edges_;
  std::multimap<LeftNode, RightNode> left_to_right_;
  std::multimap<RightNode, LeftNode> right_to_left_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_BIPARTITE_GRAPH_H_
