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

#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

// Two cameras looking with orthogonal (non-crossing) viewing rays.
// One at the origin and looking to the z direction.
// The other at (1,1,1) and looking down the y direction.
void OrthogonalViewsFundamental(Mat3 *F) {
  Mat3 K1, R1, K2, R2;
  Vec3 t1, t2;
  Mat34 P1, P2;
  K1 << 1, 0, 0,
        0, 1, 0,
        0, 0, 1;
  K2 = K1;
  R1 << 1, 0, 0,
        0, 1, 0,
        0, 0, 1;
  R2 << 1, 0,  0,
        0, 0, -1,
        0, 1,  0;
  t1 << 0, 0, 0;
  t2 << -1, 1, 1;

  P_From_KRt(K1, R1, t1, &P1);
  P_From_KRt(K2, R2, t2, &P2);
  FundamentalFromProjections(P1, P2, F);
}

TEST(FocalFromFundamental, EpipolesFromFundamental) {
  Mat3 F;
  OrthogonalViewsFundamental(&F);

  Vec3 e1, e2;
  EpipolesFromFundamental(F, &e1, &e2);
  e1 /= e1(2);
  e2 /= e2(2);
  EXPECT_NEAR( 1, e1(0), 1e-8);
  EXPECT_NEAR(-1, e1(1), 1e-8);
  EXPECT_NEAR( 1, e1(2), 1e-8);
  EXPECT_NEAR(-1, e2(0), 1e-8);
  EXPECT_NEAR( 1, e2(1), 1e-8);
  EXPECT_NEAR( 1, e2(2), 1e-8);
}

TEST(FocalFromFundamental, RotationToEliminateY) {
  Vec3 a, b;
  a << 1, -1, 0;
  Mat3 T;
  RotationToEliminateY(a, &T);
  b = T * a;
  EXPECT_NEAR(sqrt(2), b(0), 1e-8);
  EXPECT_NEAR(      0, b(1), 1e-8);
  EXPECT_NEAR(      0, b(2), 1e-8);
}

TEST(FocalFromFundamental, FundamentalAlignEpipolesToXAxis) {
  Mat3 F, F_rotated;
  OrthogonalViewsFundamental(&F);
  FundamentalAlignEpipolesToXAxis(F,&F_rotated);
  Vec3 e1, e2;
  EpipolesFromFundamental(F_rotated, &e1, &e2);
  e1 /= e1(2);
  e2 /= e2(2);
  EXPECT_NEAR(sqrt(2), fabs(e1(0)), 1e-8); // The sign of x is undetermined.
                                           // It depends on the sign of z
                                           // before the alignement.
  EXPECT_NEAR(      0,       e1(1), 1e-8); // y coordinate is now 0.
  EXPECT_NEAR(      1,       e1(2), 1e-8);
  EXPECT_NEAR(sqrt(2.0), fabs(e2(0)), 1e-8);
  EXPECT_NEAR(      0,       e2(1), 1e-8); // y coordinate is now 0.
  EXPECT_NEAR(      1,       e2(2), 1e-8);
}

TEST(FocalFromFundamental, FundamentalShiftPrincipalPoints) {
  // Create two pair of cameras with the same parameters except for the
  // principal point.
  Vec2 p1, p1_new, p2, p2_new;
  Mat3 K1, K1_new, R1, K2, K2_new, R2;
  Vec3 t1, t2;
  Mat34 P1, P1_new, P2, P2_new;
  p1 << 3, 4;
  p1_new << 2, 1;
  p2 << -2, 1;
  p2_new << 4, -3;
  K1 << 1, 0, p1(0),
        0, 1, p1(1),
        0, 0,     1;
  K1_new << 1, 0, p1_new(0),
            0, 1, p1_new(1),
            0, 0,         1;
  K2 << 1, 0, p2(0),
        0, 1, p2(1),
        0, 0,     1;
  K2_new << 1, 0, p2_new(0),
            0, 1, p2_new(1),
            0, 0,         1;
  R1 << 1, 0, 0,
        0, 1, 0,
        0, 0, 1;
  R2 << 1, 0, 0,
        0, 1, 0,
        0, 0, 1;
  t1 << 0, 0, 0;
  t2 << 1, 0, 0;

  P_From_KRt(K1, R1, t1, &P1);
  P_From_KRt(K2, R2, t2, &P2);
  P_From_KRt(K1_new, R1, t1, &P1_new);
  P_From_KRt(K2_new, R2, t2, &P2_new);

  // Compute the fundamental matrix of each camera pair.
  Mat3 F, F_new;
  FundamentalFromProjections(P1, P2, &F);
  FundamentalFromProjections(P1_new, P2_new, &F_new);

  // Shift the principal point of the first fundamental matrix to match the
  // second one.
  Mat3 F_new_computed;
  FundamentalShiftPrincipalPoints(F, p1, p1_new, p2, p2_new, &F_new_computed);

  // TODO(pau) The matrices should be scaled before comparing since they have
  // to be equal only up to scale.
  EXPECT_NEAR(F_new(0,0), F_new_computed(0,0), 1e-8);
  EXPECT_NEAR(F_new(0,1), F_new_computed(0,1), 1e-8);
  EXPECT_NEAR(F_new(0,2), F_new_computed(0,2), 1e-8);
  EXPECT_NEAR(F_new(1,0), F_new_computed(1,0), 1e-8);
  EXPECT_NEAR(F_new(1,1), F_new_computed(1,1), 1e-8);
  EXPECT_NEAR(F_new(1,2), F_new_computed(1,2), 1e-8);
  EXPECT_NEAR(F_new(2,0), F_new_computed(2,0), 1e-8);
  EXPECT_NEAR(F_new(2,1), F_new_computed(2,1), 1e-8);
  EXPECT_NEAR(F_new(2,2), F_new_computed(2,2), 1e-8);
}

