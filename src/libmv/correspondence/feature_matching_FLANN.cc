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

bool FLANN_Wrapper_LINEAR(const FLANN_Data & testSet,const FLANN_Data & dataSet,
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
  // Force linear Matching
  p.algorithm = LINEAR;
  p.log_destination = NULL;
  p.log_level = LOG_INFO;
   p.target_precision = -1.f;
  // compute the NumberOfNeighbours nearest-neighbors of each point in the testset
  int * resultPTR = &((*resultIndices)[0]);
  float * distancePTR = &(*resultDistances)[0];
  return ( 0 == flann_find_nearest_neighbors(dataSet.array,
    dataSet.rows, dataSet.cols, testSet.array, testSet.rows,
    resultPTR, distancePTR, NumberOfNeighbours, &p));
}

bool FLANN_Wrapper_KDTREE(const FLANN_Data & testSet,const FLANN_Data & dataSet,
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

  // Force KDTREE
  p.algorithm = KDTREE;
  p.checks = 32;    // Maximum number of leaf checked in one search
  p.trees = 8;      // number of randomized trees to use
  p.branching = 32; // branching factor
  p.iterations = 7; // max iterations to perform in one kmeans clustering (kmeans tree)
  p.target_precision = -1.0f;

  //-- Build FLANN index
  float fspeedUp;
  FLANN_INDEX index_id = flann_build_index(dataSet.array, dataSet.rows, dataSet.cols, &fspeedUp, &p);

   // compute the NumberOfNeighbours nearest-neighbors of each point in the testset
  int * resultPTR = &((*resultIndices)[0]);
  float * distancePTR = &(*resultDistances)[0];
  int iRet = flann_find_nearest_neighbors_index(index_id, testSet.array, testSet.rows,
   resultPTR, distancePTR, NumberOfNeighbours, p.checks, &p);

  flann_free_index(index_id, &p);
  return (iRet == 0);
}


// Compute candidate matches between 2 sets of features.  Two features A and B
// are a candidate match if A is the nearest neighbor of B and B is the nearest
// neighbor of A.
void FindSymmetricCandidateMatches_FLANN(const FeatureSet &left,
                          const FeatureSet &right,
                          Matches *matches) {
  // --
  // TODO(pmoulon) template on feature type.
  // --
  if (left.features.size() == 0)  {
    return;
  }
  int descriptorSize = left.features[0].descriptor.coords.size();

  // Paste the necessary data in contiguous arrays.
  float * arrayA = FeatureSet::FeatureSetDescriptorsToContiguousArray(left);
  float * arrayB = FeatureSet::FeatureSetDescriptorsToContiguousArray(right);

  FLANN_Data dataA={arrayA,left.features.size(),descriptorSize};
  FLANN_Data dataB={arrayB,right.features.size(),descriptorSize};

  // Perform ANN search from A to B. And B to A.
  // Use returned indices to keep only symetric matches
  libmv::vector<int> indices;
  libmv::vector<int> indicesReverse;
  libmv::vector<float> distances;
  libmv::vector<float> distancesReverse;
  int NN = 1;
  bool breturn = FLANN_Wrapper_KDTREE(dataA, dataB, &indices, &distances, NN)&&
                  FLANN_Wrapper_KDTREE(dataB, dataA, &indicesReverse,
                   &distancesReverse, NN);

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

void FindCandidateMatchesDistanceRatio_FLANN( const FeatureSet &left,
                                              const FeatureSet &right,
                                              Matches *matches,
                                              float fRatio) {

  if (left.features.size() == 0)  {
    return;
  }
  int descriptorSize = left.features[0].descriptor.coords.size();
  // Paste the necessary data in contiguous arrays.
  float * arrayA = FeatureSet::FeatureSetDescriptorsToContiguousArray(left);
  float * arrayB = FeatureSet::FeatureSetDescriptorsToContiguousArray(right);

  FLANN_Data dataA={arrayA,left.features.size(),descriptorSize};
  FLANN_Data dataB={arrayB,right.features.size(),descriptorSize};

  libmv::vector<int> indices;
  libmv::vector<float> distances;
  const int NN = 2;
  bool breturn = FLANN_Wrapper_KDTREE(dataA,dataB, &indices, &distances, NN);

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
