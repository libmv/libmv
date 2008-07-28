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

#include <cstdio>

#include "libmv/correspondence/correspondence.h"
#include "libmv/correspondence/feature.h"
#include "testing/testing.h"

using libmv::Correspondences;
using libmv::CorrespondencesView;
using libmv::Feature;

namespace {

struct TestFeature : public Feature {
  virtual ~TestFeature() {}
  TestFeature(int the_tag)
    : tag(the_tag) {}
  int tag;
};

struct SiblingTestFeature : public Feature {
  virtual ~SiblingTestFeature() {}
};

TEST(Correspondences, Views) {
  Correspondences correspondences;
  correspondences.Insert(1, 1, new SiblingTestFeature);
  correspondences.Insert(1, 2, new TestFeature(3));
  correspondences.Insert(1, 4, new SiblingTestFeature);
  Correspondences::Iterator it = correspondences.ScanAllFeatures();
  it.Next();  // Ordering means the test feature will be 2nd.
  EXPECT_TRUE(it.feature() != NULL);
  EXPECT_EQ(1, it.image());
  EXPECT_EQ(2, it.track());

  CorrespondencesView<TestFeature> view(&correspondences);
  CorrespondencesView<TestFeature>::Iterator view_it = view.ScanAllFeatures();
  ASSERT_TRUE(view_it.feature() != NULL);
  EXPECT_EQ(3, view_it.feature()->tag);
  EXPECT_EQ(1, view_it.image());
  EXPECT_EQ(2, view_it.track());
}

// This function works over TestFeatures only; other features will be filtered
// out by the view.
int CountTestPoints(CorrespondencesView<TestFeature> view) {
  CorrespondencesView<TestFeature>::Iterator it = view.ScanAllFeatures();
  int count = 0;
  for (; !it.Done(); it.Next()) {
    ++count;
  }
  return count;
}

TEST(Correspondences, ImplicitViewConstruction) {
  Correspondences correspondences;
  correspondences.Insert(1, 1, new SiblingTestFeature);
  correspondences.Insert(1, 2, new TestFeature(3));
  correspondences.Insert(1, 3, new TestFeature(4));
  correspondences.Insert(1, 4, new SiblingTestFeature);
  correspondences.Insert(1, 5, new SiblingTestFeature);

  EXPECT_EQ(2, CountTestPoints(&correspondences));
}

}  // namespace
