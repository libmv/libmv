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

using libmv::SceneGraph;
using libmv::MakeSGNode;
using libmv::SGNode;
using libmv::SGNotRoot;
using libmv::SGNotLeaf;

struct TestStruct {
  int data_;
  float some_more_data_;
  TestStruct() {}
  TestStruct(const TestStruct &t) {
    data_ = t.data_;
    some_more_data_ = t.some_more_data_;
  }
};

TEST(SceneGraph, Paths) {
  SceneGraph<TestStruct> scene_graph;
  TestStruct *t0 = new TestStruct;
  SGNotRoot<TestStruct> *n = MakeSGNode(t0, "child");
  scene_graph.AddChild(n);
  EXPECT_TRUE(n->GetParent() == &scene_graph);
  EXPECT_TRUE(scene_graph.NumChildren() == 1);
  SGNode<TestStruct> *node = scene_graph.GetAtPath("/child");
  EXPECT_TRUE(node==n);
  EXPECT_TRUE(node->GetObject() == t0);
  char *path = node->GetPath();
  EXPECT_FALSE(strcmp(path, "/child"));
  delete [] path;
  
  TestStruct *t1 = new TestStruct;
  SGNotRoot<TestStruct> *child = MakeSGNode(t1, "childb");
  node = node->AddChild(child);
  EXPECT_TRUE(child->GetParent() == node);
  EXPECT_TRUE(scene_graph.NumChildren() == 1);
  EXPECT_TRUE(node->NumChildren() == 1);
  path = child->GetPath();
  EXPECT_FALSE(strcmp(path, "/child/childb"));
  EXPECT_TRUE(scene_graph.GetAtPath("/child/childb") == child);
  delete [] path;
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
  //TODO(Daniel): Finish this test and add more.
}

