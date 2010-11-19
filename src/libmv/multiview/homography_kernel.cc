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
#include "libmv/multiview/homography_kernel.h"

namespace libmv {
namespace homography {
namespace kernel {

void FourPointSolver::Solve(const Mat &x, const Mat &y, vector<Mat3> *Hs) {
  assert(2 == x.rows());
  assert(4 <= x.cols());
  assert(x.rows() == y.rows());
  assert(x.cols() == y.cols());

  int n = x.cols();
  MatX9 L = Mat::Zero(n * 2, 9);
  for (int i = 0; i < n; ++i) {
    int j = 2 * i;
    L(j, 0) = x(0, i);
    L(j, 1) = x(1, i);
    L(j, 2) = 1.0;
    L(j, 6) = -y(0, i) * x(0, i);
    L(j, 7) = -y(0, i) * x(1, i);
    L(j, 8) = -y(0, i);

    ++j;
    L(j, 3) = x(0, i);
    L(j, 4) = x(1, i);
    L(j, 5) = 1.0;
    L(j, 6) = -y(1, i) * x(0, i);
    L(j, 7) = -y(1, i) * x(1, i);
    L(j, 8) = -y(1, i);
  }

  Vec h;
  Nullspace(&L, &h);
  Mat3 H = Map<RMat3>(h.data());
  Hs->push_back(H);
}

}  // namespace kernel
}  // namespace homography
}  // namespace libmv
