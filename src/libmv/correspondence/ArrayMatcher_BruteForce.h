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

#ifndef LIBMV_CORRESPONDENCE_ARRAYMATCHER_BRUTE_FORCE_H_
#define LIBMV_CORRESPONDENCE_ARRAYMATCHER_BRUTE_FORCE_H_

#include "libmv/correspondence/ArrayMatcher.h"
#include "third_party/flann/src/cpp/flann.h"

namespace libmv {
namespace correspondence  {

/// Implement ArrayMatcher as a FLANN LINEAR matcher.
template < typename Scalar >
class ArrayMatcher_BruteForce : public ArrayMatcher<Scalar>
{
  public:
  ArrayMatcher_BruteForce():_index_id(NULL) {}

  ~ArrayMatcher_BruteForce()  {
    flann_free_index(_index_id, &_p);
    }

  /**
   * Build the matching structure
   *
   * \param[in] dataset   Input data.
   * \param[in] nbRows    The number of component.
   * \param[in] dimension Lenght of the data contained in the each
   *  row of the dataset.
   *
   * \return True if success.
   */
  bool build( const Scalar * dataset, int nbRows, int dimension)  {

    _p.log_destination = NULL;
    _p.log_level = LOG_INFO;

    // Force LINEAR MATCHING : Brute Force.
    _p.algorithm = LINEAR;
    _p.target_precision = -1.0f;

    //-- Build FLANN index
    float fspeedUp;
    _index_id = flann_build_index( (float*)dataset, nbRows, dimension, &fspeedUp, &_p);
    return (_index_id != NULL);
  }

  /**
   * Search the nearest Neighbour of the scalar array query.
   *
   * \param[in]   query     The query array
   * \param[out]  indice    The indice of array in the dataset that
   *  have been computed as the nearest array.
   * \param[out]  distance  The distance between the two arrays.
   *
   * \return True if success.
   */
  bool searchNeighbour( const Scalar * query, int * indice, Scalar * distance)
  {
    if (_index_id != NULL) {
      int iRet = flann_find_nearest_neighbors_index(_index_id, (Scalar*)query, 1,
        indice, distance, 1, _p.checks, &_p);
        return (iRet == 0);
    }
    else  {
      return false;
    }
  }


  /**
   * Search the N nearest Neighbour of the scalar array query.
   *
   * \param[in]   query     The query array
   * \param[in]   nbQuery   The number of query rows
   * \param[out]  indice    The indices of arrays in the dataset that
   *  have been computed as the nearest arrays.
   * \param[out]  distance  The distances between the matched arrays.
   *
   * \return True if success.
   */
  bool searchNeighbours( const Scalar * query, int nbQuery,
    vector<int> * indice, vector<Scalar> * distance, int NN)
  {
    if (_index_id != NULL)  {
      //-- Check if resultIndices is allocated
      indice->resize(nbQuery * NN);
      distance->resize(nbQuery * NN);

      int * indicePTR = &((*indice)[0]);
      float * distancePTR = &(*distance)[0];
      int iRet = flann_find_nearest_neighbors_index(_index_id, (Scalar*)query, nbQuery,
        indicePTR, distancePTR, NN, _p.checks, &_p);
        return (iRet == 0);
    }
    else  {
      return false;
    }
  }

  private :
  FLANN_INDEX _index_id;
  FLANNParameters _p;
};

} // namespace correspondence
} // namespace libmv

#endif // LIBMV_CORRESPONDENCE_ARRAYMATCHER_BRUTE_FORCE_H_
