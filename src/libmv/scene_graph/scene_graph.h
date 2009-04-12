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

#include "libmv/numeric/numeric.h"
#include "libmv/logging/logging.h"

using std::list;

namespace libmv {

template<class Object>
class SGNotRoot;

template<class Object>
class SGNotLeaf;

/*
* All nodes are derived from SGNode,
* Along with SceneGraph It should/will contain all the functionality needed
* from outside of this library.
*/
template<class Object>
class SGNode {
 public:
  virtual SGNode<Object> *GetChild(const char *) {
    return NULL;
  }
  // Get object in the form
  // "/ChildOfRoot/ChildOfChildOfRoot/ChildOfChildOfChildOfRoot"
  // Don't end in a trailing slash
  virtual SGNode<Object> *GetAtPath(const char *) {
    return NULL;
  }
  virtual char *GetPath() {
    return NULL;
  }
  virtual SGNode<Object> *GetParent() {
    return NULL;
  }
  
  // AddChild and RemoveChild may result in reallocation of the node,
  // the new node is returned from each function.
  // ie Usage is: some_node = some_node->AddChild(MakeSGNode(obj, "name"));
  // The node will still have the same parents after adding children.
  // If you don't want reallocation use AddChildStable and RemoveChildStable,
  // although they may fail if for example if you try to add children to a leaf node.
  virtual SGNode *AddChild(SGNode<Object> *) = 0;
  virtual SGNode *RemoveChild(SGNode<Object> *) {
    return this;
  }
  
  virtual bool RemoveChildStable(SGNode<Object> *) {
    return false;
  }
  virtual bool AddChildStable(SGNode<Object> *) {
    return false;
  }
  
  virtual const char *GetName() const {
    return NULL;
  }
  // SetName copies from the argument,
  // so if you provide a heap ptr remember to delete or free it later
  virtual bool SetName(const char *) {
    return false;
  }
  
  // Returns the matrix for the object which includes effects from parents.
  virtual Mat4 GetMatrix() {
    Mat4 res;
    res.setIdentity();
    return res;
  }
  
  // Returns the matrix for the object which excludes effects from parents.
  virtual Mat4 GetObjectMatrix() {
    Mat4 res;
    res.setIdentity();
    return res;
  }
  
  virtual bool HasChild(SGNotRoot<Object> *) {
    return false;
  }
  // Prefer HasChildren over NumChildren() if possible,
  // HasChildren is significantly faster over large structures.
  virtual bool HasChildren() {
    return false;
  }
  virtual int NumChildren() {
    return 0;
  }
  virtual int NumChildrenRecursive() {
    return 0;
  }
  
  virtual void ForeachChild(void (*)(SGNotRoot<Object> *, void *), void *) {}
  
  virtual Object *GetObject() {
    return NULL;
  }
  virtual void SetObject(Object *) {}
  
  // Set the matrix which describes the transformation incurred by this object
  virtual void SetMatrix(Mat4) {}
  virtual void Transform(Mat4) {}
  
  virtual ~SGNode() {}
 protected:
  virtual void UpdateChildren() {}
  // Called when an operation changes the transformation matrix.
  virtual void UpdateMatrix() {}
 protected:
  virtual bool SetParent(SGNotLeaf<Object> *) {
    return false;
  }
  friend class SGNotLeaf<Object>;
};

template<class Object>
class SGNotLeaf : public virtual SGNode<Object> {
 public:
  SGNode<Object> *GetChild(const char *name);  
  SGNode<Object> *GetAtPath(const char *path) {
    assert(path);
    while(path[0] == '/')
    	path++;
    const char *next_path = strchr(path, '/');
    int next_size;
    if (next_path)
      next_size = strlen(next_path);
    else
      next_size = 0;
    int size = strlen(path) - next_size;
    char *child_name = new char [size+1];
    strncpy(child_name, path, size);
    child_name[size]='\0';
    SGNode<Object> *child = GetChild(child_name);
    if (!child)
      return NULL;
    SGNode<Object> *result;
    if (next_path)
      result = child->GetAtPath(next_path);
    else
      result = child;
    delete [] child_name;
    return result;
  }
  
  void ForeachChild(void (*func)(SGNode<Object> *, void *), void *data) {
    typename list<SGNode<Object> *>::iterator it;
    for (it=children_.begin(); it!=children_.end(); ++it)
      func(*it, data);
  }
  
  void UpdateChildren();
  