TEST(FocalFromFundamental, FocalFromFundamental) {
  Mat3 F;
  Vec2 zero2;
  zero2 << 0, 0;
  double f1, f2;
  OrthogonalViewsFundamental(&F);
  FocalFromFundamental(F, zero2, zero2, &f1, &f2);
  EXPECT_NEAR(1, f1, 1e-8);
  EXPECT_NEAR(1, f2, 1e-8);
}

TEST(FocalFromFundamental, TwoViewReconstruction) {
  // Two cameras at (0,0,-10) and (2,1,-10) looking towards z+.
  TwoViewDataSet d = TwoRealisticCameras();
  Vec2 p1, p2;
  p1 << d.K1(0,2), d.K1(1,2);
  p2 << d.K2(0,2), d.K2(1,2);
  double f1, f2;
  f1 = d.K1(0,0);
  f2 = d.K2(0,0);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondences8Point(d.x1, d.x2, &F_estimated);

  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);

  EXPECT_NEAR(0, FrobeniusDistance(F_gt_norm, F_estimated_norm), 1e-8);

  // Compute focal lenght.
  double f1_estimated, f2_estimated;
  FocalFromFundamental(F_estimated, p1, p2, &f1_estimated, &f2_estimated);

  // Build K matrices from the known principal points and the computed focals.
  Mat3 K1_estimated, K2_estimated;
  K1_estimated << f1_estimated,            0, p1(0),
                             0, f1_estimated, p1(1),
                             0,            0,     1;
  K2_estimated << f2_estimated,            0, p2(0),
                             0, f2_estimated, p2(1),
                             0,            0,     1;
  EXPECT_NEAR(0, FrobeniusDistance(d.K1, K1_estimated), 1e-8);
  EXPECT_NEAR(0, FrobeniusDistance(d.K2, K2_estimated), 1e-8);

  // Compute essential matrix
  Mat3 E_estimated;
  EssentialFromFundamental(F_estimated, K1_estimated, K2_estimated,
                           &E_estimated);

  // Recover R, t from E and K
  Vec2 x1, x2;
  MatrixColumn(d.x1, 0, &x1);
  MatrixColumn(d.x2, 0, &x2);
  Mat3 R_estimated, R;
  Vec3 t_estimated, t;
  MotionFromEssentialAndCorrespondence(E_estimated,
                                       K1_estimated, x1,
                                       K2_estimated, x2,
                                       &R_estimated, &t_estimated);

  RelativeCameraMotion(d.R1, d.t1, d.R2, d.t2, &R, &t);
  NormalizeL2(&t);

  EXPECT_LE(FrobeniusDistance(R, R_estimated), 1e-8);
  EXPECT_LE(DistanceL2(t, t_estimated), 1e-8);
}

TEST(FocalFromFundamentalExhaustive, TwoViewReconstruction) {
  // Two cameras at (0,0,-10) and (2,1,-10) looking towards z+.
  TwoViewDataSet d = TwoRealisticCameras(true);
  Vec2 pp;
  pp << d.K1(0,2), d.K1(1,2);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondences8Point(d.x1, d.x2, &F_estimated);

  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);

  EXPECT_NEAR(0, FrobeniusDistance(F_gt_norm, F_estimated_norm), 1e-8);

  // Compute focal lenght.
  double f_estimated;
  FocalFromFundamentalExhaustive(F_estimated, pp, d.x1, d.x2,
                                 10, 1000, 100, &f_estimated);

  // Build K from the known principal point and the computed focal.
  Mat3 K_estimated;
  K_estimated << f_estimated,           0, pp(0),
                           0, f_estimated, pp(1),
                           0,           0,     1;
  EXPECT_NEAR(0, FrobeniusDistance(d.K1, K_estimated), 1e-8);
  EXPECT_NEAR(0, FrobeniusDistance(d.K2, K_estimated), 1e-8);

  // Compute essential matrix
  Mat3 E_estimated;
  EssentialFromFundamental(F_estimated, K_estimated, K_estimated,
                           &E_estimated);

  // Recover R, t from E and K
  Vec2 x1, x2;
  MatrixColumn(d.x1, 0, &x1);
  MatrixColumn(d.x2, 0, &x2);
  Mat3 R_estimated, R;
  Vec3 t_estimated, t;
  MotionFromEssentialAndCorrespondence(E_estimated,
                                       K_estimated, x1,
                                       K_estimated, x2,
                                       &R_estimated, &t_estimated);

  RelativeCameraMotion(d.R1, d.t1, d.R2, d.t2, &R, &t);
  NormalizeL2(&t);

  EXPECT_LE(FrobeniusDistance(R, R_estimated), 1e-8);
  EXPECT_LE(DistanceL2(t, t_estimated), 1e-8);
}

}
