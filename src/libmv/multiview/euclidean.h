// Copyright (c) 2011 libmv authors.
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

#ifndef LIBMV_MULTIVIEW_EUCLIDEAN_H_
#define LIBMV_MULTIVIEW_EUCLIDEAN_H_

#include "libmv/numeric/numeric.h"

namespace libmv {

/** 2D Euclidean transformation estimation.
 * 
 * This function can be used in order to estimate the euclidean transformation
 * between two sets of points with known 2D correspondences.
 * 
 * \param[in] x1 The first 2xN matrix of euclidean points
 * \param[in] x2 The second 2xN matrix of euclidean points
 * \param[out] M The 3x3 euclidean transformation matrix  (3 dof)
 *          with the following parametrization
 *              |cos -sin  tx|
 *          M = |sin  cos  ty|
 *              |0    0    1 |
 *          such that  x2 = M * x1
 * \param[in] expected_precision The expected precision in order for instance 
 *        to accept almost euclidean matrices.
 * 
 * \return true if the transformation estimation has succeeded
 * 
 * \note Need at least 2 points 
 * \note Points coordinates must be normalized (euclidean)
 */
bool Euclidean2DFromCorrespondencesLinear(const Mat &x1,
                                          const Mat &x2,
                                          Mat3 *M,
                                          double expected_precision = 
                                            EigenDouble::dummy_precision());

/** 3D Euclidean transformation estimation.
 * 
 * This function can be used in order to estimate the euclidean transformation
 * between two sets of points with known 3D correspondences.
 * It relies on the affine transformation estimation first and SVD decomposition
 * in order to have an orthogonal rotation part.
 * 
 * \param[in] x1 The first 3xN matrix of euclidean points
 * \param[in] x2 The second 3xN matrix of euclidean points
 * \param[out] H The 4x4 euclidean transformation matrix  (6 dof)
 *               with the following parametrization such that  x2 = H * x1
 *            |      tx|
 *        H = |  R   ty|
 *            |      tz|
 *            |0 0 0 1 |
 *          
 * \param[in] expected_precision The expected precision in order for instance 
 *        to accept almost euclidean matrices.
 * 
 * \return true if the transformation estimation has succeeded
 * 
 * \note Need at least 4 non coplanar points 
 * \note Points coordinates must be normalized (euclidean)
 */
bool Euclidean3DFromCorrespondencesLinear(const Mat &x1,
                                          const Mat &x2,
                                          Mat4 *H,
                                          double expected_precision = 
                                            EigenDouble::dummy_precision());

/** Extract the translation and rotation angle from a 2D euclidean 
 * matrix.
 * 
 * \param[in]  H 2D euclidean matrix
 * \param[out] tr extracted 2D translation (x, y)
 * \param[out] angle extracted rotation angle (radian)
 *
 * \return true if success
 */
bool ExtractEuclidean2DCoefficients(const Mat3 &H,
                                    Vec2   *tr,
                                    double *angle);

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_EUCLIDEAN_H_
