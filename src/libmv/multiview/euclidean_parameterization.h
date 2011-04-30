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

#ifndef LIBMV_MULTIVIEW_EUCLIDEAN_PARAMETERIZATION_H_
#define LIBMV_MULTIVIEW_EUCLIDEAN_PARAMETERIZATION_H_

#include "libmv/multiview/rotation_parameterization.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

/** A parameterization of the 2D euclidean matrix that uses 3 
 * parameters.The euclidean matrix H is built from a list of 3 parameters 
 * (angle, tx, ty) as follows
 *        |cos(angle) -sin(angle)  tx|
 *    M = |sin(angle)  cos(angle)  ty|
 *        |0                0      1 |
 */
template<typename T = double>
class Euclidean2DEulerParameterization {
 public:
  typedef Eigen::Matrix<T, 3, 1> Parameters;     // angle, tx, ty
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // H

  /// Convert from the 3 parameters (angle, tx, ty) to a H matrix.
  static void To(const Parameters &p, Parameterized *h) { 
    Eigen::Matrix<T, 2, 2> R;
    Rotation2DEulerParameterization<T>::To(p(0), &R);
    h->template block<2, 2>(0, 0) = R;
    h->col(2).template head<2>() << p(1), p(2);
    h->row(2) << 0.0, 0.0, 1.0;
    /*const T sin_a = std::sin(p(0));   
    const T cos_a = std::cos(p(0));   
    (*h)<<cos_a,-sin_a, p(1), // cos -sin tx
          sin_a, cos_a, p(2), // sin  cos ty
          0.0,     0.0,  1.0;*/
  }

  /// Convert from a H matrix to the 3 parameters (angle, tx, ty).
  static void From(const Parameterized &h, Parameters *p) {
    Rotation2DEulerParameterization<T>::From(h.template block<2, 2>(0, 0), 
                                             &(*p)(0));
    p->template segment<2>(1) << h(0, 2), h(1, 2);
    /*const double angle = std::atan2(h.coeff(1,0), h.coeff(0,0));
    *p << angle, h(0, 2), h(1, 2);*/
  }
};

/** A parameterization of the 2D homography euclidean matrix that uses 4
 * parameters. This method ensures that the rotation matrix is orthogonal
 * thanks to a SVD decomposition.
 * The homography matrix H is built from a list of 4 parameters 
 * (sin, cos, tx, ty) as follows
 *        |cos -sin  tx|
 *    M = |sin  cos  ty|
 *        |0    0    1 |
 */
template<typename T = double>
class Euclidean2DSCParameterization {
 public:
  typedef Eigen::Matrix<T, 4, 1> Parameters;     // sin, cos, tx, ty
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // H

  /// Convert from the 4 parameters (sin, cos, tx, ty) to a H matrix.
  static void To(const Parameters &p, Parameterized *h) { 
    /*(*h)<<p(1),-p(0), p(2), // cos -sin tx
          p(0), p(1), p(3), // sin  cos ty
          0.0,   0.0,  1.0;*/
    Eigen::Matrix<T, 2, 2> R;
    Rotation2DSCParameterization<T>::To(p.template segment<2>(0), &R);
    h->template block<2, 2>(0, 0) = R;
    h->col(2).template head<2>() << p(2), p(3);
    h->row(2) << 0.0, 0.0, 1.0;
  }

  /// Convert from a H matrix to the 4 parameters (sin, cos, tx, ty).
  static void From(const Parameterized &h, Parameters *p) {
    //*p << h(1, 0), h(0, 0), h(0, 2), h(1, 2);
    Eigen::Matrix<T, 2, 1> sin_cos;
    Rotation2DSCParameterization<T>::From(h.template block<2, 2>(0, 0), 
                                          &sin_cos);
    *p << sin_cos(0), sin_cos(1), h(0, 2), h(1, 2);
  }
};

