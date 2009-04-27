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

#include <string>

#include "libmv/scene_graph/scene_graph_simple.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"

using libmv::scene::Node;

TEST(SceneGraph, IteratingOverAddedNodes) {
  Node<int> scene("root node", new int(0));
  scene.AddChild(new Node<int>("bar", new int(5)));
  scene.AddChild(new Node<int>("zap", new int(10)));
  scene.AddChild(new Node<int>("foo", new int(4)));

  const char * expected_names[] = { "bar", "foo", "zap" };
  int expected_values[] = { 5, 4, 10 };
  int i = 0;
  for (Node<int>::iterator it = scene.begin(); it != scene.end(); ++it, ++i) {
    EXPECT_EQ(expected_names[i], it->GetName());
    EXPECT_EQ(expected_values[i], *it->GetObject());
    EXPECT_EQ(&scene, it->GetParent());
  }
  scene.DeleteObject();
  for (Node<int>::iterator it = scene.begin(); it != scene.end(); ++it) {
    it->DeleteObject();
  }
}

TEST(SceneGraph, GetChildReturnsNullIfNotFound) {
  Node<int> scene("root node", new int(0));

  const char * test_names[] = { "bar", "foo", "zap" };
  for (int i = 0; i < 3; ++i) {
    EXPECT_TRUE(NULL == scene.GetChild(test_names[i]));
    Node<int> *node = new Node<int>(test_names[i], new int(10));
    scene.AddChild(node);
    EXPECT_EQ(node, scene.GetChild(test_names[i]));
  }
  scene.DeleteObject();
  for (Node<int>::iterator it = scene.begin(); it != scene.end(); ++it) {
    it->DeleteObject();
  }
}

TEST(SceneGraph, SetName) {
  Node<int> scene("root node", new int(0));

  const char * test_names[] = { "bar", "foo", "zap" };
  for (int i = 0; i < 3; ++i) {
    Node<int> *node = new Node<int>(test_names[i], new int(10));
    scene.AddChild(node);
  }
  
  Node<int> *node = scene.GetChild("bar");
  EXPECT_TRUE(node);
  node->SetName("something");
  EXPECT_EQ(node, scene.GetChild("something"));
  
  scene.DeleteObject();
  for (Node<int>::iterator it = scene.begin(); it != scene.end(); ++it) {
    it->DeleteObject();
  }
}
