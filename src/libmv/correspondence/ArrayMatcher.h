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

#ifndef LIBMV_CORRESPONDENCE_ARRAYMATCHER_H_
#define LIBMV_CORRESPONDENCE_ARRAYMATCHER_H_

#include "libmv/base/vector.h"

namespace libmv {
namespace correspondence  {

template < typename Scalar = float >
class ArrayMatcher
{
  public:
  virtual ~ArrayMatcher() {};

  /**
   * Build the matching structure
   *
   * \param[in] dataset   Input data.
   * \param[in] nbRows    The number of component.
   * \param[in] dimension Lenght of the data contained in the dataset.
   *
   * \return True if success.
   */
  virtual bool build( const Scalar * dataset, int nbRows, int dimension)=0;

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
  virtual bool searchNeighbour( const Scalar * query, int * indice, Scalar * distance)=0;


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
  virtual bool searchNeighbours( const Scalar * query, int nbQuery,
    vector<int> * indice, vector<Scalar> * distance, int NN)=0;
};

} // namespace correspondence
} // namespace libmv

#endif // LIBMV_CORRESPONDENCE_ARRAYMATCHER_H_
