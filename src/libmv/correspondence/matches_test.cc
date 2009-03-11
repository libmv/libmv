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

TEST(Matches, ImplicitViewConstruction) {
  Matches matches;
  int expected_its[] = {
    1, 1, 11,
    1, 2, 12,
    2, 8, 14,
    3, 9, 15,
    0
  };

  int max_i = 0;
  for (; expected_its[max_i]; max_i += 3) {
    matches.Insert(expected_its[max_i + 0],
                   expected_its[max_i + 1], 
                   new MyPoint(expected_its[max_i + 2])); 
      LOG(INFO) << "Inserting " << expected_its[max_i + 0]
                << ", " << expected_its[max_i + 1] 
                << ", " << expected_its[max_i + 2]; 
  }

  // Add other types of features to filter out.
  matches.Insert(0, 0,  new SiblingTestFeature);
  matches.Insert(4, 4,  new SiblingTestFeature);
  matches.Insert(2, 4,  new SiblingTestFeature);
  matches.Insert(9, 5,  new SiblingTestFeature);
  matches.Insert(9, 10, new SiblingTestFeature);

  // Over images, then features in each image.
  int i = 0;
  for (Matches::ImageIterator it = matches.ImageBegin();
       it != matches.ImageEnd(); ++it) {
    for (Matches::ImageFeatureIterator<MyPoint> tt = it.begin<MyPoint>();
         tt != it.end<MyPoint>(); ++tt) {
      LOG(INFO) << "tt.image() " << tt.image()
                << " tt.track() " << tt.track()
                << " tt.feature()->tag() " << tt.feature()->tag;
      ASSERT_LT(i, max_i);
      EXPECT_EQ(expected_its[i+0], tt.image());
      EXPECT_EQ(expected_its[i+1], tt.track());
      EXPECT_EQ(expected_its[i+2], tt.feature()->tag);
      i += 3;
    }
  }
  EXPECT_EQ(12, i);

  // Over tracks, then features in each track. Because of specially selected
  // bipartite graph edges, the ordering is the same as above.
  i = 0;
  for (Matches::TrackIterator it = matches.TrackBegin();
       it != matches.TrackEnd(); ++it) {
    for (Matches::TrackFeatureIterator<MyPoint> tt = it.begin<MyPoint>();
         tt != it.end<MyPoint>(); ++tt) {
      LOG(INFO) << "tt.image() " << tt.image()
                << " tt.track() " << tt.track()
                << " tt.feature()->tag() " << tt.feature()->tag;
      ASSERT_LT(i, max_i);
      EXPECT_EQ(expected_its[i+0], tt.image());
      EXPECT_EQ(expected_its[i+1], tt.track());
      EXPECT_EQ(expected_its[i+2], tt.feature()->tag);
      i += 3;
    }
  }
  EXPECT_EQ(12, i);

  DeleteMatchFeatures(&matches);
}

}  // namespace
