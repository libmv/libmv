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

TEST(Matches, FilteringIterators) {
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

TEST(Matches, IteratingOverTracksInCertainImages) {
  // It's often necessary to iterate over either all the tracks that are
  // visible in a given set of images. We do this with an iterator, shown
  // below, which takes a set of images and then iterates over the tracks which
  // appear in all images.

  int features[] = {
    // Image ID, track ID, feature tag.
    1, 1, 11,  // Track 1 is in all 3 images.
    2, 1, 12,
    3, 1, 13,
    1, 2, 14,  // This track, 2, should be ignored.
    2, 2, 15,
    1, 3, 16,  // Track 3 is in all 3 images.
    2, 3, 17,
    3, 3, 18,
    6, 3, 19,  // This feature should not be scanned.
    6, 4, 20,  // This track should not be scanned.
    0
  };

  Matches matches;

  int max_i = 0;
  for (; features[max_i]; max_i += 3) {
    matches.Insert(features[max_i + 0],
                   features[max_i + 1], 
                   new MyPoint(features[max_i + 2])); 
    LOG(INFO) << "> " << features[max_i + 0]
              << ", " << features[max_i + 1]
              << ", " << features[max_i + 2];
  }
  EXPECT_EQ(max_i, 30);

  // Insert some distractor tracks
  matches.Insert(1, 50, new SiblingTestFeature);
  matches.Insert(2, 50, new SiblingTestFeature);
  matches.Insert(3, 50, new SiblingTestFeature);
  matches.Insert(1, 51, new SiblingTestFeature);
  matches.Insert(2, 51, new SiblingTestFeature);
  matches.Insert(3, 51, new SiblingTestFeature);

  std::set<Matches::Image> images;
  images.insert(1);
  images.insert(2);
  images.insert(3);

  int expected_features[] = {
    // Image ID, track ID, feature tag.
    1, 1, 11,  // Track 1 is in all 3 images.
    2, 1, 12,
    3, 1, 13,
    1, 3, 16,  // Track 3 is in all 3 images.
    2, 3, 17,
    3, 3, 18,
    0
  };
  int i = 0;
  int num_tracks = 0;
  // For each track...
  for (Matches::TracksInImagesIterator<MyPoint> it =
             matches.TracksInImagesBegin<MyPoint>(images);
       it != matches.TracksInImagesEnd<MyPoint>(images); ++it) {
    // For each feature...
    for (Matches::TracksInImagesFeatureIterator<MyPoint> tt =
         it.begin(); tt != it.end(); ++tt) {
      EXPECT_EQ(expected_features[i + 0], tt.image());
      EXPECT_EQ(expected_features[i + 1], tt.track());
      EXPECT_EQ(expected_features[i + 2], tt.feature()->tag);
      i += 3;
    }
    LOG(INFO) << "Track: " << *it;
    ++num_tracks;
  }
  EXPECT_EQ(18, i);
  EXPECT_EQ(2, num_tracks);
}

}  // namespace
