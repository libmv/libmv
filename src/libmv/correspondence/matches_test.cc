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

#include "libmv/correspondence/matches.h"
#include "libmv/correspondence/feature.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"

using libmv::Matches;
using libmv::Feature;
using libmv::PointFeature;

namespace {

// A derived feature with a tag, so that it is possible to check that the right
// features are produced by the iterators.
struct MyPoint : public Feature {
  virtual ~MyPoint() {}
  MyPoint(int the_tag) : tag(the_tag) {}
  int tag;
};

struct SiblingTestFeature : public Feature {
  virtual ~SiblingTestFeature() {}
};

TEST(Matches, Views) {
  Matches matches;
  matches.Insert(1, 1, new SiblingTestFeature);
  matches.Insert(1, 2, new MyPoint(30));
  matches.Insert(1, 4, new SiblingTestFeature);
  Matches::Features<Feature> r1 = matches.All<Feature>();
  ++r1;  // Ordering means the test feature will be 2nd.
  ASSERT_TRUE(r1);
  EXPECT_TRUE(r1.feature() != NULL);
  EXPECT_EQ(1, r1.image());
  EXPECT_EQ(2, r1.track());

  Matches::Features<MyPoint> r = matches.All<MyPoint>();
  ASSERT_TRUE(r);
  ASSERT_TRUE(r.feature() != NULL);
  EXPECT_EQ(30, r.feature()->tag);
  EXPECT_EQ(1,  r.image());
  EXPECT_EQ(2,  r.track());

  //DeleteCorrespondenceFeatures(&matches);
}

}  // namespace