  bool HasChild(SGNode<Object> *node) {
    typename list<SGNode<Object> *>::iterator it;
    for (it=children_.begin(); it!=children_.end(); ++it)
      if (*it == node)
      	return true;
    return false;
  }
  int NumChildrenRecursive() {
    int res=0;
    typename list<SGNode<Object> *>::iterator it;
    for (it=children_.begin(); it!=children_.end(); ++it) {
      res++;
      res += (*it)->NumChildrenRecursive();
    }
    return res;
  }
  
  int NumChildren() {
    return children_.size();
  }
  bool HasChildren() {
    return !children_.empty();
  }
  
  bool RemoveChildStable(SGNode<Object> *node);
  bool AddChildStable(SGNode<Object> *node);
  
  SGNotLeaf() {}
  // This is not a copy constructor! It moves the children of l to this.
  SGNotLeaf(SGNotLeaf &l) : SGNode<Object>(*(SGNode<Object> *)&l) {
    children_ = l.children_;
    l.children_.clear();
    assert(l.children_.empty());
  }
  virtual ~SGNotLeaf();
 private:
  list<SGNode<Object> *> children_;
};

template<class Object>
class SGNotRoot : public virtual SGNode<Object> {
 public:
  SGNode<Object> *GetParent() {
    return parent_;
  }
  const char *GetName() const {
    return name_;
  }
  
  char *GetPath() {
    assert(parent_);
    char *parent_path = parent_->GetPath();
    int len;
    if (parent_path)
      len = strlen(parent_path);
    else
      len = 0;
    char *res = new char [len + strlen(name_) + 2];
    if (parent_path)
      strcpy(res, parent_path);
    res[len] = '/';
    strcpy(&res[len+1], name_);
    delete [] parent_path;
    return res;
  }
  
  bool SetName(const char *name) {
    int i;
    for (i=0; i<strlen(name); i++) {
    	if (!((name[i]>='0'&&name[i]<='9')||
    	     (name[i]>='a'&&name[i]<='z')||
    	     (name[i]>='A'&&name[i]<='Z')||
    	     name[i]==' ')) {
    	  name_=NULL;
    	  return false;
    	}
    }
    if (parent_)
      if (parent_->GetChild(name) != NULL)
        return false;
    if (name_)
    	delete name_;
    name_ = new char[strlen(name)+1];
    strcpy(name_, name);
    return true;
  }
  
  Mat4 GetMatrix() {
    return current_;
  }
  Mat4 GetObjectMatrix() {
    return transform_;
  }
  
  void SetMatrix(Mat4 mat) {
    transform_ = mat;
    UpdateMatrix();
  }
  
  void Transform(Mat4 mat) {
    transform_ = transform_ * mat;
    UpdateMatrix();
  }
  
  void UpdateMatrix() {
    if (parent_) {
      current_ = parent_->GetMatrix() * transform_;
      this->UpdateChildren();
    }
  }
  
  Object *GetObject() {
    return obj_;
  }
  void SetObject(Object *obj) {
    obj_ = obj;
  }
  
  virtual ~SGNotRoot() {
    delete [] name_;
    delete obj_;
    if (parent_)
      parent_->RemoveChildStable(this);
  }
  
  SGNotRoot() {
    name_ = NULL;
    parent_ = NULL;
    transform_.setIdentity();
    current_ = transform_;
  }
  // This is not a copy constructor! r's name, parent and object
  // are taken and given to this.
  SGNotRoot(SGNotRoot<Object> &r) : SGNode<Object>(*(SGNode<Object> *)&r) {
    parent_ = r.parent_;
    r.parent_ = NULL;
    name_ = r.name_;
    r.name_ = NULL;
    transform_ = r.transform_;
    current_ = r.current_;
    obj_ = r.obj_;
    r.obj_ = NULL;
    
    if (parent_) {
      parent_->RemoveChildStable(&r);
      parent_->AddChildStable(this);
    }
  }
 protected:  
  SGNode<Object> *parent_;
  char *name_;
  
  Mat4 transform_; // This holds the transform incurred by this object.
  Mat4 current_; // This holds the transform of this object and all parents.
  
