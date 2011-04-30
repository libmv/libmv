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

#ifndef LIBMV_MULTIVIEW_AFFINE_PARAMETERIZATION_H_
#define LIBMV_MULTIVIEW_AFFINE_PARAMETERIZATION_H_

#include "libmv/numeric/numeric.h"

namespace libmv {

/** A parameterization of the 2D affine homography matrix that uses 6 
 * parameters.The homography matrix H is built from a list of 6 parameters 
 * (a, b, c, d, x ,y) as follows
 *       |a b x|
 *   h = |c d y|
 *       |0 0 1|
 */
template<typename T = double>
class Affine2DGenericParameterization {
 public:
  typedef Eigen::Matrix<T, 6, 1> Parameters;     // a, b, c, d, x, y
  typedef Eigen::Matrix<T, 3, 3> Parameterized;  // H

  /// Convert from the 6 parameters to a H matrix.
  static void To(const Parameters &p, Parameterized *h) {    
    *h << p(0), p(1), p(4),
          p(2), p(3), p(5),
          0.0,   0.0,  1.0;
  }

  /// Convert from a H matrix to the 6 parameters.
  static void From(const Parameterized &h, Parameters *p) {
    *p << h(0, 0), h(0, 1), h(1, 0), h(1, 1), h(0, 2), h(1, 2);
  }
};

/** A parameterization of the 3D affine homography matrix that uses 6 
 * parameters.The homography matrix H is built from a list of 6 parameters 
 * (a, b, ... h, i, x ,y, z) as follows
 *              |a b c x|
 *          h = |d e f y|
 *              |g h i z|
 *              |0 0 0 1|
 */
template<typename T = double>
class Affine3DGenericParameterization {
 public:
  typedef Eigen::Matrix<T, 12, 1> Parameters;    // a, b, ... h, i, x ,y, z
  typedef Eigen::Matrix<T, 4, 4> Parameterized;  // H

  /// Convert from the 12 parameters to a H matrix.
  static void To(const Parameters &p, Parameterized *h) {    
    *h << p(0), p(1), p(2), p(9),  // a b c x
          p(3), p(4), p(5), p(10), // d e f y
          p(6), p(7), p(8), p(11), // g h i z
          0.0,  0.0,  0.0,  1.0;
  }

  /// Convert from a H matrix to the 12 parameters.
  static void From(const Parameterized &h, Parameters *p) {
    *p << h(0, 0), h(0, 1), h(0, 2),
          h(1, 0), h(1, 1), h(1, 2),
          h(2, 0), h(2, 1), h(2, 2),
          h(0, 3), h(1, 3), h(2, 3);
  }
};

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_AFFINE_PARAMETERIZATION_H_
