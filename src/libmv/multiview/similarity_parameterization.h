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

#ifndef LIBMV_MULTIVIEW_SIMILARITY_PARAMETERIZATION_H_
#define LIBMV_MULTIVIEW_SIMILARITY_PARAMETERIZATION_H_

#include "libmv/multiview/rotation_parameterization.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

/** A parameterization of the 2D homography similarity matrix that uses 4 
 * parameters. This method ensures that the rotation matrix is orthogonal
 * thanks to a SVD decomposition.
 * The homography matrix H is built from a list of 4 parameters 
 * (s*sin, s*cos, tx, ty) as follows
 *        |s*cos -s*sin  tx|
 *    M = |s*sin  s*cos  ty|
 *        |0      0      1 |
 */
template<typename T = double>
class Similarity2DSCParameterization {
 public:
  typedef Eigen::Matrix<T, 4, 1> Parameters;     // s*sin, s*cos, tx, ty
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // H

  /// Convert from the 4 parameters (s*sin, s*cos, tx, ty) to a H matrix.
  static bool To(const Parameters &p, Parameterized *h) {
    const T scale = std::sqrt(p(0)*p(0) + p(1)*p(1));
    if (std::abs(scale) < 1e-6)
      return false;
    Eigen::Matrix<T, 2, 2> R;
    Rotation2DSCParameterization<T>::To(p.template segment<2>(0) / scale, &R);
    h->template block<2, 2>(0, 0) = scale * R;
    h->col(2).template head<2>() << p(2), p(3);
    h->row(2) << 0.0, 0.0, 1.0;
    return true;
  }

  /// Convert from a H matrix to the 4 parameters (s*sin, s*cos, tx, ty).
  static void From(const Parameterized &h, Parameters *p) {
    *p << h(1, 0), h(0, 0), h(0, 2), h(1, 2);
  }
};

/** A parameterization of the 2D homography similarity matrix that uses 4 
 * parameters.
 * The homography matrix H is built from a list of 4 parameters 
 * (scale, angle, tx, ty) as follows
 *        |scale*cos(angle) -scale*sin(angle)  tx|
 *    M = |scale*sin(angle)  scale*cos(angle)  ty|
 *        |0                 0                 1 |
 */
template<typename T = double>
class Similarity2DSAParameterization {
 public:
  typedef Eigen::Matrix<T, 4, 1> Parameters;     // scale, angle, tx, ty
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // H

  /// Convert from the 4 parameters (scale, angle, tx, ty) to a H matrix.
  static void To(const Parameters &p, Parameterized *h) {   
    Eigen::Matrix<T, 2, 2> R;
    Rotation2DEulerParameterization<T>::To(p(0), &R);
    h->template block<2, 2>(0, 0) = p(0)*R;
    h->col(2).template head<2>() << p(2), p(3);
    h->row(2) << 0.0, 0.0, 1.0;
  }

  /// Convert from a H matrix to the 4 parameters (scale, angle, tx, ty).
  static void From(const Parameterized &h, Parameters *p) {
    (*p)(0) =  std::sqrt(std::pow(h(0, 0),2) + std::pow(h(0, 1),2));
    (*p)(0) = ((*p)(0) + std::sqrt(std::pow(h(0, 0),2) + std::pow(h(0, 1),2))) / 2.0;
    (*p)(1) = std::atan2(h.coeff(1,0) / (*p)(0), h.coeff(0,0) / (*p)(0));
    (*p)(2) = h(0, 2);
    (*p)(3) = h(1, 2);  
  }
};

/** A parameterization of the 2D similarity matrix that uses 7
 * parameters. The angle convention can be chosen with the
 * template arguments N0, N1 and N2 with a possible value of 0 (X),
 * 1 (Y) and 2 (Z). The default convention is Y-X-Z.
 * The euclidean matrix H is built from a list of 7 parameters 
 * (s, e0, e1, e2, tx, ty, tz) as follows
 *            |      tx|
 *        H = | sR   ty|
 *            |      tz|
 *            |0 0 0 1 |
 * where R is estimated by R = Rot(Y, e0) * Rot(X, e1) * Rot(Z, e2)
 * for N0 = 1, N1 = 0, N2 = 2.
 */
template<typename T = double, 
         int N0 = 1, 
         int N1 = 0, 
         int N2 = 2>
class Similarity3DEulerParameterization {
 public:
  typedef Eigen::Matrix<T, 7, 1> Parameters;// scale, ey, ex, ez, tx, ty, tz
  typedef Eigen::Matrix<T, 4, 4> Parameterized;  // H

  /// Convert from the 7 parameters (scale e0, e1, e2, tx, ty, tz) to a
  /// similarity matrix.
  static void To(const Parameters &p, Parameterized *h) { 
    Eigen::Matrix<T, 3, 3> R;
    Rotation3DEulerParameterization<T, N0, N1, N2>::To(p.template segment<3>(1), 
                                                       &R);
    h->template block<3, 3>(0, 0) = p(0) * R;
    h->col(3).template head<3>() << p(4), p(5), p(6);
    h->row(3) << 0.0, 0.0, 0.0, 1.0;
  }

  /// Convert from a similarity matrix to the 7 parameters 
  /// (scale, e0, e1, e2, tx, ty, tz).
  static void From(const Parameterized &h, Parameters *p) {
    T scale = h.template block<3, 3>(0, 0).determinant();
    scale = std::pow(std::abs(scale), 1.0/3.0);
    Eigen::Matrix<T, 3, 1> v;
    Rotation3DEulerParameterization<T, N0, N1, N2>::From(
        h.template block<3, 3>(0, 0) / scale, &v);
    *p << scale, v(0), v(1), v(2), h(0, 3), h(1, 3), h(2, 3);
  }
};


} // namespace libmv

#endif  // LIBMV_MULTIVIEW_SIMILARITY_PARAMETERIZATION_H_
