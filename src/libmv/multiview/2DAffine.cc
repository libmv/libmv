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

#include "libmv/multiview/2DAffine.h"
#include <iostream>
namespace libmv {

bool Affine2D_FromCorrespondencesLinear(const Mat &x1, const Mat &x2,
                                            Mat3 *M)  {

  assert(2 == x1.rows());
  assert(2 <= x1.cols());
  assert(x1.rows() == x2.rows());
  assert(x1.cols() == x2.cols());

  int n = x1.cols();
  Mat A(4, 4);
  A.setZero();
  Mat B(4, 1);
  B.setZero();
  for (int i = 0; i < n; ++i) {
    int j=(i%2)*2;
    A(j,0) += -x1(1,i);
    A(j,1) +=  x1(0,i);
    A(j,2) +=  1.0;
    A(j,3) =  0.0;

    A(j+1,0) += x1(0,i);
    A(j+1,1) += x1(1,i);
    A(j+1,2) = 0.0;
    A(j+1,3) += 1.0;

    // x'
    B(j,0)   += x2(0,i);
    B(j+1,0) += x2(1,i);
  }
  // Does A is invertible ?
  if( fabs(A.determinant()) < 1e-1) {
     return false;
  }
  Mat res = A.inverse() * B;

  // Configure output matrix :
  (*M)(0,0) = res(1,0);   (*M)(0,1) = res(0,0);// cos sin
  (*M)(1,0) = -res(0,0);  (*M)(1,1) = res(1,0);// sin cos
  (*M)(0,2) = res(2,0);// tx
  (*M)(1,2) = res(3,0);// ty
  (*M)(2,0) = (*M)(2,1) = 0.0;
  // Force homogeneous coord
  (*M)(2,2) = 1.0;
  return true;
}

}
