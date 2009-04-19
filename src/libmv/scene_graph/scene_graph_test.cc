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

#include <cstring>

#include "libmv/scene_graph/scene_graph.h"
#include "testing/testing.h"
#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

using libmv::SceneGraph;
using libmv::MakeSGNode;
using libmv::SGNode;
using libmv::Mat4;

struct TestStruct {
  int data_;
  float some_more_data_;
  TestStruct() : data_(0), some_more_data_(0) {}
};

TEST(SceneGraph, Paths) {
  SceneGraph<TestStruct> scene_graph;
  TestStruct *t0 = new TestStruct;
  SGNode<TestStruct> *n = MakeSGNode(t0, "child");
  scene_graph.AddChild(n);
  EXPECT_EQ(n->GetParent(), &scene_graph);
  EXPECT_EQ(scene_graph.NumChildren(), 1);
  SGNode<TestStruct> *node = scene_graph.GetAtPath("/child");
  EXPECT_EQ(node, n);
  EXPECT_EQ(node->GetObject(), t0);
  char *path = node->GetPath();
  EXPECT_FALSE(strcmp(path, "/child"));
  delete [] path;
  
  TestStruct *t1 = new TestStruct;
  SGNode<TestStruct> *child = MakeSGNode(t1, "childb");
  node = node->AddChild(child);
  EXPECT_EQ(child->GetParent(), node);
  EXPECT_EQ(scene_graph.NumChildren(), 1);
  EXPECT_EQ(node->NumChildren(), 1);
  path = child->GetPath();
  EXPECT_FALSE(strcmp(path, "/child/childb"));
  EXPECT_EQ(scene_graph.GetAtPath("/child/childb"), child);
  delete [] path;
}

TEST(SceneGraph, NoHangingPtrs) {
  SceneGraph<TestStruct> scene_graph;
  TestStruct *t0 = new TestStruct;
  scene_graph.AddChild(MakeSGNode(t0, "child"));
  EXPECT_EQ(scene_graph.NumChildren(), 1);
  SGNode<TestStruct> *n = scene_graph.GetChild("child");
  EXPECT_TRUE(n);
  delete n;
  EXPECT_EQ(scene_graph.NumChildren(), 0);
  EXPECT_FALSE(scene_graph.GetChild("child"));
}

TEST(SceneGraph, BigGraph) {
  SceneGraph<TestStruct> scene_graph;
  char name[2];
  name[1] = '\0';
  name[0] = 'a';
  int i;
  TestStruct data;
  for (i=0; i<20; i++) {
    scene_graph.AddChild(MakeSGNode(new TestStruct(data), name));
    name[0]++;
  }
  // TODO(Daniel): Finish this test and add more.
}

TEST(SceneGraph, MatrixTest) {
  Mat4 mat, ident;
  ident.setIdentity();
  mat << 6, 5, 3, 4,
         7, 3, 2, 1,
         5, 4, 3, 2,
         0, 0, 1, 4;
  TestStruct data;
  SceneGraph<TestStruct> scene_graph;
  scene_graph.GetObjectMatrix() = mat;
  scene_graph.UpdateMatrix();
  scene_graph.AddChild(MakeSGNode(new TestStruct(data), "child"));
  SGNode<TestStruct> *ptr = scene_graph.GetAtPath("/child");
  EXPECT_EQ(ptr->GetMatrix(), mat);
  EXPECT_EQ(ptr->GetObjectMatrix(), ident);
}

TEST(SceneGraph, Misc) {
  TestStruct data;
  SceneGraph<TestStruct> scene_graph;
  SGNode<TestStruct> *node = MakeSGNode(new TestStruct(data), "child");
  scene_graph.AddChild(node);
  node->AddChild(MakeSGNode(new TestStruct(data), "childofchild"));
  EXPECT_EQ(scene_graph.NumChildrenRecursive(), 2);
  EXPECT_EQ(scene_graph.NumChildren(), 1);
}

