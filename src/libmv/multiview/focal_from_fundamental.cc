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

void FindEpipoles(const Mat3 &F, Vec3 *e1, Vec3 *e2) {
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
// In the original image, the fundamental property is x1'Fx2 = 0 for all x1 and
// x2 that are corresponding scene points. Transforming the image we have
//
//   (T1x1)'F(T2x2) = 0
//
void TransformFundamentalForFocalCalculation(const Mat3 &F, Mat3 *Fp) {
  Vec3 e1, e2;
  FindEpipoles(F, &e1, &e2);
  Mat3 T, Fpp;
  RotationToEliminateY(e1, &T);
  TransposeInPlace(&T);  // Inverse!
  Fpp = F*transpose(T);
  RotationToEliminateY(e2, &T);
  *Fp = Fpp*T;
  //*Fp = Fpp*transpose(T);
}

}  // namespace libmv