/** A parameterization of the 2D euclidean matrix that uses 6 
 * parameters. The angle convention can be chosen with the
 * template arguments N0, N1 and N2 with a possible value of 0 (X),
 * 1 (Y) and 2 (Z). The default convention is Y-X-Z.
 * The euclidean matrix H is built from a list of 6 parameters 
 * (e0, e1, e2, tx, ty, tz) as follows
 *            |      tx|
 *        H = | R(e) ty|
 *            |      tz|
 *            |0 0 0 1 |
 * where R(e) = Rot(Y, e0) * Rot(X, e1) * Rot(Z, e2)
 * for N0 = 1, N1 = 0, N2 = 2.
 */
template<typename T = double, 
         int N0 = 1, 
         int N1 = 0, 
         int N2 = 2>
class Euclidean3DEulerParameterization {
 public:
  typedef Eigen::Matrix<T, 6, 1> Parameters;     // e0, e1, e2, tx, ty, tz
  typedef Eigen::Matrix<T, 4, 4> Parameterized;  // H

  /// Convert from the 6 parameters (e0, e1, e2, tx, ty, tz) to an
  /// Euclidean matrix.
  static void To(const Parameters &p, Parameterized *h) { 
    Eigen::Matrix<T, 3, 3> R;
    Rotation3DEulerParameterization<T, N0, N1, N2>::To(
        p.template segment<3>(0), &R);
    h->template block<3, 3>(0, 0) = R;
    h->col(3).template head<3>() << p(3), p(4), p(5);
    h->row(3) << 0.0, 0.0, 0.0, 1.0;
  }

  /// Convert from an Euclidean matrix to the 6 parameters 
  /// (e0, e1, e2, tx, ty, tz).
  static void From(const Parameterized &h, Parameters *p) {
    Eigen::Matrix<T, 3, 1> v;
    Rotation3DEulerParameterization<T, N0, N1, N2>::From(
        h.template block<3, 3>(0, 0), &v);
    *p << v, h(0, 3), h(1, 3), h(2, 3);
  }
};

/** A parameterization of the 2D euclidean matrix that uses 7 
 * parameters. The rotation is parametrized by a quaternion.
 * The euclidean matrix H is built from a list of 7 parameters 
 * (q0, q1, q2, q3, tx, ty, tz) as follows
 *            |      tx|
 *        H = | R(q) ty|
 *            |      tz|
 *            |0 0 0 1 |
 * where R(q) transforms the quaternion q into a rotation matrix
 *        |1 - 2 y^2 - 2 z^2,    2 x y - 2 z w,      2 x z + 2 y w|
 * R(q) = |2 x y + 2 z w,    1 - 2 x^2 - 2 z^2,      2 y z - 2 x w|
 *        |2 x z - 2 y w,        2 y z + 2 x w,  1 - 2 x^2 - 2 y^2|
 * with q = (w,x,y,z)
 */
template<typename T = double>
class Euclidean3DQuaternionParameterization {
 public:
  typedef Eigen::Matrix<T, 7, 1> Parameters;     // q0, q1, q2, q3, tx, ty, tz
  typedef Eigen::Matrix<T, 4, 4> Parameterized;  // H

  /// Convert from the 7 parameters (q0, q1, q2, q3, tx, ty, tz) to an
  /// Euclidean matrix.
  static void To(const Parameters &p, Parameterized *h) { 
    Eigen::Matrix<T, 3, 3> R;
    Rotation3DQuaternionParameterization<T>::To(
        p.template segment<4>(0), &R);
    h->template block<3, 3>(0, 0) = R;
    h->col(3).template head<3>() << p(4), p(5), p(6);
    h->row(3) << 0.0, 0.0, 0.0, 1.0;
  }

  /// Convert from an Euclidean matrix to the 7 parameters 
  /// (q0, q1, q2, q3, tx, ty, tz).
  static void From(const Parameterized &h, Parameters *p) {
    Eigen::Matrix<T, 4, 1> v;
    Rotation3DQuaternionParameterization<T>::From(
        h.template block<3, 3>(0, 0), &v);
    *p << v, h(0, 3), h(1, 3), h(2, 3);
  }
};
} // namespace libmv

#endif  // LIBMV_MULTIVIEW_EUCLIDEAN_PARAMETERIZATION_H_
