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

#include "libmv/numeric/numeric.h"

namespace libmv {

void P_From_KRt(const Mat3 &K, const Mat3 &R, const Vec3 &t, Mat34 *P) {
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      (*P)(i,j) = R(i,j);
    }
    (*P)(i,3) = t(i);
  }
  (*P) = K * (*P);
}

void KRt_From_P(const Mat34 &P, Mat3 *Kp, Mat3 *Rp, Vec3 *tp) {
  Mat K(3,3);
  Mat R(3,3);
  Vec t(3,3);

  // Decompose using the RQ decomposition HZ A4.1.1 pag.579.
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
      K(i,j) = P(i,j);
    }
  }
  Mat Q(3,3);
  Q = 1, 0, 0,
      0, 1, 0,
      0, 0, 1;

  // Set K(2,1) to zero.
  if (K(2,1) != 0) {
    double c = -K(2,2);
    double s = K(2,1);
    double l = sqrt(c * c + s * s);
    c /= l; s /= l;
    Mat Qx(3,3);
    Qx = 1, 0, 0,
         0, c, -s,
         0, s, c;
    K = K * Qx;
    Q = transpose(Qx) * Q;
  }
  // Set K(2,0) to zero.
  if (K(2,0) != 0) {
    double c = K(2,2);
    double s = K(2,0);
    double l = sqrt(c * c + s * s);
    c /= l; s /= l;
    Mat Qy(3,3);
    Qy = c, 0, s,
         0, 1, 0,
        -s, 0, c;
    K = K * Qy;
    Q = transpose(Qy) * Q;
  }
  // Set K(1,0) to zero.
  if (K(1,0) != 0) {
    double c = -K(1,1);
    double s = K(1,0);
    double l = sqrt(c * c + s * s);
    c /= l; s /= l;
    Mat Qz;
    Qz = c,-s, 0,
         s, c, 0,
         0, 0, 1;
    K = K * Qz;
    Q = transpose(Qz) * Q;
  }

  R = Q;

  // Ensure that the diagonal is positive.
  // TODO(pau) Change this to ensure that:
  //  - K(0,0) > 0
  //  - K(2,2) = 1
  //  - det(R) = 1
  if( K(2,2) < 0) {
    K = -K;
    R = -R;
  }
  if( K(1,1) < 0 ) {
    Mat S(3,3);
    S = 1, 0, 0,
        0,-1, 0,
        0, 0, 1;
    K = K * S;
    R = S * R;
  }
  if( K(0,0) < 0 ) {
    Mat S(3,3);
    S =-1, 0, 0,
        0, 1, 0,
        0, 0, 1;
    K = K * S;
    R = S * R;
  }

  // compute the translation vector
  Vec p(3);
  p = P(0,3), P(1,3), P(2,3);
  // TODO(pau) This sould be done by a SolveLinearSystem(A, b, &x) call.
  Mat K1;
  InverseSlow(K, &K1);
  t = K1 * p;

  // scale K so that K(2,2) = 1
  K = K / K(2,2);

  *Kp = K;
  *Rp = R;
  *tp = t;
}

void HomogeneousToEuclidean(const Mat &H, Mat *X) {
  int d = H.numRows() - 1;
  int n = H.numCols();
  X->resize(d, n);
  for (int i = 0; i < n; ++i) {
    double h = H(d, i);
    for (int j = 0; j < d; ++j) {
      (*X)(j, i) = H(j, i) / h;
    }
  }
}

void EuclideanToHomogeneous(const Mat &X, Mat *H) {
  int d = X.numRows();
  int n = X.numCols();
  H->resize(d + 1, n);
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < d; ++j) {
      (*H)(j, i) = X(j, i);
    }
    (*H)(d, i) = 1;
  }
}

void Project(const Mat34 &P, const Mat &X, Mat *x) {
  Mat PP, XX, xx;
  PP = P;
  EuclideanToHomogeneous(X, &XX);
  xx = PP * XX;
  HomogeneousToEuclidean(xx, x);
}


}  // namespace libmv
