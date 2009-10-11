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
#include <algorithm>

#include "libmv/correspondence/feature_set.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

// A derived feature with a tag, so that it is possible to check that the right
// features are produced by the iterators.
struct MyPoint : public Feature {
  virtual ~MyPoint() {}
  MyPoint() {}
  MyPoint(int the_tag) : tag(the_tag) {}
  int tag;
};

struct SiblingTestFeature : public Feature {
  virtual ~SiblingTestFeature() {}
};


TEST(FeatureSet, New) {
  FeatureSet features;

  // Create a feature.
  FeatureSet::Iterator<MyPoint> it = features.New<MyPoint>();
  // Use it.
  it.feature().tag = 3;
}

TEST(FeatureSet, Insert) {
  FeatureSet features;
  // Insert a feature by copying its contents.
  FeatureSet::Iterator<MyPoint> it = features.Insert<MyPoint>(MyPoint(1));
  EXPECT_EQ(1, it.feature().tag);
  it = features.Insert<MyPoint>(MyPoint(2));
  EXPECT_EQ(2, it.feature().tag);
}

TEST(FeatureSet, All) {
  FeatureSet features;
  
  // Insert some features of different types.
  features.Insert<MyPoint>(MyPoint(1));
  features.New<SiblingTestFeature>();
  features.Insert<MyPoint>(MyPoint(2));
  features.New<SiblingTestFeature>();
  features.Insert<MyPoint>(MyPoint(3));

  // Scan my points.
  std::vector<int> tags;
  for (FeatureSet::Iterator<MyPoint> it = features.All<MyPoint>();
       it; ++it) {
    tags.push_back(it.feature().tag);
  }
  std::sort(tags.begin(), tags.end());
  EXPECT_EQ(3, tags.size());
  EXPECT_EQ(1, tags[0]);
  EXPECT_EQ(2, tags[1]);
  EXPECT_EQ(3, tags[2]);
}

TEST(FeatureSet, Delete) {
  FeatureSet features;
  
  // Insert some features of different types.
  FeatureSet::Iterator<MyPoint> it = features.Insert<MyPoint>(MyPoint(1));
  features.New<SiblingTestFeature>();
  features.Insert<MyPoint>(MyPoint(2));
  features.New<SiblingTestFeature>();
  features.Insert<MyPoint>(MyPoint(3));

  features.Delete(&it);
  
  // Scan my points.
  std::vector<int> tags;
  for (FeatureSet::Iterator<MyPoint> it = features.All<MyPoint>();
       it; ++it) {
         tags.push_back(it.feature().tag);
       }
       std::sort(tags.begin(), tags.end());
       EXPECT_EQ(2, tags.size());
       EXPECT_EQ(2, tags[0]);
       EXPECT_EQ(3, tags[1]);
}

TEST(FeatureSet, Find) {
  FeatureSet features;
  
  // Insert some features of different types.
  FeatureSet::Iterator<MyPoint> it = features.Insert<MyPoint>(MyPoint(1));
  features.New<SiblingTestFeature>();
  features.Insert<MyPoint>(MyPoint(2));
  features.New<SiblingTestFeature>();
  features.Insert<MyPoint>(MyPoint(3));

  FeatureSet::Iterator<MyPoint> found = features.Find<MyPoint>(it.id());
  std::vector<int> tags;
  EXPECT_EQ(it.id(), found.id());
  EXPECT_EQ(it.feature().tag, found.feature().tag);
}

}  // namespace
