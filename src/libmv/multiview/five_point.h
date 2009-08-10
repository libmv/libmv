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
//
//
// Five point relative pose computation using Groebner basis.
// We follow the algorithm of [1] and apply some optimization hints of [2].
//
// [1] H. Stewénius, C. Engels and D. Nistér,  "Recent Developments on Direct
//     Relative Orientation",  ISPRS 2006
//
// [2] D. Nistér,  "An Efficient Solution to the Five-Point Relative Pose",
//     PAMI 2004


#ifndef LIBMV_MULTIVIEW_FIVE_POINT_H_
#define LIBMV_MULTIVIEW_FIVE_POINT_H_

#include "libmv/base/vector.h"
#include "libmv/numeric/numeric.h"


namespace libmv {

/** Computes the relative pose of two calibrated cameras from 5 correspondences.
 *
 * \param x1 Points in the first image.  One per column.
 * \param x2 Corresponding points in the second image. One per column.
 * \param E  A list of at most 10 candidate essential matrix solutions.
 */
void FivePointsRelativePose(const Mat2X &x1, const Mat2X &x2,
                            vector<Mat3> *E);


// In the following code, polynomials are expressed as vectors containing
// their coeficients in the basis of monomials:
// 
//  [xxx xxy xyy yyy xxz xyz yyz xzz yzz zzz xx xy yy xz yz zz x y z 1]
//
// Note that there is an error in Stewenius' paper.  In equation (9) they
// propose to use the basis:
//
//  [xxx xxy xxz xyy xyz xzz yyy yyz yzz zzz xx xy xz yy yz zz x y z 1]
//
// But this is not the basis used in the rest of the paper, neither in
// the code they provide.  I (pau) have spend 4 hours debugging and
// reverse engeniering their code to find the problem. :(
enum {
  coef_xxx,
  coef_xxy,
  coef_xyy,
  coef_yyy,
  coef_xxz,
  coef_xyz,
  coef_yyz,
  coef_xzz,
  coef_yzz,
  coef_zzz,
  coef_xx,
  coef_xy,
  coef_yy,
  coef_xz,
  coef_yz,
  coef_zz,
  coef_x,
  coef_y,
  coef_z,
  coef_1
};

// Compute the nullspace of the linear constraints given by the matches.
Mat FivePointsNullspaceBasis(const Mat2X &x1, const Mat2X &x2);

// Multiply two polynomials of degree 1.
Vec o1(const Vec &a, const Vec &b);

// Multiply a polynomial of degree 2, a, by a polynomial of degree 1, b.
Vec o2(const Vec &a, const Vec &b);

// Builds the polynomial constraint matrix M.
Mat FivePointsPolynomialConstraints(const Mat &E_basis);

// Gauss--Jordan elimination for the constraint matrix.
void FivePointsGaussJordan(Mat *Mp);
  
} // namespace libmv

#endif  // LIBMV_MULTIVIEW_FIVE_POINT_H_

