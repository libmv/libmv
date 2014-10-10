// Copyright (c) 2010 libmv authors.
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

#include "libmv/correspondence/ArrayMatcher.h"
#include "libmv/correspondence/ArrayMatcher_BruteForce.h"
#include "libmv/correspondence/ArrayMatcher_Kdtree_Flann.h"
#include "libmv/correspondence/ArrayMatcher_Kdtree.h"

#include "libmv/correspondence/feature_matching.h"
#include "libmv/logging/logging.h"
#include "testing/testing.h"
using testing::Types;

namespace {

using namespace libmv;
using namespace libmv::descriptor;
using namespace libmv::correspondence;

template <class Kernel>
struct MatchingKernelTest : public testing::Test {
};

// Test :
// - Libmv Kdtree,
// - Linear matching FLANN,
// - FLANN Kdtree.
typedef Types< ArrayMatcher_Kdtree<float>,
               ArrayMatcher_BruteForce<float>,
               ArrayMatcher_Kdtree_Flann<float>
               > MatchingKernelImpl;

TYPED_TEST_CASE(MatchingKernelTest, MatchingKernelImpl);

TYPED_TEST(MatchingKernelTest, MatcherInterfaceSymmetry)
{
  int descriptorSize = 2;
  // Build one feature set.
  KeypointFeatureSet featureSet;
  for (int i=0; i < 4; ++i)
  {
    KeypointFeature feat;
    Vecf desc(descriptorSize);
    desc << i+1, i+1;
    feat.descriptor = VecfDescriptor(desc);
    featureSet.features.push_back(feat);
  }

  // Match feature set between same feature set and assert the result.
  float * data =
    KeypointFeatureSet::KeypointFeatureSetDescriptorsToContiguousArray(featureSet);

  // Build the array matcher in order to compute matches pair.
  libmv::correspondence::ArrayMatcher<float> * pArrayMatcher =
    new TypeParam;

  libmv::vector<int> indices;
  libmv::vector<float> distances;

  bool breturn = false;
  int nbFeatures = featureSet.features.size();
  if (pArrayMatcher->build(data,nbFeatures,descriptorSize)) {
      const int NN = 1;
      breturn =
        pArrayMatcher->searchNeighbours(data,nbFeatures,
          &indices, &distances, NN);
  }
  delete pArrayMatcher;

  delete [] data;

  EXPECT_EQ(indices.size(), 4);
  EXPECT_EQ(distances.size(), 4);
  // Check that matches are symmetric.
  if (breturn)  {
    for (size_t i = 0; i < indices.size(); ++i) {
      EXPECT_EQ(i , indices[i]);
      }
  }
}

TYPED_TEST(MatchingKernelTest, MatcherInterface)
{
  int descriptorSize = 2;
  // Build two feature set.
  KeypointFeatureSet featureSetA;
  for (int i=0; i < 4; ++i)
  {
    KeypointFeature feat;
    Vecf desc(descriptorSize);
    desc << i*2, i*2;
    feat.descriptor = VecfDescriptor(desc);
    featureSetA.features.push_back(feat);
  }

  // Build two feature set.
  KeypointFeatureSet featureSetB;
  for (int i=0; i < 5; ++i)
  {
    KeypointFeature feat;
    Vecf desc(descriptorSize);
    desc << i*2, i*2;
    feat.descriptor = VecfDescriptor(desc);
    featureSetB.features.push_back(feat);
  }

  // Match feature set between same feature set and assert the result.
  float * dataA =
    KeypointFeatureSet::KeypointFeatureSetDescriptorsToContiguousArray(featureSetA);
  float * dataB =
    KeypointFeatureSet::KeypointFeatureSetDescriptorsToContiguousArray(featureSetB);

  // Build the array matcher in order to compute matches pair.
  libmv::correspondence::ArrayMatcher<float> * pArrayMatcherA =
    new TypeParam;
  libmv::correspondence::ArrayMatcher<float> * pArrayMatcherB =
    new TypeParam;

  libmv::vector<int> indices, indicesReverse;
  libmv::vector<float> distances, distancesReverse;

  bool breturn = false;
  if (pArrayMatcherA->build(dataA,featureSetA.features.size(),descriptorSize) &&
      pArrayMatcherB->build(dataB,featureSetB.features.size(),descriptorSize) )
    {
      const int NN = 1;
      breturn =
        pArrayMatcherB->searchNeighbours(dataA,featureSetA.features.size(),
          &indices, &distances, NN) &&
        pArrayMatcherA->searchNeighbours(dataB,featureSetB.features.size(),
          &indicesReverse, &distancesReverse, NN);
    }
  delete pArrayMatcherA;
  delete pArrayMatcherB;

  delete [] dataA;
  delete [] dataB;

  EXPECT_EQ(indices.size(), 4);
  EXPECT_EQ(distances.size(), 4);
  EXPECT_EQ(indicesReverse.size(), 5);
  EXPECT_EQ(distancesReverse.size(), 5);

  // Check distances and indexes.
  for (size_t i=0; i < indices.size(); ++i)
  {
    EXPECT_EQ(i, indices[i]);
    EXPECT_NEAR(0.0, distances[i], 1e-8);
  }

  // Check distances and indexes.
  int gtIndices[]={0,1,2,3,3};
  float gtDistances[]={0.0f,0.0f,0.0f,0.0f,8.0f};
  for (size_t i=0; i < indicesReverse.size(); ++i)
  {
    EXPECT_NEAR(gtDistances[i], distancesReverse[i], 1e-8);
    EXPECT_EQ(gtIndices[i], indicesReverse[i]);
  }

  // Check that matches are symmetric.
  if (breturn)  {
    for (size_t i = 0; i < indices.size(); ++i) {
      EXPECT_EQ(i , indicesReverse[indices[i]]);
      }
  }
}


}  // namespace
