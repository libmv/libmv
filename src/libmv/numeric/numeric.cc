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

Mat Identity(int n) {
  Mat I(n, n);
  I = 0;
  for (int i = 0; i < n; ++i) {
    I(i, i) = 1;
  }
  return I;
}

Mat3 RotationAroundX(double angle) {
  double c = cos(angle);
  double s = sin(angle);
  Mat3 R;
  R = 1, 0,  0,
      0, c, -s,
      0, s,  c;
  return R;
}

Mat3 RotationAroundY(double angle) {
  double c = cos(angle);
  double s = sin(angle);
  Mat3 R;
  R =  c, 0, s,
       0, 1, 0,
      -s, 0, c;
  return R;
}

Mat3 RotationAroundZ(double angle) {
  double c = cos(angle);
  double s = sin(angle);
  Mat3 R;
  R = c, -s, 0,
      s,  c, 0,
      0,  0, 1;
  return R;
}


Mat Diag(const Vec &x) {
  int n = x.length();
  Mat A(n,n);
  A = 0;
  for (int i = 0; i < n; ++i) {
    A(i,i) = x(i);
  }
  return A;
}

double DeterminantLU(Mat *A) {
  assert(A->numRows() == A->numCols());
  int n = A->numRows();

  // This segfaults on pau's ubuntu.
  flens::DenseVector<flens::Array<int> > P;
  flens::trf(*A, P);

  double det = 1;
  for (int i = 0; i < n; ++i) {
    det *= (*A)(i, i);
  }
  // TODO(pau): compute the sign of the permutation P.
  double Psign = 1;

  return Psign * det;
}

void MinorMatrix(const Mat &A, int i, int j, Mat *M) {
  int n = A.numRows();
  int m = A.numCols();
  M->resize(n-1, m-1);

  if (0 < i) {
    if (0 < j) {
      (*M)(_(0, i - 1), _(0, j - 1)) = A(_(    0, i - 1), _(    0, j - 1));
    }
    if (j < m - 1) {
      (*M)(_(0, i - 1), _(j, m - 2)) = A(_(    0, i - 1), _(j + 1, m - 1));
    }
  }
  if (i < n - 1) {
    if (0 < j) {
      (*M)(_(i, n - 2), _(0, j - 1)) = A(_(i + 1, n - 1), _(    0, j - 1));
    }
    if (j < m - 1) {
      (*M)(_(i, n - 2), _(j, m - 2)) = A(_(i + 1, n - 1), _(j + 1, m - 1));
    }
  }
}

double DeterminantSlow(const Mat &A) {
  assert(A.numRows() == A.numCols());
  int n = A.numRows();
  if (n == 1) {
    return A(0, 0);
  }

  Mat minor_matrix;
  double det = 0;
  double sign = 1;
  for (int i = 0; i < n; ++i) {
    MinorMatrix(A, 0, i, &minor_matrix);
    det += sign * A(0, i) * DeterminantSlow(minor_matrix);
    sign *= -1;
  }
  return det;
}

double DeterminantSlow(const Mat3 &A) {
  Mat tmp;
  tmp = A;
  return DeterminantSlow(tmp);
}


double Cofactor(const Mat &A, int i, int j) {
  assert(A.numRows() == A.numCols());
  int n = A.numRows();
  if (n == 1) {
    return 1;
  }
  else {
    double sign;
    if ((i + j) % 2) {
      sign = -1;
    } else {
      sign = 1;
    }
    Mat M;
    MinorMatrix(A, i, j, &M);
    return sign * DeterminantSlow(M);
  }
}

void Adjoint(const Mat &A, Mat *B) {
  assert(A.numRows() == A.numCols());
  int n = A.numRows();
  B->resize(n, n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < n; ++j) {
      (*B)(i, j) = Cofactor(A, i, j);
    }
  }
}

void InverseSlow(const Mat &A, Mat *I) {
  assert(A.numRows() == A.numCols());
  int n = A.numRows();
  I->resize(n, n);

  Adjoint(A, I);
  TransposeInPlace(I);
  *I = *I / DeterminantSlow(A);
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
  (*stacked)(_, _(0, m1 - 1)) = left;
  (*stacked)(_, _(m1, m1 + m2 - 1)) = right;
}

void VerticalStack(const Mat &top, const Mat &bottom, Mat *stacked) {
  assert(top.numCols() == bottom.numCols());
  int n1 = top.numRows();
  int n2 = bottom.numRows();
  int m = top.numCols();

  stacked->resize(n1 + n2, m);
  (*stacked)(_(0, n1 - 1), _) = top;
  (*stacked)(_(n1, n1 + n2 - 1), _) = bottom;
}

}  // namespace libmv

