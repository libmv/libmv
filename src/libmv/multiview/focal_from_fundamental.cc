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
#include "libmv/multiview/projection.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/focal_from_fundamental.h"

namespace libmv {

void EpipolesFromFundamental(const Mat3 &F, Vec3 *e1, Vec3 *e2) {
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
  *T << c, -s, 0,
        s,  c, 0,
        0,  0, 1;
}

// Rotate each image to cause the y component of both epipoles to become zero.
// When this happens, the fundamental matrix takes on a special form.
// 
// In the original image, the fundamental property is x2'Fx1 = 0 for all x1 and
// x2 that are corresponding scene points. Transforming the image we have
//
//   (T2x2)' F_rotated (T1x1) = 0.
//
// Thus, F_rotated = T2 F T1'.
void FundamentalAlignEpipolesToXAxis(const Mat3 &F, Mat3 *F_rotated) {
  Vec3 e1, e2;
  EpipolesFromFundamental(F, &e1, &e2);
  Mat3 T1, T2, T2_F;
  RotationToEliminateY(e1, &T1);
  RotationToEliminateY(e2, &T2);
  T2_F = T2 * F;
  *F_rotated = T2 * F * T1.transpose();
}

// Given a fundamental matrix of two cameras and their principal points,
// computes the fundamental matrix corresponding to the same cameras with the
// principal points shifted to a new location.
void FundamentalShiftPrincipalPoints(const Mat3 &F,
                                     const Vec2 &p1,
                                     const Vec2 &p1_new,
                                     const Vec2 &p2,
                                     const Vec2 &p2_new,
                                     Mat3 *F_new) {
  Mat3 T1, T2;
  T1 << 1, 0, p1_new(0) - p1(0),
        0, 1, p1_new(1) - p1(1),
        0, 0,                 1;
  T2 << 1, 0, p2_new(0) - p2(0),
        0, 1, p2_new(1) - p2(1),
        0, 0,                 1;
  *F_new = T2.inverse().transpose() * F * T1.inverse();
}

void FocalFromFundamental(const Mat3 &F,
                          const Vec2 &principal_point1,
                          const Vec2 &principal_point2,
                          double *f1,
                          double *f2) {
  Mat3 F_shifted, F_rotated;
  Vec2 zero2;
  zero2 << 0, 0;
  FundamentalShiftPrincipalPoints(F,
                                  principal_point1, zero2,
                                  principal_point2, zero2,
                                  &F_shifted);

  FundamentalAlignEpipolesToXAxis(F_shifted, &F_rotated);

  Vec3 e1, e2;
  EpipolesFromFundamental(F_rotated, &e1, &e2);

  Mat3 T1, T2;
  T1 << 1 / e1(2), 0,          0,
                0, 1,          0,
                0, 0, -1 / e1(0);
  T2 << 1 / e2(2), 0,          0,
                0, 1,          0,
                0, 0, -1 / e2(0);

  Mat3 A = T2 * F_rotated * T1;

  double a = A(0,0);
  double b = A(0,1);
  double c = A(1,0);
  double d = A(1,1);

  // TODO(pau) Should check we are not dividing by 0.
  double f1_square = - (a * c * Square(e1(0)))
                     / (a * c * Square(e1(2)) + b * d);
  double f2_square = - (a * b * Square(e2(0)))
                     / (a * b * Square(e2(2)) + c * d);

  // TODO(keir) deterimne a sensible thing to do in this case.
  assert(f1_square > 0.);
  assert(f2_square > 0.);
  *f1 = sqrt(f1_square);
  *f2 = sqrt(f2_square);
}



//TODO(pau) Move this libmv/numeric and add tests.
template<class Tf, typename T>
T GoldenRatioSearch(const Tf &f, T a, T b, T tol, int max_it) {
  // Golden ration search.
  double c = a + 0.618033988 * (b - a);
  double fa = f(a);
  double fb = f(b);
  double fc = f(c);
  for (int i = 0; i < max_it && fabs(a - b) >= tol; ++i) {
    VLOG(3) << "a: " << a << " b: " << b << " c: " << c << " fc: " << fc << "\n";

    double d = a + 0.618033988 * (c - a);
    double fd = f(d);
    if (fd < fc) {
      b = c; fb = fc;
      c = d; fc = fd;
    } else {
      a = b; fa = fb;
      b = d; fb = fd;
    }
  }
  return c;
}


class FocalReprojectionError {
 public:
  FocalReprojectionError(const Mat3 &F,
                         const Vec2 &principal_point,
                         const Mat2X &x1,
                         const Mat2X &x2) 
      : F_(F), principal_point_(principal_point), x1_(x1), x2_(x2) {
  }
  
  double operator()(double focal) const {
    Mat3 K;
    K << focal,      0, principal_point_(0),
              0, focal, principal_point_(1),
              0,     0,                  1;
          
    Mat3 E;
    EssentialFromFundamental(F_, K, K, &E);
    
    Mat3 R;
    Vec3 t;
    MotionFromEssentialAndCorrespondence(E, K, x1_.col(0), K, x2_.col(0),
                                         &R, &t);
    
    std::vector<Mat34> Ps(2);
    P_From_KRt(K, Mat3::Identity(), Vec3::Zero(), &Ps[0]);
    P_From_KRt(K, R, t, &Ps[1]);
    
    double error = 0;
    for (int j = 0; j < x1_.cols(); ++j) {
      Vec4 X;
      Mat2X x(2,2);
      x.col(0) = x1_.col(j);
      x.col(1) = x2_.col(j);
      NViewTriangulate(x, Ps, &X);
      Vec3 x1_reproj = Ps[0] * X;
      Vec3 x2_reproj = Ps[1] * X;
      double threshold = 1.;
      double d1 = Depth(Mat3::Identity(), Vec3::Zero(), X);
      double d2 = Depth(R, t, X);
      if (d1 < 0 || d2 < 0) {
        error += 2 * Square(threshold);
      } else {
        error += std::min(Square(threshold),
            (x1_.col(j) - HomogeneousToEuclidean(x1_reproj)).squaredNorm());
        error += std::min(Square(threshold),
            (x2_.col(j) - HomogeneousToEuclidean(x2_reproj)).squaredNorm());
      }
    }
    return error;
  }
  
 private:
  const Mat3 &F_;
  const Vec2 &principal_point_;
  const Mat2X &x1_;
  const Mat2X &x2_;
};


//TODO(pau) Move this to libmv/numeric and add tests.
static double Lerp(double x, double x0, double y0, double x1, double y1) {
  return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

void FocalFromFundamentalExhaustive(const Mat3 &F,
                                    const Vec2 &principal_point,
                                    const Mat2X &x1,
                                    const Mat2X &x2,
                                    double min_focal,
                                    double max_focal,
                                    int n_samples,
                                    double *focal) {
  FocalReprojectionError error(F, principal_point, x1, x2);
  
  // Exhaustive search.
  int best_focal = 0;
  double best_error = std::numeric_limits<double>::max();
  
  for (int i = 0; i < n_samples; ++i) {
    double f = Lerp(i, 0, min_focal, n_samples - 1, max_focal);
    double e = error(f);
    
    if (e < best_error) {
      best_error = e;
      best_focal = i;
    }
    VLOG(3) << "focal: " << f << "  error: " << e << "\n";
  }  
  
  // Golden ration search.
  double a = Lerp(best_focal - 1, 0, min_focal, n_samples - 1, max_focal);
  double b = Lerp(best_focal + 1, 0, min_focal, n_samples - 1, max_focal);

  *focal = GoldenRatioSearch(error, a, b, 1e-8, 99999);
}


}  // namespace libmv
