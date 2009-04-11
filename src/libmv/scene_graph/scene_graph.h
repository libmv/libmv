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

/*
* Implements a simple scene graph structure.
*/

#ifndef LIBMV_SCENE_GRAPH_SCENE_GRAPH_H
#define LIBMV_SCENE_GRAPH_SCENE_GRAPH_H

#include <cstring>

#include <list>

#include <libmv/numeric/numeric.h>

using std::list;

namespace libmv {

template<class Object>
class SGNotRoot;

template<class Object>
class SGNotLeaf;

template<class Object>
class SGNode
{
 public:
  virtual SGNode<Object> *GetChild(const char *) {
    return NULL;
  }
  virtual SGNode<Object> *GetAtPath(const char *) {
    return NULL;
  }
  virtual SGNotLeaf<Object> *GetParent() {
    return NULL;
  }
  
  //AddChild and RemoveChild may result in reallocation of this,
  //the new node is returned from each function.
  //If you don't want reallocation use AddChildStable and RemoveChildStable
  //although they may fail.
  virtual SGNode *AddChild(SGNotRoot<Object> *) = 0;
  virtual SGNode *RemoveChild(SGNotRoot<Object> *) {
    return this;
  }
  
  virtual bool RemoveChildStable(SGNotRoot<Object> *) {
    return false;
  }
  virtual bool AddChildStable(SGNotRoot<Object> *) {
    return false;
  }
  
  virtual const char *GetName() const {
    return NULL;
  }
  virtual bool SetName(const char *) {
    return false;
  }
  
  virtual Mat4 GetMatrix() {
    Mat4 res;
    res.setIdentity();
    return res;
  }
  
  virtual void SetMatrix(Mat4) {}
  virtual void Transform(Mat4) {}
 protected:
  virtual void UpdateChildren() {}
  virtual void UpdateMatrix() {}
};

template<class Object>
class SGNotLeaf : public virtual SGNode<Object>
{
 public:
  SGNode<Object> *GetChild(const char *name);  
  SGNode<Object> *GetAtPath(const char *path) {
    //TODO(Daniel): actually check that this works
    assert(path);
    while(path[0] == '/')
    	path++;
    const char *next_path = strchr(path, '/');
    int size = strlen(path) - strlen(next_path);
    char *child_name = new char [size+1];
    strncpy(child_name, path, size);
    child_name[size]='\0';
    SGNode<Object> *child = GetChild(child_name);
    if(!child)
    	return NULL;
    SGNode<Object> *result = child->GetAtPath(next_path);
    delete [] child_name;
    return result;
  }
  
  void UpdateChildren();
  
  bool HasChild(SGNotRoot<Object> *node) {
    typename list<SGNotRoot<Object> *>::iterator it;
    for(it=children_.begin(); it!=children_.end(); ++it)
      if(*it == node)
      	return true;
    return false;
  }
  
  int NumChildren() {
    return children_.size();
  }
  bool HasChildren() {
    return !children_.empty();
  }
  
  bool RemoveChildStable(SGNotRoot<Object> *node);
  bool AddChildStable(SGNotRoot<Object> *node);
  
  ~SGNotLeaf();
 private:
  list<SGNotRoot<Object> *> children_;
};

template<class Object>
class SGNotRoot : public virtual SGNode<Object>
{
 public:
  SGNotLeaf<Object> *GetParent() {
    return parent_;
  }
  const char *GetName() const {
    return name_;
  }
  
  bool SetName(const char *name) {
    int i;
    for(i=0; i<strlen(name); i++) {
    	if(!((name[i]>'0'&&name[i]<'9')||
    	     (name[i]>'a'&&name[i]<'z')||
    	     (name[i]>'A'&&name[i]<'Z')||
    	     name[i]==' ')) {
    	  name_=NULL;
    	  return false;
    	}
    }
    name_ = new char[strlen(name)+1];
    strcpy(name_, name);
    return true;
  }
  
  Mat4 GetMatrix() {
    return current_;
  }
  
  void SetMatrix(Mat4 mat) {
    transform_ = mat;
    UpdateMatrix();
  }
  
  void Transform(Mat4 mat) {
    transform_ = transform_ * mat;
    UpdateMatrix();
  }
  
  void SetParent(SGNotLeaf<Object> *node) {
    assert(parent_->HasChild(this));
    parent_ = node;
    UpdateMatrix();
  }
  
