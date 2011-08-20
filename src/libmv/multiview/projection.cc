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

#include "libmv/multiview/projection.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

void P_From_KRt(const Mat3 &K, const Mat3 &R, const Vec3 &t, Mat34 *P) {
  P->block<3, 3>(0, 0) = R;
  P->col(3) = t;
  (*P) = K * (*P);
}

void KRt_From_P(const Mat34 &P, Mat3 *Kp, Mat3 *Rp, Vec3 *tp) {
  // Decompose using the RQ decomposition HZ A4.1.1 pag.579.
  Mat3 H = P.block(0, 0, 3, 3).transpose()
      .colwise().reverse().rowwise().reverse();
  // RQ decomposition
  Eigen::HouseholderQR<Mat3> qr(H);
  *Kp = qr.matrixQR().triangularView<Eigen::Upper>();
  *Rp = qr.householderQ();
  
  *Rp = (Rp->transpose().colwise().reverse().rowwise().reverse()).eval();
  *Kp = (Kp->transpose().colwise().reverse().rowwise().reverse()).eval();
  
  if (Rp->determinant() < 0) {
    Kp->col(0) = -Kp->col(0);
    Rp->row(0) = -Rp->row(0);
  }
  *Kp /= (*Kp)(2,2);
  if ((*Kp)(0,0) < 0) {
    Mat3 D;
    D << -1,  0, 0,
          0, -1, 0,
          0,  0, 1;
    *Kp = *Kp * D;
    *Rp =   D * *Rp;
  }
  if ((*Kp)(1,1) < 0) {
    (*Kp)(1,1) = -(*Kp)(1,1);
    Mat3 D;
    D << -1, 0, 0,
          0, 1, 0,
          0, 0,-1;
    *Rp =   D * *Rp;
  }
  Eigen::PartialPivLU<Mat3> lu(*Kp);//P.block(0, 0, 3, 3));
  *tp = lu.solve(P.col(3));
  if (!P.col(3).isApprox((*Kp)*(*tp))) {
  //TODO(julien) If the solution doesn't exist, do something
  }
}

void ProjectionShiftPrincipalPoint(const Mat34 &P,
                                   const Vec2 &principal_point,
                                   const Vec2 &principal_point_new,
                                   Mat34 *P_new) {
  Mat3 T;
  T << 1, 0, principal_point_new(0) - principal_point(0),
       0, 1, principal_point_new(1) - principal_point(1),
       0, 0,                                           1;
  *P_new = T * P;
}

void ProjectionChangeAspectRatio(const Mat34 &P,
                                 const Vec2 &principal_point,
                                 double aspect_ratio,
                                 double aspect_ratio_new,
                                 Mat34 *P_new) {
  Mat3 T;
  T << 1,                               0, 0,
       0, aspect_ratio_new / aspect_ratio, 0,
       0,                               0, 1;
  Mat34 P_temp;
  
  ProjectionShiftPrincipalPoint(P, principal_point, Vec2(0,0), &P_temp);
  P_temp = T * P_temp;
  ProjectionShiftPrincipalPoint(P_temp, Vec2(0,0), principal_point, P_new);
}

void HomogeneousToEuclidean(const Mat &H, Mat *X) {
  int d = H.rows() - 1;
  int n = H.cols();
  X->resize(d, n);
  for (size_t i = 0; i < n; ++i) {
    double h = H(d, i);
    for (int j = 0; j < d; ++j) {
      (*X)(j, i) = H(j, i) / h;
    }
  }
}

void HomogeneousToEuclidean(const Mat3X &h, Mat2X *e) {
  e->resize(2, h.cols());
  e->row(0) = h.row(0).array() / h.row(2).array();
  e->row(1) = h.row(1).array() / h.row(2).array();
}
void HomogeneousToEuclidean(const Mat4X &h, Mat3X *e) {
  e->resize(3, h.cols());
  e->row(0) = h.row(0).array() / h.row(3).array();
  e->row(1) = h.row(1).array() / h.row(3).array();
  e->row(2) = h.row(2).array() / h.row(3).array();
}

void HomogeneousToEuclidean(const Vec3 &H, Vec2 *X) {
  double w = H(2);
  *X << H(0) / w, H(1) / w;
}

void HomogeneousToEuclidean(const Vec4 &H, Vec3 *X) {
  double w = H(3);
  *X << H(0) / w, H(1) / w, H(2) / w;
}

void EuclideanToHomogeneous(const Mat &X, Mat *H) {
  int d = X.rows();
  int n = X.cols();
  H->resize(d + 1, n);
  H->block(0, 0, d, n) = X;
  H->row(d).setOnes();
}

void EuclideanToHomogeneous(const Vec2 &X, Vec3 *H) {
  *H << X(0), X(1), 1;
};

void EuclideanToHomogeneous(const Vec3 &X, Vec4 *H) {
  *H << X(0), X(1), X(2), 1;
}

// TODO(julien) Call conditioning.h/ApplyTransformationToPoints ?
void EuclideanToNormalizedCamera(const Mat2X &x, const Mat3 &K, Mat2X *n) {
 Mat3X x_image_h;
 EuclideanToHomogeneous(x, &x_image_h);
 Mat3X x_camera_h = K.inverse() * x_image_h;
 HomogeneousToEuclidean(x_camera_h, n);
}

void HomogeneousToNormalizedCamera(const Mat3X &x, const Mat3 &K, Mat2X *n) {
 Mat3X x_camera_h = K.inverse() * x;
 HomogeneousToEuclidean(x_camera_h, n);
}

double Depth(const Mat3 &R, const Vec3 &t, const Vec3 &X) {
  return (R*X)(2) + t(2);
}

double Depth(const Mat3 &R, const Vec3 &t, const Vec4 &X) {
  Vec3 Xe = X.head<3>() / X(3);
  return Depth(R, t, Xe);
}

}  // namespace libmv
