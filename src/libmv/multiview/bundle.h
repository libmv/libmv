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


#ifndef LIBMV_MULTIVIEW_BUNDLE_H_
#define LIBMV_MULTIVIEW_BUNDLE_H_

#include <vector>

#include "libmv/numeric/numeric.h"

namespace libmv {

/**
 * \brief Euclidean bundle adjustment given a full observation.
 *
 * \param[in]     x  The observed projections. x[j].col(i) is the projection of
 *                   point i in camera j.
 * \param[in/out] K  The calibration matrices.  For now it assume a single
 *                   calibration matrix, so all K[i] should be the same.
 * \param[in/out] R  The rotation matrices.
 * \param[in/out] t  The translation vectors.
 * \param[in/out] X  The point structure.
 *
 * All the points are assumed to be observed in all images.
 * We use the convention x = K * (R * X + t).
 */
void EuclideanBAFull(const std::vector<Mat2X> &x,
                     std::vector<Mat3> *K,
                     std::vector<Mat3> *R,
                     std::vector<Vec3> *t,
                     Mat3X *X);

                                                  
} // namespace libmv

#endif  // LIBMV_MULTIVIEW_BUNDLE_H_
