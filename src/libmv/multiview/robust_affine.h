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

#ifndef LIBMV_MULTIVIEW_ROBUST_AFFINE_H_
#define LIBMV_MULTIVIEW_ROBUST_AFFINE_H_

#include "libmv/base/vector.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

/** Robust 2D affine transformation estimation
 * 
 * This function estimates robustly the 2d affine matrix between two dataset 
 * of 2D point (image coords space). The 2d affine solver relies on the 3 
 * points solution. 
 * 
 * \param[in] x1 The first 2xN matrix of euclidean points
 * \param[in] x2 The second 2xN matrix of euclidean points
 * \param[in] max_error maximum error (in pixels)
 * \param[out] H The 3x3 affine transformation matrix  (6 dof)
 *          with the following parametrization
 *              |a b tx|
 *          H = |c d ty|
 *              |0 0 1 |
 *          such that  x2 = H * x1
 * \param[out] inliers the indexes list of the detected inliers
 * \param[in] outliers_probability outliers probability (in ]0,1[).
 * The number of iterations is controlled using the following equation:
 *    n_iter = log(outliers_prob) / log(1.0 - pow(inlier_ratio, min_samples)))
 * The more this value is high, the less the function selects ramdom samples.
 * 
 * \return the best error found (in pixels), associated to the solution H
 * 
 * \note The function needs at least 3 points 
 * \note The overall iteration limit is 1000
 */
double Affine2DFromCorrespondences3PointRobust(
    const Mat &x1,
    const Mat &x2,
    double max_error,
    Mat3 *H,
    vector<int> *inliers = NULL,
    double outliers_probability = 1e-2);

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_ROBUST_AFFINE_H_
