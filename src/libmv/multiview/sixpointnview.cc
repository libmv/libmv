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

#include <set>
#include <vector>

#include "third_party/glog/src/glog/logging.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/sixpointnview.h"
#include "libmv/numeric/numeric.h"
#include "libmv/numeric/poly.h"

namespace libmv {

template<typename TMatP, typename TMatA, typename TMatB>
static void FivePointCameraPencil(const TMatP &points, TMatA *A, TMatB *B) {
  CHECK_EQ(5, points.cols());
  Mat3 H;
  PreconditionerFromPoints(points, &H);
  Mat35 design = H * points;

  Vec5 v1, v2;
  Nullspace2(&design, &v1, &v2);

  Mat34 five_points = design.block<3,4>(0, 0);
  Mat34 tmpA = five_points;
  Mat34 tmpB = five_points;
  for (int r = 0; r < 3; ++r) {
    // The last component of v1 and v2 is ignored, because it is a scale factor.
    tmpA.row(r) = five_points.row(r).cwise() * v1.start(4).transpose();
    tmpB.row(r) = five_points.row(r).cwise() * v2.start(4).transpose();
  }
  Mat3 Hinv = H.inverse();
  *A = Hinv * tmpA;
  *B = Hinv * tmpB;
}

static Vec4 CalcX6FromDesignMat(
    double a, double b, double c, double d, double e) {
  // This should match the matrix in step 6 above, equation (9) in [1].
  // The 6th world point is the nullspace of this matrix.
  Mat X6null(6,4); 
  X6null << e-d,  0 ,  0 , a-b,
            e-c,  0 ,  a ,  0 ,
            d-c,  b ,  0 ,  0 ,
             0 ,  e ,  0 , a-c,
             0 , e-b, a-d,  0 ,
             0 ,  0 ,  d , b-c;
  Vec4 X6;
  Nullspace(&X6null, &X6);
  return X6;
}

// See paragraph after equation 16 in torr97robust for the equation used to
// derive the following coefficients.
#define ACCUMULATE_CUBIC_COEFFICIENTS(x,y,z, sgn) \
  p = t1[x]*t1[y]; \
  q = t2[x]*t1[y] + t1[x]*t2[y]; \
  d += sgn *  p*t1[z]; \
  c += sgn * (q*t1[z] + p*t2[z]); \
  b += sgn * (t2[x]*t2[y]*t1[z] + q*t2[z]); \
  a += sgn *  t2[x]*t2[y]*t2[z];

// TODO(keir): Break this up into smaller functions.
// TODO(keir): Change 'points' from 2 x 6nviews to be 2n views x 6; this way it
// can be directly passed from the robust estimation code without copying.
void SixPointNView(const Mat2X &points,
                   std::vector<SixPointReconstruction> *reconstructions) {
  CHECK(points.cols() % 6 == 0);
  int nviews = points.cols() / 6;

  // Convert to homogenous coordinates.
  Mat3X hpoints(3, points.cols());
  hpoints.block(0, 0, 2, 6*nviews) = points;
  hpoints.row(2).setOnes();

  // See equation (7.2) p179 of HZ; this is the DLT for solving cameras.
  // Chose wi = 1, i.e. the homogenous component of each image location is 1.
  // Note that As and Bs are on the heap to avoid blowing the stack for a large
  // number of views.
  Mat34 *As = new Mat34[nviews];
  Mat34 *Bs = new Mat34[nviews];
  Mat ws(nviews,5);

  for (int i = 0; i < nviews; ++i) {
    // Extract pencil of camera matricies.
    FivePointCameraPencil(hpoints.block(0, 6*i, 3, 5), As+i, Bs+i);

    // Calculate Q.
    Vec3 x6 = hpoints.col(6*i+5);
    Mat3 x6cross = CrossProductMatrix(x6);
    Mat4 Qa = As[i].transpose() * x6cross * Bs[i];
    Mat4 Q = Qa + Qa.transpose();

    // Read the coefficients w^i from Q and put into the ws matrix.
    ws(i,0) = Q(0,1);
    ws(i,1) = Q(0,2);
    ws(i,2) = Q(1,2);
    ws(i,3) = Q(1,3);
    ws(i,4) = Q(2,3);
  }
  Vec t1, t2;
  Nullspace2(&ws, &t1, &t2);

  // The svd gives us the basis for the nullspace of ws in which the t vector
  // lives, such that t = beta*t1+alpha*t2. However, there is a cubic
  // constraint on the elements of t, such that we can substitute and solve for
  // alpha. See equation (10) in [1].
  double a, b, c, d, p, q;
  a = b = c = d = 0;
  ACCUMULATE_CUBIC_COEFFICIENTS(0,1,3,  1);
  ACCUMULATE_CUBIC_COEFFICIENTS(0,1,4, -1);
  ACCUMULATE_CUBIC_COEFFICIENTS(0,2,4,  1);
  ACCUMULATE_CUBIC_COEFFICIENTS(0,3,4, -1);
  ACCUMULATE_CUBIC_COEFFICIENTS(1,2,3, -1);
  ACCUMULATE_CUBIC_COEFFICIENTS(1,3,4,  1);

  // TODO(keir): Handle case a = 0.
  // TODO(keir): Handle the case (a=b=c=d=0. If a=b=c=0 and d!=0, then alpha=0;
  // in that case, find beta instead.
  CHECK(a != 0.0);
  // Assume beta = 1.
  double a1 = b/a, b1 = c/a, c1 = d/a;
  a = a1;
  b = b1;
  c = c1;

  double alpha, alphas[3];
  int nroots = SolveCubicPolynomial(a, b, c, alphas+0, alphas+1, alphas+2);

  // Check each solution for alpha.
  reconstructions->resize(nroots);
  for (int ia=0; ia<nroots; ia++) {
    alpha = alphas[ia];

    double e;
    a = t1[0] + alpha*t2[0];
    b = t1[1] + alpha*t2[1];
    c = t1[2] + alpha*t2[2];
    d = t1[3] + alpha*t2[3];
    e = t1[4] + alpha*t2[4];

    SixPointReconstruction &pr = (*reconstructions)[ia];

    // The world position of the first five points, X1 through X5, are the
    // standard P^3 basis.
    pr.X.block<4, 4>(0, 0).setIdentity();
    pr.X.col(4).setOnes();

    // Find X6 from the chi vector.
    Vec4 Xp = CalcX6FromDesignMat(a, b, c, d, e);
    pr.X.col(5) = Xp;

    // Find P for each camera by finding suitable values of u,v.
    pr.P.resize(nviews);
    for (int i = 0; i < nviews; ++i) {
      // Project X6 with A and B. DO NOT NORMALIZE, it breaks the next step.
      Vec3 AX = As[i] * Xp;
      Vec3 BX = Bs[i] * Xp;

      // Find mu and nu with smallest algebraic error; see step 7. For exactly
      // six points, M will be rank 2. With more than 6 points, measurement
      // error will make M nonsingular.
      Mat3 M;
      M.col(0) = AX;
      M.col(1) = BX;
      M.col(2) = hpoints.col(6*i+5);  // x6.

      Vec3 munu;
      Nullspace(&M, &munu);
      double mu = munu[0];
      double nu = munu[1];

      pr.P[i] = mu*As[i] + nu*Bs[i];
    }
  }
  delete [] As;
  delete [] Bs;
}

} // namespace libmv
