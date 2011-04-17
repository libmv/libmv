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

#ifndef LIBMV_MULTIVIEW_SIMILARITY_H_
#define LIBMV_MULTIVIEW_SIMILARITY_H_

#include "libmv/numeric/numeric.h"

namespace libmv {

/** 2D Similarity transformation estimation
 * 
 * This function can be used in order to estimate the similarity transformation
 * between two sets of points with known 2D correspondences.
 *
 * \param[in] x1 The first 2xN matrix of euclidean points
 * \param[in] x2 The second 2xN matrix of euclidean points
 * \param[out] M The 3x3 similarity transformation matrix (4 dof)
 *          with the following parametrization
 *              |s*cos -s*sin  tx|
 *          M = |s*sin  s*cos  ty|
 *              |0      0      1 |
 *          such that    x2 = M * x1
 * \param[in] expected_precision The expected precision in order for instance 
 *        to accept almost similarity matrices.
 * 
 * \return true if the transformation estimation has succeeded
 * 
 * \note Need at least 2 points 
 * \note Points coordinates must be normalized (euclidean)
 * \note This method ensures that the rotation matrix is orthogonal.
 */
bool Similarity2DFromCorrespondencesLinear(const Mat &x1,
                                           const Mat &x2,
                                           Mat3 *M,
                                           double expected_precision = 
                                             EigenDouble::dummy_precision());

/** 3D Similarity transformation estimation.
 * 
 * This function can be used in order to estimate the similarity transformation
 * between two sets of points with known 3D correspondences.
 * It relies on the affine transformation estimation first and SVD decomposition
 * in order to have an orthogonal rotation part.
 * 
 * \param[in] x1 The first 3xN matrix of euclidean points
 * \param[in] x2 The second 3xN matrix of euclidean points
 * \param[out] H The 4x4 similarity transformation matrix  (7 dof)
 *               with the following parametrization such that  x2 = H * x1
 *            |      tx|
 *        H = | s*R  ty|
 *            |      tz|
 *            |0 0 0 1 |
 *          
 * \param[in] expected_precision The expected precision in order for instance 
 *        to accept almost similarity matrices.
 * 
 * \return true if the transformation estimation has succeeded
 * 
 * \note Need at least 4 non coplanar points 
 * \note Points coordinates must be normalized (euclidean)
 */
bool Similarity3DFromCorrespondencesLinear(const Mat &x1,
                                          const Mat &x2,
                                          Mat4 *H,
                                          double expected_precision = 
                                            EigenDouble::dummy_precision());

/** Extract the translation, rotation angle and scale from a 2D similarity 
 * matrix.
 * 
 * \param[in] M 2D similarity matrix (4 dof)
 * \param[out] tr extracted 2D translation (x, y)
 * \param[out] angle extracted rotation angle (radian)
 * \param[out] scale extracted scale
 *
 * \return true if success
 */
bool ExtractSimilarity2DCoefficients(const Mat3 &M,
                                     Vec2   *tr,
                                     double *angle,
                                     double *scale);

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_SIMILARITY_H_
