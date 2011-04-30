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

#ifndef LIBMV_MULTIVIEW_ROTATION_PARAMETERIZATION_H_
#define LIBMV_MULTIVIEW_ROTATION_PARAMETERIZATION_H_

#include "libmv/numeric/numeric.h"

namespace libmv {

/** A parameterization of the 2D rotation matrix based on the Euler
 * angle.The rotation matrix R is built as follows
 *        |cos(angle) -sin(angle)|
 *    R = |sin(angle)  cos(angle)|
 */
template<typename T = double>
class Rotation2DEulerParameterization {
 public:
  typedef double Parameters;     // angle
  typedef Eigen::Matrix<T, 2, 2> Parameterized;  // R

  /// Convert from the angle to a rotation matrix R.
  static void To(const Parameters &a, Parameterized *R) { 
    Eigen::Rotation2D<T> r(a);
    *R = r.toRotationMatrix();
  }

  /// Convert from a rotation matrix R to the Euler angle.
  static void From(const Parameterized &R, Parameters *p) {
    Eigen::Rotation2D<T> r(0);
    *p = r.fromRotationMatrix(R).angle();
  }
};


/** A parameterization of the 2D rotation matrix based on two parameters:
 * S=sin(angle) and C=cos(angle). This method ensures that the rotation matrix 
 * is orthogonal thanks to a SVD decomposition.
 * The rotation matrix R is built as follows
 *        |C -S|
 *    R = |S  C|
 */
template<typename T = double>
class Rotation2DSCParameterization {
 public:
  typedef Eigen::Matrix<T, 2, 1> Parameters;     // sin, cos
  typedef Eigen::Matrix<T, 2, 2> Parameterized;  // R

  /// Convert from the 2 parameters (sin, cos) to a rotation matrix R.
  static void To(const Parameters &p, Parameterized *R) { 
    (*R)<<p(1),-p(0), // cos -sin 
          p(0), p(1); // sin  cos 
    // Ensures that R is orthogonal (using SDV decomposition)
    Eigen::JacobiSVD<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> > 
        svd(*R, Eigen::ComputeThinU | Eigen::ComputeThinV);
    const Eigen::Matrix<T, 2, 2> I2 = Mat2::Identity();
    *R = svd.matrixU() * I2 * svd.matrixV().transpose();  
    if (R->determinant() < 0)
      *R = -*R;  
    // TODO(julien) Consider this paper:
    // Polar decomposition algorithm proposed by [Higham 86]
    // SIAM J. Sci. Stat. Comput. Vol. 7, Num. 4, October 1986.
    // "Computing the Polar Decomposition - with Applications"
    // by Nicholas Higham.
  }

  /// Convert from a rotation matrix R to the 2 parameters (sin, cos).
  static void From(const Parameterized &R, Parameters *p) {
    *p << R.coeff(1,0), R.coeff(0,0);
  }
};

/** A parameterization of the 3D rotation matrix based on the Euler
 * angles (e0, e1, e2). The angle convention can be chosen with the
 * template arguments N0, N1 and N2 with a possible value of 0 (X),
 * 1 (Y) and 2 (Z). 
 * The default convention is Y-X-Z and in this case, the rotation 
 * matrix R is built as follows
 *        
 *    R = Rot(Y, e0) * Rot(X, e1) * Rot(Z, e2)
 * 
 *        |c1*c3 + s1*s2*s3, c3*s1*s2 - c1*s3, c2*s1|
 *    R = |           c2*s3,            c2*c3,   -s2|
 *        |c1*s2*s3 - c3*s1, s1*s3 + c1*c3*s2, c1*c2|
 */
template<typename T = double, 
         int N0 = 1, 
         int N1 = 0, 
         int N2 = 2>
class Rotation3DEulerParameterization {
 public:
  typedef Eigen::Matrix<T, 3, 1> Parameters;     // e0, e1, e2
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // R

  /// Convert from the angle to a rotation matrix R.
  static void To(const Parameters &p, Parameterized *R) { 
    Eigen::Matrix<T, 3, 1> n0, n1, n2; 
    n0 << ((N0 == 0)? 1.0: 0.0), ((N0 == 1)? 1.0: 0.0), ((N0 == 2)? 1.0: 0.0);
    n1 << ((N1 == 0)? 1.0: 0.0), ((N1 == 1)? 1.0: 0.0), ((N1 == 2)? 1.0: 0.0);
    n2 << ((N2 == 0)? 1.0: 0.0), ((N2 == 1)? 1.0: 0.0), ((N2 == 2)? 1.0: 0.0);
    *R = Eigen::AngleAxis<T>(p(0), n0)
       * Eigen::AngleAxis<T>(p(1), n1)
       * Eigen::AngleAxis<T>(p(2), n2);
}

