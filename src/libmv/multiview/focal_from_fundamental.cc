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
// http://axiom.anu.edu.au/~hartley/Papers/focal-lengths/focal.pdf

#include "libmv/numeric/numeric.h"

namespace libmv {

void EpipolesFromFundamental(const Mat3 &F, Vec3 *e1, Vec3 *e2) {
  Mat3 Fp = F;
  double s1 = Nullspace(&Fp, e1);  // Left nullspace.
  Fp = F;
  TransposeInPlace(&Fp);
  double s2 = Nullspace(&Fp, e2);  // Rigth nullspace.
  // TODO(keir): Check that s1 and s2 are small.
  (void) s1;
  (void) s2;
}

// Make a transformation that forces the second component of x to zero.
// sx + cy = 0              s = -y / r
// cx - sy > 0      ===>    c =  x / r
// s^2 + c^2 = 1            r = |(x, y)|
void RotationToEliminateY(const Vec3 &x, Mat3 *T) {
  double r = sqrt(Square(x(0)) + Square(x(1)));
  double c =  x(0) / r;
  double s = -x(1) / r;
  *T = c, -s, 0,
       s,  c, 0,
       0,  0, 1;
}

// Rotate each image to cause the y component of both epipoles to become zero.
// When this happens, the fundamental matrix takes on a special form.
// 
// In the original image, the fundamental property is x2'Fx1 = 0 for all x1 and
// x2 that are corresponding scene points. Transforming the image we have
//
//   (T2x2)' F_rotated (T1x1) = 0.
//
// Thus, F_rotated = T2 F T1'.
void FundamentalAlignEpipolesToXAxis(const Mat3 &F, Mat3 *F_rotated) {
  Vec3 e1, e2;
  EpipolesFromFundamental(F, &e1, &e2);
  Mat3 T1, T2, T2_F;
  RotationToEliminateY(e1, &T1);
  RotationToEliminateY(e2, &T2);
  T2_F = T2 * F;
  *F_rotated = T2_F * transpose(T1);
}

// Given a fundamental matrix of two cameras and their principal points,
// computes the fundamental matrix corresponding to the same cameras with the
// principal points shifted to a new location.
void FundamentalShiftPrincipalPoints(const Mat3 &F,
                                     const Vec2 &p1,
                                     const Vec2 &p1_new,
                                     const Vec2 &p2,
                                     const Vec2 &p2_new,
                                     Mat3 *F_new) {
  Mat T1(3,3), T2(3,3), T1_inv(3,3), T2_inv(3,3),
      F_T1_inv(3,3), T2_inv_trans_F_T1_inv(3,3);
  T1 = 1, 0, p1_new(0) - p1(0),
       0, 1, p1_new(1) - p1(1),
       0, 0,                 1;
  T2 = 1, 0, p2_new(0) - p2(0),
       0, 1, p2_new(1) - p2(1),
       0, 0,                 1;
  InverseSlow(T1, &T1_inv);
  InverseSlow(T2, &T2_inv);
  Mat F_tmp;
  F_tmp = F;
  F_T1_inv = F_tmp * T1_inv;
  T2_inv_trans_F_T1_inv = transpose(T2_inv) * F_T1_inv;
  *F_new = T2_inv_trans_F_T1_inv;
}

void FocalFromFundamental(const Mat3 &F,
                          const Vec2 &principal_point1,
                          const Vec2 &principal_point2,
                          double *f1,
                          double *f2) {
  Mat3 F_shifted, F_rotated;
  Vec2 zero2;
  zero2 = 0,0;
  FundamentalShiftPrincipalPoints(F,
                                  principal_point1, zero2,
                                  principal_point2, zero2,
                                  &F_shifted);

  FundamentalAlignEpipolesToXAxis(F_shifted, &F_rotated);

  Vec3 e1, e2;
  EpipolesFromFundamental(F_rotated, &e1, &e2);

  Mat3 T1, T2;
  T1 = 1 / e1(2), 0,          0,
               0, 1,          0,
               0, 0, -1 / e1(0);
  T2 = 1 / e2(2), 0,          0,
               0, 1,          0,
               0, 0, -1 / e2(0);
  Mat3 T2_F, A;
  T2_F = T2 * F_rotated;
  A = T2_F * T1;

  double a = A(0,0);
  double b = A(0,1);
  double c = A(1,0);
  double d = A(1,1);

  // TODO(pau) Should check we are not dividing by 0.
  double f1_square = - (a * c * Square(e1(0)))
                     / (a * c * Square(e1(2)) + b * d);
  double f2_square = - (a * b * Square(e2(0)))
                     / (a * b * Square(e2(2)) + c * d);

  // TODO(pau) Should check that the squares are positive.
  *f1 = sqrt(f1_square);
  *f2 = sqrt(f2_square);
}

}  // namespace libmv
