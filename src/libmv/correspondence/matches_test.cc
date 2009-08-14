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

// TODO(keir): These tests leak!

namespace {

using namespace libmv;

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
}

TEST(Intersect, SimpleCase) {
  std::vector< std::vector<int> > sorted_items;
  sorted_items.resize(2);
  sorted_items[0].push_back(1);
  sorted_items[0].push_back(2);
  sorted_items[0].push_back(10);

  sorted_items[1].push_back(1);
  sorted_items[1].push_back(2);
  sorted_items[1].push_back(3);
  sorted_items[1].push_back(10);
  sorted_items[1].push_back(50);

  Intersect(&sorted_items);

  ASSERT_EQ(1,  sorted_items.size());
  ASSERT_EQ(3,  sorted_items[0].size());
  EXPECT_EQ(1,  sorted_items[0][0]);
  EXPECT_EQ(2,  sorted_items[0][1]);
  EXPECT_EQ(10, sorted_items[0][2]);
}

TEST(Intersect, ThreeItems) {
  std::vector< std::vector<int> > sorted_items;
  sorted_items.resize(3);
  sorted_items[0].push_back(1);
  sorted_items[0].push_back(2);
  sorted_items[0].push_back(10);

  sorted_items[1].push_back(1);
  sorted_items[1].push_back(2);
  sorted_items[1].push_back(3);
  sorted_items[1].push_back(10);
  sorted_items[1].push_back(50);

  sorted_items[2].push_back(1);
  sorted_items[2].push_back(3);
  sorted_items[2].push_back(10);
  sorted_items[2].push_back(50);

  Intersect(&sorted_items);

  ASSERT_EQ(1,  sorted_items.size());
  ASSERT_EQ(2,  sorted_items[0].size());
  EXPECT_EQ(1,  sorted_items[0][0]);
  EXPECT_EQ(10, sorted_items[0][1]);
}

TEST(TracksInAllImages, SimpleCase) {
  Matches matches;
  matches.Insert(1, 1, new PointFeature(1, 10));
  matches.Insert(1, 2, new PointFeature(1, 10));
  matches.Insert(1, 3, new PointFeature(1, 10));

  matches.Insert(4, 1, new PointFeature(1, 10));
  matches.Insert(4, 2, new PointFeature(1, 10));
  matches.Insert(4, 3, new PointFeature(1, 10));
  matches.Insert(4, 6, new PointFeature(1, 10));

  matches.Insert(7, 2, new PointFeature(1, 10));
  matches.Insert(7, 3, new PointFeature(1, 10));
  matches.Insert(7, 5, new PointFeature(1, 10));
  matches.Insert(7, 6, new PointFeature(1, 10));

  vector<Matches::Image> images;
  images.push_back(1);
  images.push_back(4);
  images.push_back(7);

  vector<Matches::Track> tracks;
  TracksInAllImages(matches, images, &tracks);

  ASSERT_EQ(2, tracks.size());
  ASSERT_EQ(2, tracks[0]);
  ASSERT_EQ(3, tracks[1]);
}

TEST(PointMatchMatrices, SimpleCase) {
  Matches matches;
  matches.Insert(1, 1, new PointFeature( 1,  10));
  matches.Insert(1, 2, new PointFeature( 2,  20));
  matches.Insert(1, 3, new PointFeature( 3,  30));
                                              
  matches.Insert(4, 1, new PointFeature( 4,  40));
  matches.Insert(4, 2, new PointFeature( 5,  50));
  matches.Insert(4, 3, new PointFeature( 6,  60));
  matches.Insert(4, 6, new PointFeature( 7,  70));
                                              
  matches.Insert(7, 2, new PointFeature( 8,  80));
  matches.Insert(7, 3, new PointFeature( 9,  90));
  matches.Insert(7, 6, new PointFeature(10, 100));
                                              
  matches.Insert(5, 1, new PointFeature(11, 110));
  matches.Insert(5, 3, new PointFeature(12, 120));
  matches.Insert(5, 5, new PointFeature(13, 130));
  matches.Insert(5, 6, new PointFeature(14, 140));

  vector<Mat> xs, xse;
  xse.resize(3);
  xse[0].resize(2, 2); xse[0] <<  2,  3,
                                 20, 30;
  xse[1].resize(2, 2); xse[1] <<  5,  6,
                                 50, 60;
  xse[2].resize(2, 2); xse[2] <<  8,  9,
                                 80, 90;

  vector<Matches::Image> images;
  vector<Matches::Track> tracks;
  images.push_back(1);
  images.push_back(4);
  images.push_back(7);
  PointMatchMatrices(matches, images, &tracks, &xs);

  ASSERT_EQ(2, tracks.size());
  ASSERT_EQ(xse.size(), xs.size());
  for (int i = 0; i < xse.size(); ++i) {
    EXPECT_MATRIX_EQ(xse[i], xs[i]);
  }
}

}  // namespace