  void UpdateMatrix() {
    assert(parent_);
    current_ = parent_->GetMatrix() * transform_;
    SGNode<Object>::UpdateChildren();
  }
  
  void SetObject(Object &obj) {
    obj_ = obj;
  }
  
  Object GetObject() {
    return obj_;
  }
  
  ~SGNotRoot() {
    delete [] name_;
  }
  
  SGNotRoot() {
    name_ = NULL;
    parent_ = NULL;
    transform_.setIdentity();
    current_ = transform_;
  }
 protected:  
  SGNotLeaf<Object> *parent_;
  char *name_;
  
  Mat4 transform_; // This holds the transform incurred by this object.
  Mat4 current_; // This holds the transform of this object and all parents.
  
  Object obj_;
};

template<class Object>
class SGRootNode : public SGNotLeaf<Object>
{
 public:
  SGNode<Object> *AddChild(SGNotRoot<Object> *node) {
    SGNotLeaf<Object>::AddChildStable(node);
    return this;
  }
  SGNode<Object> *RemoveChild(SGNotRoot<Object> *node) {
    SGNotLeaf<Object>::RemoveChildStable(node);
    return this;
  }
};

template<class Object>
class SGBranchNode : public SGNotLeaf<Object>, public SGNotRoot<Object>
{
 public:
  SGNode<Object> *AddChild(SGNotRoot<Object> *node) {
    SGNotLeaf<Object>::AddChildStable(node);
    return this;
  }
  SGNode<Object> *RemoveChild(SGNotRoot<Object> *node);
  
  SGBranchNode(SGNotLeaf<Object> *l) : SGNotLeaf<Object>(*l) {}
  SGBranchNode(SGNotRoot<Object> *r) : SGNotRoot<Object>(*r) {}
  SGBranchNode() {}
};

template<class Object>
class SGLeafNode : public SGNotRoot<Object>
{
 public:
  SGNode<Object> *AddChild(SGNotRoot<Object> *node) {
    SGBranchNode<Object> *branch = new SGBranchNode<Object>((SGNotRoot<Object> *)this);
    branch->AddChild(node);
    SGNotRoot<Object>::parent_->RemoveChildStable(this);
    SGNotRoot<Object>::parent_->AddChildStable(branch);
    delete this;
    return branch;
  }
  SGNode<Object> *RemoveChild(SGNotRoot<Object> *) {
    return this;
  }
  
  SGLeafNode() {}
  SGLeafNode(SGNotRoot<Object> *r) : SGNotRoot<Object>(*r) {}
};

template<class Object>
class SceneGraph
{
 public:
  SGNode<Object> *GetAtPath(const char *path) {
    return root_.GetAtPath(path);
  }
 private:
  SGRootNode<Object> root_;
};

template<class Object>
SGNode<Object> *SGNotLeaf<Object>::GetChild(const char *name) {
  typename list<SGNotRoot<Object> *>::iterator it;
  for(it=children_.begin(); it!=children_.end(); ++it)
    if(!strcmp(name, (*it)->GetName()))
      return *it;
  return NULL;
}

template<class Object>
void SGNotLeaf<Object>::UpdateChildren() {
  typename list<SGNotRoot<Object> *>::iterator it;
  for(it=children_.begin(); it!=children_.end(); ++it)
    (*it)->UpdateMatrix();
}

template<class Object>
SGNotLeaf<Object>::~SGNotLeaf() {
  typename list<SGNotRoot<Object> *>::iterator it;
  for(it=children_.begin(); it!=children_.end(); ++it)
    delete *it;
}

template<class Object>
bool SGNotLeaf<Object>::AddChildStable(SGNotRoot<Object> *node) {
  children_.push_back(node);
  node->SetParent(this);
  return true;
}

template<class Object>
bool SGNotLeaf<Object>::RemoveChildStable(SGNotRoot<Object> *node) {
  children_.remove(node);
  node->SetParent(NULL);
  return true;
}

template<class Object>
SGNode<Object> *SGBranchNode<Object>::RemoveChild(SGNotRoot<Object> *node) {
  SGNotLeaf<Object>::RemoveChildStable(node);
  if(!SGNode<Object>::HasChildren())
  {
    SGLeafNode<Object> *leaf = new SGLeafNode<Object>((SGNotRoot<Object> *)this);
    SGNotRoot<Object>::parent_->RemoveChild(this);
    SGNotRoot<Object>::parent_->AddChild(leaf);
    delete this;
    return leaf;
  }
  return this;
}

} //namespace libmv

#endif
