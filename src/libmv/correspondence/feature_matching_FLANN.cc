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


#include "libmv/correspondence/feature_matching_FLANN.h"
#include "third_party/flann/src/cpp/flann.h"

//-- Small Wrapper to use Flann library :
// http://www.cs.ubc.ca/~mariusm/index.php/FLANN/FLANN
// David G. Lowe and Marius Muja

bool FLANN_Wrapper(const FLANN_Data & testSet,const FLANN_Data & dataSet,
                  vector<int> * resultIndices, vector<float> * resultDistances,
                  int NumberOfNeighbours)
{
  //-- Check data length compatibility :
  if (testSet.cols != dataSet.cols)
    return false;

  //-- Check if resultIndices is allocated
  resultIndices->resize(testSet.rows * NumberOfNeighbours);
  resultDistances->resize(testSet.rows * NumberOfNeighbours);

  FLANNParameters p;  // index parameters are stored here
  p.log_destination = NULL;
  p.log_level = LOG_INFO;
  // want 90% target precision
  // the rest of the parameters are automatically computed
  p.target_precision = 0.9f;
  // compute the NumberOfNeighbours nearest-neighbors of each point in the testset
  int * resultPTR = &((*resultIndices)[0]);
  float * distancePTR = &(*resultDistances)[0];
  return ( 0 == flann_find_nearest_neighbors(dataSet.array,
    dataSet.rows, dataSet.cols, testSet.array, testSet.rows,
    resultPTR, distancePTR, NumberOfNeighbours, &p));
}

// Compute candidate matches between 2 sets of features.  Two features a and b
// are a candidate match if a is the nearest neighbor of b and b is the nearest
// neighbor of a.
void FindSymmetricCandidateMatches_FLANN(const FeatureSet &left,
                          const FeatureSet &right,
                          Matches *matches) {

  if (left.features.size() == 0)  {
    return;
  }
  int descriptorSize = left.features[0].descriptor.coords.size();
  // Allocate and paste the necessary data.
  // FLANN need a contiguous data array.
  float * arrayA = new float[left.features.size()*descriptorSize];
  float * arrayB = new float[right.features.size()*descriptorSize];

  //-- Paste data :
  for (int i = 0; i < (int)left.features.size(); ++i)
  {
    for (int j = 0;j < descriptorSize; ++j)
      arrayA[descriptorSize*i + j] = (float)left.features[i][j];
  }
  for (int i = 0; i < (int)right.features.size(); ++i)
  {
    for (int j = 0;j < descriptorSize; ++j)
      arrayB[descriptorSize*i + j] = (float)right.features[i][j];
  }

  FLANN_Data dataA={arrayA,left.features.size(),descriptorSize};
  FLANN_Data dataB={arrayB,right.features.size(),descriptorSize};

  // Perform ANN search from A to B. And B to A.
  // Use returned indices to keep only symetric matches
  libmv::vector<int> indices;
  libmv::vector<int> indicesReverse;
  libmv::vector<float> distances;
  libmv::vector<float> distancesReverse;
  int NN = 1;
  bool breturn = FLANN_Wrapper(dataA, dataB, &indices, &distances, NN) &&
                  FLANN_Wrapper(dataB, dataA, &indicesReverse, &distancesReverse, NN);

  delete [] arrayA;
  delete [] arrayB;

  if (breturn)
  {
    //TODO(pmoulon) clear previous matches.
    int max_track_number = 0;
    for (size_t i = 0; i < indices.size(); ++i) {
      // Add the matche only if we have a symetric result.
      if (i == indicesReverse[indices[i]]) {
        matches->Insert(0, max_track_number, &left.features[i]);
        matches->Insert(1, max_track_number, &right.features[indices[i]]);
        max_track_number++;
      }
    }
  }
}

void FindCandidateMatchesDistanceRatio_FLANN( const FeatureSet &left,
                                              const FeatureSet &right,
                                              Matches *matches,
                                              float fRatio) {

  if (left.features.size() == 0)  {
    return;
  }
  int descriptorSize = left.features[0].descriptor.coords.size();
 //-- Allocate and paste the necessary data
  float * arrayA = new float[left.features.size()*descriptorSize];
  float * arrayB = new float[right.features.size()*descriptorSize];

  //-- Paste data :
  for (int i = 0; i < (int)left.features.size(); ++i)
  {
    for (int j = 0;j < descriptorSize; ++j)
      arrayA[descriptorSize*i + j] = (float)left.features[i][j];
  }
  for (int i = 0; i < (int)right.features.size(); ++i)
  {
    for (int j = 0;j < descriptorSize; ++j)
      arrayB[descriptorSize*i + j] = (float)right.features[i][j];
  }

  FLANN_Data dataA={arrayA,left.features.size(),descriptorSize};
  FLANN_Data dataB={arrayB,right.features.size(),descriptorSize};

  libmv::vector<int> indices;
  libmv::vector<float> distances;
  const int NN = 2;
  bool breturn = FLANN_Wrapper(dataA,dataB, &indices, &distances, NN);

  delete [] arrayA;
  delete [] arrayB;

  if (breturn)
  {
    //TODO(pmoulon) clear previous matches.
    int max_track_number = 0;
    for (size_t i = 0; i < left.features.size(); ++i) {
      // Test distance ratio :
      float distance0 = distances[i*NN];
      float distance1 = distances[i*NN+NN-1];
      if (distance0 < fRatio * distance1)
      {
        matches->Insert(0, max_track_number, &left.features[i]);
        matches->Insert(1, max_track_number, &right.features[indices[i*NN]]);
        max_track_number++;
      }
    }
  }
}
