// Copyright (c) 2008, 2009 libmv authors.
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

#include "libmv/logging/logging.h"
#include "libmv/multiview/minimalPanoramic_kernel.h" 
#include "libmv/multiview/minimalPanoramic.h"

namespace libmv {
namespace panography {
namespace kernel {
  
void TwoPointSolver::Solve(const Mat &x1, const Mat &x2, vector<Mat3> *Hs) {
  assert(2 == x1.rows());
  assert(2 <= x1.cols());
  assert(x1.rows() == x2.rows());
  assert(x1.cols() == x2.cols());

  // Algorithm :
  // .Solve for focals.
  // .For each focal :
  //  - estimate the RotationMatrix.

  vector<double> v_focals;
  F_FromCorrespondance_2points( x1.col(0) , x1.col(1), x2.col(0), x2.col(1),
    &v_focals);

  Mat x1H, x2H;
  EuclideanToHomogeneous(x1,&x1H);
  EuclideanToHomogeneous(x2,&x2H);

  for(int i =0; i< v_focals.size(); ++i)  {
    Mat3 K1 = Mat3::Identity() * v_focals[i];
    K1(2,2) = 1;
    Mat3 K2 = K1;

    Mat3 R;
    GetR_FixedCameraCenter( x1H, x2H, K1, K2, &R);

    (*Hs).push_back( K2 * R * K1.inverse() );
  }

}

}  // namespace kernel
}  // namespace panography
}  // namespace libmv
