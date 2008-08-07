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


#include "libmv/numeric/numeric.h"

namespace libmv {

Mat Diag(const Vec &x) {
  int n = x.length();
  Mat A(n,n);
  A = 0;
  for (int i = 0; i < n; ++i) {
    A(i,i) = x(i);
  }
  return A;
}

void MeanAndVarianceAlongRows(const Mat &A,
                              Vec *mean_pointer,
                              Vec *variance_pointer) {
  Vec &mean = *mean_pointer;
  Vec &variance = *variance_pointer;
  int n = A.numRows();
  int m = A.numCols();
  mean.resize(n);
  variance.resize(n);

  for (int i = 0; i < n; ++i) {
    mean(i) = 0;
    variance(i) = 0;
    for (int j = 0; j < m; ++j) {
      double x = A(i, j);
      mean(i) += x;
      variance(i) += x * x;
    }
  }

  mean /= m;
  for (int i = 0; i < n; ++i) {
    variance(i) = variance(i) / m - Square(mean(i));
  }
}

void HorizontalStack(const Mat &left, const Mat &right, Mat *stacked) {
  assert(left.numRows() == left.numRows());
  int n = left.numRows();
  int m1 = left.numCols();
  int m2 = right.numCols();

  stacked->resize(n, m1 + m2);
  (*stacked)(_(0, n - 1), _(0, m1 - 1)) = left;
  (*stacked)(_(0, n - 1), _(m1, m1 + m2 - 1)) = right;
}

void VerticalStack(const Mat &top, const Mat &bottom, Mat *stacked) {
  assert(top.numCols() == bottom.numCols());
  int n1 = top.numRows();
  int n2 = bottom.numRows();
  int m = top.numCols();

  stacked->resize(n1 + n2, m);
  (*stacked)(_(0, n1 - 1), _(0, m - 1)) = top;
  (*stacked)(_(n1, n1 + n2 - 1), _(0, m - 1)) = bottom;
}

}  // namespace libmv

