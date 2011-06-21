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

#ifndef LIBMV_MULTIVIEW_FUNDAMENTAL_PARAMETERIZATION_H_
#define LIBMV_MULTIVIEW_FUNDAMENTAL_PARAMETERIZATION_H_

#include <vector>

#include <Eigen/Geometry>
#include <Eigen/QR>

#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

/** A parameterization of the fundamental matrix that uses 9 parameters, but is
 * constrained to have rank 2. The parameterization is a straigtforward SVD of
 * the F matrix into F = USV^T. S = diag(1 1/s^2 0) is one parameter. U and V
 * are 3x3 rotation matrices, and are parameterized by unnormalized
 * quaternions.
 *
 * The parameter vector breaks down into
 *
 *   u  - p[0..3]
 *   s  - p[4]
 *   vt - p[5..8]
 */
template<typename T>
class FundamentalRank2Parameterization {
 public:
  typedef Eigen::Matrix<T, 9, 1> Parameters;     // u, s, v
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // F = USV^T

  /** Convert from the 9 parameters to a F matrix.
   */
  static void To(const Parameters &p, Parameterized *f) {
    // TODO(keir): This is fantastically inefficient. Multiply through by the
    // zeros of S symbolically to speed this up.
    Eigen::Quaternion<T> u, vt;
    u  = p.template head<4>();
    vt = p.template tail<4>();

    // Use 1 / (1.0 + s^2) to prevent negative singular values. Use the inverse
    // so that the singular value ordering remains consistent.
    //
    // TODO(keir): This parameterization may be to stringent; consider also
    // letting the second element roam.
    T s = T(1.0) / (T(1.0) + p[4]*p[4]);

    Matrix<T, 3, 1> S;
    S << T(1), s, T(0);

    *f = u.toRotationMatrix() * S.asDiagonal() * vt.toRotationMatrix();
  }

  /** Convert from a F matrix to the 9 parameters.
   */
  static void From(const Parameterized &f, Parameters *p) {
    // This ignores the third singular value, which should be zero for a real F
    // matrix. If F is rank 3 rather than 2, the assumption that the third
    // singular value is zero produces the closest valid F matrix in the
    // Frobenius sense.
    Eigen::JacobiSVD<Parameterized> svd(f,
                                        Eigen::ComputeFullU |
                                        Eigen::ComputeFullV);

    // U and V are either rotations or reflections. Since the fundamental
    // matrix is invariant to scale changes, force U and V to rotations by
    // flipping their signs.
    Eigen::Quaternion<T> u, vt;
    u  =   svd.matrixU().determinant() > 0
       ?   svd.matrixU()
       : (-svd.matrixU()).eval();

    vt =   svd.matrixV().determinant() > 0
       ?   svd.matrixV().transpose().eval()
       :  (-svd.matrixV().transpose()).eval();

    // This may not be necessary.
    u.normalize();
    vt.normalize();

    (*p) << u.coeffs(),
            sqrt(svd.singularValues()[0] / svd.singularValues()[1] - T(1.0)),
            vt.coeffs();
  }
};

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_FUNDAMENTAL_PARAMETERIZATION_H_
