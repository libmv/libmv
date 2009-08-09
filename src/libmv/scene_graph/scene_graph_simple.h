// Copyright (c) 2009 libmv authors.
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

#ifndef LIBMV_SCENE_GRAPH_SCENE_GRAPH_SIMPLE_H
#define LIBMV_SCENE_GRAPH_SCENE_GRAPH_SIMPLE_H

#include <map>
#include <iostream>

#include "libmv/numeric/numeric.h"
#include "libmv/logging/logging.h"

namespace libmv {
namespace scene {

using std::string;

/**
 * The simplest possible scene graph that could possibly work for displaying a
 * two view reconstruction. Performance is utterly ignored in favor of code
 * simplicity and interface ease of use.
 */
template<class Object>
class Node {
  typedef Node<Object> NodeT;
  typedef std::map<string, NodeT *> ChildMap;
 public:
  virtual ~Node() {
    typename ChildMap::iterator it;
    for (it = children_.begin(); it != children_.end(); ++it) {
      delete it->second;
    }
    SetParent(NULL);
  }
  
  Node() : parent_(NULL), object_(NULL) {
    transform_ = Mat4::Identity(4, 4);
  }
  Node(const string &name, Object *object) : parent_(NULL) {
    name_ = name;
    object_ = object;
    transform_ = Mat4::Identity(4, 4);
  }

  NodeT *GetParent() const { return parent_; }
  void SetParent(NodeT *new_parent) {
    if (new_parent == parent_)
      return;
    if (parent_ != NULL) {
      parent_->RemoveChild(this);
    }
    parent_ = new_parent;
    if (new_parent != NULL) {
      new_parent->AddChild(this);
    }
  }

  NodeT *GetChild(const string &child_name) const {
    typename ChildMap::const_iterator it = children_.find(child_name);
    return (it == children_.end()) ? NULL : it->second;
  }

  void AddChild(NodeT *child) {
    assert(child);
    assert(!child->GetName().empty());
    // Children must have unique names.
    NodeT *existing_child = GetChild(child->GetName());
    if (existing_child == NULL) {
      children_[child->GetName()] = child;
      child->SetParent(this);
    } else if (child == existing_child) {
      LOG(WARNING) << "Ignoring attempt to add child node <" << child->GetName()
                   << "> to parent node <" << GetName() << " multiple times";
    } else {
      LOG(WARNING) << "Ignoring attempt to add child node <" << child->GetName()
                   << "> to parent node <" << GetName()
                   << "> which already has a node with this name "
                   << "(which doesn't match the node to be added";
    }
  }

  void RemoveChild(NodeT *child) {
    typename ChildMap::iterator it = children_.find(child->GetName());
    if (it != children_.end()) {
      assert(it->second == child);
      children_.erase(it);
    }
  }
  
  bool HasChild(NodeT *node) {
    return GetChild(node->GetName()) == node;
  }

  const string &GetName() const    { return name_; }
  void SetName(const string &name) {
    if (name == name_)
      return;
    if (parent_) {
      parent_->RemoveChild(this);
      name_ = name;
      parent_->AddChild(this);
    } else {
      name_ = name;
    }
  }

  const Mat4 &GetTransform()  const { return transform_; }

  Object *GetObject() const { return object_; }
  void SetObject(Object *object) { object_ = object; }
  void DeleteObject() {
    if (object_) {
      delete object_;
      object_ = NULL;
    }
  }
  
  // The transform to move from the coordinate space of parent to the
  // coordinate space of this object.
  void SetTransform(const Mat4 &transform) {
    transform_ = transform;
  }
  void TransformBy(const Mat4 &transform) {
    transform_ = transform_ * transform;
  }

  class iterator {
    friend class Node<Object>;
   public:
    NodeT &operator*() {
      return *it_->second;
    }
    NodeT *operator->() {
      return it_->second;
    } 
    bool operator!=(const iterator &other) {
      return it_ != other.it_;
    }
    bool operator==(const iterator &other) {
      return it_ == other.it_;
    }
    void operator++() {
      ++it_;
    }
    iterator() {}
   private:
    iterator(const typename ChildMap::iterator &it) {
      it_ = it;
    }
    typename ChildMap::iterator it_;
  };
  // Iterate over the children of this node.
  iterator begin() {
    return iterator(children_.begin());
  }
  iterator end() {
    return iterator(children_.end());
  }
  
  iterator erase(iterator position) {
    iterator next = position;
    ++next;
    children_.erase(position.it_);
    return next;
  }
  
  unsigned int NumChildren() {
    return children_.size();
  }
  unsigned int NumChildrenRecursive() {
    unsigned int result = 0;
    typename ChildMap::iterator it;
    for (it = children_.begin(); it != children_.end(); ++it) {
      ++result;
      result += it->second->NumChildrenRecursive();
    }
    return result;
  }
  
  bool HasChildren() {
    return !children_.empty();
  }

  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

 private:

  // Transform from the parents space to this object's space.
  Mat4 transform_;
  string name_;
  NodeT *parent_;
  ChildMap children_;
  Object *object_;
};

}  // namespace scene
}  // namespace libmv

#endif  // LIBMV_SCENE_GRAPH_SCENE_GRAPH_SIMPLE_H
