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


#include "libmv/correspondence/feature_matching.h"

#include "libmv/correspondence/ArrayMatcher.h"
#include "libmv/correspondence/ArrayMatcher_BruteForce.h"
#include "libmv/correspondence/ArrayMatcher_Kdtree_Flann.h"
#include "libmv/correspondence/ArrayMatcher_Kdtree.h"

// Compute candidate matches between 2 sets of features.  Two features A and B
// are a candidate match if A is the nearest neighbor of B and B is the nearest
// neighbor of A.
void FindCandidateMatches(const FeatureSet &left,
                          const FeatureSet &right,
                          Matches *matches) {
  if (left.features.size() == 0 ||
      right.features.size() == 0 )  {
    return;
  }
  int descriptorSize = left.features[0].descriptor.coords.size();

  // Paste the necessary data in contiguous arrays.
  float * arrayA = FeatureSet::FeatureSetDescriptorsToContiguousArray(left);
  float * arrayB = FeatureSet::FeatureSetDescriptorsToContiguousArray(right);

  // Build the arrays matcher in order to compute matches pair.
  libmv::correspondence::ArrayMatcher<float> * pArrayMatcherA =
    new libmv::correspondence::ArrayMatcher_Kdtree<float>;

  libmv::correspondence::ArrayMatcher<float> * pArrayMatcherB =
    new libmv::correspondence::ArrayMatcher_Kdtree<float>;

  libmv::vector<int> indices, indicesReverse;
  libmv::vector<float> distances, distancesReverse;

  bool breturn = false;
  if (pArrayMatcherA->build(arrayA,left.features.size(),descriptorSize) &&
      pArrayMatcherB->build(arrayB,right.features.size(),descriptorSize) )
    {
      const int NN = 1;
      breturn =
        pArrayMatcherB->searchNeighbours(arrayA,left.features.size(),
          &indices, &distances, NN) &&
        pArrayMatcherA->searchNeighbours(arrayB,right.features.size(),
          &indicesReverse, &distancesReverse, NN);
    }
  delete pArrayMatcherA;
  delete pArrayMatcherB;

  delete [] arrayA;
  delete [] arrayB;

  // From putative matches get symmetric matches.
  if (breturn)
  {
    //TODO(pmoulon) clear previous matches.
    int max_track_number = 0;
    for (size_t i = 0; i < indices.size(); ++i) {
      // Add the matche only if we have a symetric result.
      if (i == indicesReverse[indices[i]])  {
        matches->Insert(0, max_track_number, &left.features[i]);
        matches->Insert(1, max_track_number, &right.features[indices[i]]);
        ++max_track_number;
      }
    }
  }
  else  {
    LOG(INFO) << "Cannot compute symmetric matches.";
  }
}

float * FeatureSet::FeatureSetDescriptorsToContiguousArray
    ( const FeatureSet & featureSet ) {

    if (featureSet.features.size() == 0)  {
      return NULL;
    }
    int descriptorSize = featureSet.features[0].descriptor.coords.size();
    // Allocate and paste the necessary data.
    float * array = new float[featureSet.features.size()*descriptorSize];

    //-- Paste data in the contiguous array :
    for (int i = 0; i < (int)featureSet.features.size(); ++i)
    {
      for (int j = 0;j < descriptorSize; ++j)
        array[descriptorSize*i + j] = (float)featureSet.features[i][j];
    }
    return array;
  }
