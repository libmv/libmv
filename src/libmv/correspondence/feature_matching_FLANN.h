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


#ifndef LIBMV_CORRESPONDENCE_FEATURE_MATCHING_FLANN_H_
#define LIBMV_CORRESPONDENCE_FEATURE_MATCHING_FLANN_H_

#include "libmv/base/vector.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/matches.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"

using namespace libmv;


// Compute candidate matches between 2 sets of features.  Two features A and B
// are a candidate match if A is the nearest neighbor of B and B is the nearest
// neighbor of A.
void FindSymmetricCandidateMatches_FLANN( const FeatureSet &left,
                                          const FeatureSet &right,
                                          Matches *matches);

// Compute 2 nearest matches of featureSet left in featureSet right.
// It allow to keep only strong and distinctive match.
// A match is kept if the ratio of the two returned distances is strong.
// I.E distance[0] < fRatio * distances[1].
// From David Lowe “Distinctive Image Features from Scale-Invariant Keypoints”.
// You can use David Lowe's magic ratio (0.6 or 0.8).
// 0.8 allow to remove 90% of the false matches while discarding less than 5%
// of the correct matches.
void FindCandidateMatchesDistanceRatio_FLANN( const FeatureSet &left,
                                              const FeatureSet &right,
                                              Matches *matches,
                                              float fRatio = 0.8f);

// FLANN_Data is only a convenient wrapper over the data
// It do not manage data destruction
struct FLANN_Data
{
        float * array;
        int rows;
        int cols;
};

//-- Small Wrapper to use Flann library :
// http://www.cs.ubc.ca/~mariusm/index.php/FLANN/FLANN
// David G. Lowe and Marius Muja

bool FLANN_Wrapper(const FLANN_Data & testSet,const FLANN_Data & dataSet,
                  vector<int> * resultIndices, vector<float> * resultDistances,
                  int NumberOfNeighbours);


#endif //LIBMV_CORRESPONDENCE_FEATURE_MATCHING_FLANN_H_