  /// Convert from a rotation matrix R to the Euler angle.
  static void From(const Parameterized &R, Parameters *p) {
    *p = R.eulerAngles(N0, N1, N2);
  }
};
// TODO(julien) Write Quaternion, AngleAxis parameterizations

/** A parameterization of the 3D rotation matrix based on the normalized 
 * quaternions q = (w,x,y,z). The rotation matrix R is built as follows
 * 
 *        |1 - 2 y^2 - 2 z^2,    2 x y - 2 z w,      2 x z + 2 y w|
 *    R = |2 x y + 2 z w,    1 - 2 x^2 - 2 z^2,      2 y z - 2 x w|
 *        |2 x z - 2 y w,        2 y z + 2 x w,  1 - 2 x^2 - 2 y^2|
 */
template<typename T = double>
class Rotation3DQuaternionParameterization {
 public:
  typedef Eigen::Matrix<T, 4, 1> Parameters;     // q = (w,x,y,z)
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // R

  /// Convert from the quaternion to a rotation matrix R.
  static void To(const Parameters &p, Parameterized *R) { 
    *R = Eigen::Quaternion<T>(p);
  }

  /// Convert from a rotation matrix R to a quaternion.
  static void From(const Parameterized &R, Parameters *p) {
    *p = Eigen::Quaternion<T>(R).coeffs();
  }
};


/** A parameterization of the 3D rotation matrix based on the angle + axis 
 * representation p = (angle, x, y, z). The rotation matrix R is built as follows
 * 
 *        | xxC+c   xyC-zs  xzC+ys|
 *   R =  | yxC+zs  yyC+c   yzC-xs|
 *        | zxC-ys  zyC+xs  zzC+c |
 * 
 *  with c = cos(θ); s = sin(θ); C = 1-c
 * 
 */
template<typename T = double>
class Rotation3DAngleAxisParameterization {
 public:
  typedef Eigen::Matrix<T, 4, 1> Parameters;     // p = (angle, x, y, z)
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // R

  /// Convert from angle/axis to a rotation matrix R.
  static void To(const Parameters &p, Parameterized *R) { 
    *R = Eigen::AngleAxis<T>(p(0), p.template segment<3>(1));
  }

  /// Convert from a rotation matrix R to angle/axis.
  static void From(const Parameterized &R, Parameters *p) {
    Eigen::AngleAxis<T> aa(R);
    *p << aa.angle(), aa.axis();
  }
};
  

/** A parameterization of the 3D rotation matrix based on the exponential map 
 * p = (w0, w1, w2). The rotation matrix R is built as follows
 * 
 *   R =  exp([p]),  with [p] is the matrix cross product
 * 
 * \warning The method internally use the conversion to/from angle axis.
 *          This is not the proper exponential map algorithm.
 *          This function must be used only for conversion purpose.
 * \todo Wait/Implement Eigen Bug 29 - Implement matrix logarithm in order to 
 *       have to real method
 */
template<typename T = double>
class Rotation3DExponentialMapParameterization {
 public:
  typedef Eigen::Matrix<T, 3, 1> Parameters;     // p = (w0, w1, w2)
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // R

  /// Convert from exponential map to a rotation matrix R.
  static void To(const Parameters &p, Parameterized *R) { 
    Eigen::Matrix<T, 4, 1> aa;
    const T angle = p.norm();
    aa << angle, p / angle;
    Rotation3DAngleAxisParameterization<T>::To(aa, R);
    /// OR
    //Vec4 q;
    //const T np = p.norm();
    //q << (p / np) * std::sin(nv / 2.0), cos(np / 2.0);
    //Rotation3DQuaternionParameterization<T>::To(q, R);*/
    /// OR use std::exp(X);
  }

  /// Convert from a rotation matrix R to exponential map.
  static void From(const Parameterized &R, Parameters *p) {
    Eigen::Matrix<T, 4, 1> aa;
    Rotation3DAngleAxisParameterization<T>::From(R, &aa);
    *p = aa.template segment<3>(1);
    *p *= aa(0) / p->norm();
    /// OR
    //Mat3 m = real(std::log(R.template block<3, 3>(0, 0))); 
    //*p << -m(1, 2), m(0, 2), -m(0, 1);
    // TODO(julien) Wait/Implement Eigen Bug 29 - Implement matrix logarithm
  }
};


} // namespace libmv

#endif  // LIBMV_MULTIVIEW_ROTATION_PARAMETERIZATION_H_
