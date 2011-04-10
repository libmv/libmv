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

#include "libmv/multiview/affine.h"

namespace libmv {

// Parametrization
// a b x
// c d y
// 0 0 1

// It gives the following system A x = B :
// | X1 Y1 0  0  1 0 |  | a |   | X2 |
// | 0  0  X1 Y1 0 1 |  | b | = | Y2 |
//         ...          | c |     ..
//                      | d | 
//                      | x | 
//                      | y |
bool Affine2DFromCorrespondencesLinear(const Mat &x1, const Mat &x2,
                                       Mat3 *M,
                                       double expected_precision) {
  assert(3 == x1.rows());
  assert(3 >= x1.cols());
  assert(x1.rows() == x2.rows());
  assert(x1.cols() == x2.cols());

  const int n = x1.cols();
  Mat A = Mat::Zero(2*n, 6);
  Mat b = Mat::Zero(2*n, 1);
  for (int i = 0; i < n; ++i) {
    const int j= i * 2;
    A(j,0) =  x1(0,i);
    A(j,1) =  x1(1,i);
    A(j,4) =  1.0;
    //A(j,5) =  0.0;

    A(j+1,2) = x1(0,i);
    A(j+1,3) = x1(1,i);
    //A(j+1,4) = 0.0;
    A(j+1,5) = 1.0;

    b(j,0)   = x2(0,i);
    b(j+1,0) = x2(1,i);
  }
  // Solve Ax=B
  Vec x = A.fullPivLu().solve(b);
  if ((A * x).isApprox(b, expected_precision))  {
    // Configure output matrix :
    (*M)<<x(0), x(1), x(4), // a b x
          x(2), x(3), x(5), // c d y
          0.0,  0.0,  1.0;
    return true;
  } else {
    return false;
  }
}

} // namespace libmv
