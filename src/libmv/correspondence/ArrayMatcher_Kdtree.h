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

#ifndef LIBMV_CORRESPONDENCE_ARRAYMATCHER_KDTREE_H_
#define LIBMV_CORRESPONDENCE_ARRAYMATCHER_KDTREE_H_

#include "libmv/correspondence/ArrayMatcher.h"
#include "libmv/correspondence/kdtree.h"

namespace libmv {
namespace correspondence  {

/// Implement ArrayMatcher as the native KDtree libmv matcher.
template < typename Scalar >
class ArrayMatcher_Kdtree : public ArrayMatcher<Scalar>
{
  public:
  ArrayMatcher_Kdtree() {}

  ~ArrayMatcher_Kdtree() {}

  /**
   * Build the matching structure
   *
   * \param[in] dataset   Input data.
   * \param[in] nbRows    The number of component.
   * \param[in] dimension Length of the data contained in the each
   *  row of the dataset.
   *
   * \return True if success.
   */
  bool build( const Scalar * dataset, int nbRows, int dimension)  {

    _tree.SetDimensions(dimension);
    const Scalar * ptrDataset = dataset;
    for (int i=0; i < nbRows; ++i)  {
      _tree.AddPoint( ptrDataset,i);
      ptrDataset+=dimension;
    }
    _tree.Build(10);
    return true;
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
    Scalar distanceToQuery;
    int nni;
    _tree.ApproximateNearestNeighborBestBinFirst(query, 1000, &nni, &distanceToQuery);
    *indice = nni;
    *distance = distanceToQuery;
    return true;
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
    if (NN==1)  {
      const Scalar * ptrQuery =  query;
      for (int i=0; i < nbQuery; ++i) {
        Scalar distanceToQuery;
        int nni;
        _tree.ApproximateNearestNeighborBestBinFirst(ptrQuery, 1000, &nni, &distanceToQuery);
        distance->push_back(distanceToQuery);
        indice->push_back(nni);
        ptrQuery+=_tree.NumDimension();
      }
      return true;
    }
    else  {
      //-- Not yet implemented
      return false;
    }
  }

  private :
  KdTree<Scalar> _tree;

};

} // namespace correspondence
} // namespace libmv

#endif // LIBMV_CORRESPONDENCE_ARRAYMATCHER_KDTREE_H_