  Object *obj_;
 private:
  bool SetParent(SGNotLeaf<Object> *node) {
    parent_ = node;
    UpdateMatrix();
    return true;
  }
  friend class SGNotLeaf<Object>;
};

template<class Object>
class SGRootNode : public SGNotLeaf<Object> {
 public:
  SGNode<Object> *AddChild(SGNode<Object> *node) {
    this->AddChildStable(node);
    return this;
  }
  SGNode<Object> *RemoveChild(SGNode<Object> *node) {
    this->RemoveChildStable(node);
    return this;
  }
};

template<class Object>
class SGBranchNode : public SGNotLeaf<Object>, public SGNotRoot<Object> {
 public:
  SGNode<Object> *AddChild(SGNode<Object> *node) {
    this->AddChildStable(node);
    return this;
  }
  SGNode<Object> *RemoveChild(SGNode<Object> *node);
  
  SGBranchNode(SGNotLeaf<Object> *l) : SGNotLeaf<Object>(*l) {}
  SGBranchNode(SGNotRoot<Object> *r) : SGNotRoot<Object>(*r) {}
  SGBranchNode() {}
};

template<class Object>
class SGLeafNode : public SGNotRoot<Object> {
 public:
  SGNode<Object> *AddChild(SGNode<Object> *node) {
    SGBranchNode<Object> *branch =
      new SGBranchNode<Object>(static_cast<SGNotRoot<Object> *>(this));
    delete this;
    branch->AddChild(node);
    return branch;
  }
  SGNode<Object> *RemoveChild(SGNode<Object> *) {
    return this;
  }
  
  SGLeafNode() {}
  SGLeafNode(SGNotRoot<Object> *r) : SGNotRoot<Object>(*r) {}
};

template<class Object>
SGNode<Object> *SGNotLeaf<Object>::GetChild(const char *name) {
  typename list<SGNode<Object> *>::iterator it;
  for (it=children_.begin(); it!=children_.end(); ++it) {
    const char *child_name = (*it)->GetName();
    if (child_name)
      if (!strcmp(name, child_name))
        return *it;
  }
  return NULL;
}

template<class Object>
void SGNotLeaf<Object>::UpdateChildren() {
  typename list<SGNode<Object> *>::iterator it;
  for (it=children_.begin(); it!=children_.end(); ++it)
    (*it)->UpdateMatrix();
}

template<class Object>
SGNotLeaf<Object>::~SGNotLeaf() {
  typename list<SGNode<Object> *>::iterator it;
  for (it=children_.begin(); it!=children_.end(); ++it) {
    (*it)->SetParent(NULL);
    delete *it;
  }
  children_.clear();
}

template<class Object>
bool SGNotLeaf<Object>::AddChildStable(SGNode<Object> *node) {
  assert(node);
  assert(node->GetName());
  // Prevent having 2 children with the same name
  // node will not be reallocated so node->GetName()
  // will reveal whether the name has changed.
  const char *name = node->GetName();
  char *name_new = new char [strlen(name)+1];
  strcpy(name_new, name);
  while (this->GetChild(name_new)) {
    char *prev = name_new;
    name_new = new char [strlen(prev) + 2];
    strcpy(name_new, prev);
    name_new[strlen(prev) + 1] = '+';
    name_new[strlen(prev) + 2] = '\0';
    delete prev;
  }
  node->SetName(name_new);
  delete name_new;
  
  children_.push_back(node);
  if (!node->SetParent(this)) {
    this->RemoveChildStable(node);
    return false;
  }
  return true;
}

template<class Object>
bool SGNotLeaf<Object>::RemoveChildStable(SGNode<Object> *node) {
  assert(node);
  children_.remove(node);
  node->SetParent(NULL);
  return true;
}

template<class Object>
SGNode<Object> *SGBranchNode<Object>::RemoveChild(SGNode<Object> *node) {
  SGNotLeaf<Object>::RemoveChildStable(node);
  if (!this->HasChildren()) {
    SGLeafNode<Object> *leaf =
       new SGLeafNode<Object>(static_cast<SGNotRoot<Object> *>(this));
    delete this;
    return leaf;
  }
  return this;
}

template<class Object>
class SceneGraph : public SGRootNode<Object> {
 public:
};

template<class Object>
SGLeafNode<Object> *MakeSGNode(Object *obj, const char *name) {
  SGLeafNode<Object> *leaf = new SGLeafNode<Object>;
  if (!leaf->SetName(name))
    LOG(ERROR) << "couldn't set name: " << name;
  else
    assert(leaf->GetName());
  leaf->SetObject(obj);
  return leaf;
}

} // namespace libmv

#endif
