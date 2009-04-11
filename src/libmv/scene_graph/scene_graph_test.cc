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

struct test_struct {
  int data_;
  float some_more_data_;
};

TEST(scene_graph, general_usage) {
  SceneGraph<test_struct> scene_graph;
  test_struct *t0 = new test_struct;
  SGNotRoot<test_struct> *n = MakeSGNode(t0, "child");
  scene_graph.AddChild(n);
  EXPECT_TRUE(n->GetParent() == &scene_graph);
  EXPECT_TRUE(scene_graph.NumChildren() == 1);
  SGNode<test_struct> *node = scene_graph.GetAtPath("/child");
  EXPECT_TRUE(node==n);
  EXPECT_TRUE(node->GetObject() == t0);
  char *path = node->GetPath();
  EXPECT_FALSE(strcmp(path, "/child"));
  delete [] path;
  
  test_struct *t1 = new test_struct;
  SGNotRoot<test_struct> *child = MakeSGNode(t1, "childb");
  node = node->AddChild(child);
  EXPECT_TRUE(child->GetParent() == node);
  EXPECT_TRUE(scene_graph.NumChildren() == 1);
  EXPECT_TRUE(node->NumChildren() == 1);
  path = child->GetPath();
  EXPECT_FALSE(strcmp(path, "/child/childb"));
  EXPECT_TRUE(scene_graph.GetAtPath("/child/childb") == child);
  delete [] path;
}

