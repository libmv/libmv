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

#ifndef LIBMV_MULTIVIEW_PROJECTION_H_
#define LIBMV_MULTIVIEW_PROJECTION_H_

#include <vector>

#include "libmv/numeric/numeric.h"

namespace libmv {

void P_From_KRt(const Mat3 &K, const Mat3 &R, const Vec3 &t, Mat34 *P);
void KRt_From_P(const Mat34 &P, Mat3 *K, Mat3 *R, Vec3 *t);

// Applies a change of basis to the image coordinates of the projection matrix
// so that the principal point becomes principal_point_new.
void ProjectionShiftPrincipalPoint(const Mat34 &P,
                                   const Vec2 &principal_point,
                                   const Vec2 &principal_point_new,
                                   Mat34 *P_new);
                                    
// Applies a change of basis to the image coordinates of the projection matrix
// so that the aspect ratio becomes aspect_ratio_new.  This is done by
// stretching the y axis.  The aspect ratio is defined as the quotient between
// the focal length of the y and the x axis.
void ProjectionChangeAspectRatio(const Mat34 &P,
                                 const Vec2 &principal_point,
                                 double aspect_ratio,
                                 double aspect_ratio_new,
                                 Mat34 *P_new);

void HomogeneousToEuclidean(const Mat &H, Mat *X);
void HomogeneousToEuclidean(const Vec3 &H, Vec2 *X);
void HomogeneousToEuclidean(const Vec4 &H, Vec3 *X);
inline Vec2 HomogeneousToEuclidean(const Vec3 &h) {
  return h.start<2>() / h(2);
}
inline Vec3 HomogeneousToEuclidean(const Vec4 &h) {
  return h.start<3>() / h(3);
}

void EuclideanToHomogeneous(const Mat &X, Mat *H);
void EuclideanToHomogeneous(const Vec2 &X, Vec3 *H);
void EuclideanToHomogeneous(const Vec3 &X, Vec4 *H);
inline Vec3 EuclideanToHomogeneous(const Vec2 &x) {
  return Vec3(x(0), x(1), 1);  
}
inline Vec4 EuclideanToHomogeneous(const Vec3 &x) {
  return Vec4(x(0), x(1), x(2), 1);
}

void Project(const Mat34 &P, const Mat &X, Mat *x);

inline Mat2X Project(const Mat34 &P, const Mat4X &X) {
  Mat2X x(2, X.cols());

  Mat3X xs = P*X;
  xs.row(0) = xs.row(0).cwise() / xs.row(2);

  for (int c = 0; c < X.cols(); ++c) {
    Vec3 hx = P * X.col(c);
    x(0, c) = hx(0) / hx(2);
    x(1, c) = hx(1) / hx(2);
  }
  return x;
}

inline Mat2X Project(const Mat34 &P, const Mat3X &X) {
  Mat2X x(2, X.cols());
  for (int c = 0; c < X.cols(); ++c) {
    Vec4 HX;
    HX.start<3>() = X.col(c);
    HX(3) = 1.0;
    Vec3 hx = P * HX;
    x(0, c) = hx(0) / hx(2);
    x(1, c) = hx(1) / hx(2);
  }
  return x;
}

double Depth(const Mat3 &R, const Vec3 &t, const Vec3 &X);

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_PROJECTION_H_
