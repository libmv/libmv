// Copyright (c) 2007, 2008, 2009 libmv authors.
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

#ifndef LIBMV_MULTIVIEW_HOMOGRAPHY_H_
#define LIBMV_MULTIVIEW_HOMOGRAPHY_H_

#include <vector>

#include "libmv/numeric/numeric.h"

namespace libmv {

/** 
 * \brief Computes the homography that transforms x to y using the linear
 * method (DLT).
 * 
 * \param[out] H  The computed homography.
 * \param[in]  x  A 2xN matrix of column vectors.
 * \param[in]  y  A 2xN matrix of column vectors.
 * 
 * The estimated homography should approximatelly hold the condition y = H x.
 * 
 * \see HomographyFromCorrespondencesLinearRobust
 */
void HomographyFromCorrespondencesLinear(const Mat &x,
                                         const Mat &y,
                                         Mat3 *H);

/** 
 * \brief Computes the homography that transforms x to y using RANSAC.
 * 
 * \param[in]  x  A 2xN matrix of column vectors.
 * \param[in]  y  A 2xN matrix of column vectors.
 * \param[in]  max_error  Threshold to decide if a point inlier or outlier.
 * \param[out] H  The computed homography.
 * \param[out] inliers  Indices of points that are inliers according to the
 *                      given threhold.
 * 
 * This function uses RANSAC to robustly estimate the homography that maximizes
 * the number of inliers.  A point is considered inlier if
 * HomographyError(H, x, y) < max_error^2.
 * 
 * \see HomographyFromCorrespondencesLinear, HomographyError
 */
void HomographyFromCorrespondencesLinearRobust(const Mat &x,
                                               const Mat &y,
                                               double max_error,
                                               Mat3 *H,
                                               std::vector<int> *inliers);

/**
 * \brief Squared Euclidean distance between y and H x.
 * 
 * \param[in] H  An homography matrix.
 * \param[in] x  A point in Euclidean coordinates.
 * \param[in] y  A point in Euclidean coordinates.
 * \return The squared Euclidean distance between y and x transformed by H.
 */
double HomographyError(const Mat3 &H, const Vec2 &x, const Vec2 &y);
  
} // namespace libmv

#endif //LIBMV_MULTIVIEW_HOMOGRAPHY_H_
