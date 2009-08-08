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

#include <iostream>

#include "libmv/logging/logging.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

// Check the properties of a fundamental matrix:
//
//   1. The determinant is 0 (rank deficient)
//   2. The condition x'T*F*x = 0 is satisfied to precision.
//
template<typename TMat>
void ExpectFundamentalProperties(const TMat &F,
                                 const Mat &ptsA,
                                 const Mat &ptsB,
                                 double precision) {
  EXPECT_NEAR(0, F.determinant(), precision);
  assert(ptsA.cols() == ptsB.cols());
  Mat hptsA, hptsB;
  EuclideanToHomogeneous(ptsA, &hptsA);
  EuclideanToHomogeneous(ptsB, &hptsB);
  for (int i = 0; i < ptsA.cols(); ++i) {
    double residual = hptsB.col(i).dot(F * hptsA.col(i));
    EXPECT_NEAR(0.0, residual, precision);
  }
}

TEST(Fundamental, FundamentalFromProjections) {
  Mat34 P1_gt, P2_gt;
  P1_gt << 1, 0, 0, 0,
           0, 1, 0, 0,
           0, 0, 1, 0;
  P2_gt << 1, 1, 1, 3,
           0, 2, 0, 3,
           0, 1, 1, 0;
  Mat3 F_gt;
  FundamentalFromProjections(P1_gt, P2_gt, &F_gt);

  Mat34 P1, P2;
  ProjectionsFromFundamental(F_gt, &P1, &P2);

  Mat3 F;
  FundamentalFromProjections(P1, P2, &F);

  EXPECT_MATRIX_PROP(F_gt, F, 1e-6);
}

TEST(Fundamental, PreconditionerFromPoints) {
  int n = 4;
  Mat points(2, n);
  points << 0, 0, 1, 1,
            0, 2, 1, 3;

  Mat3 T;
  PreconditionerFromPoints(points, &T);

  Mat normalized_points;
  ApplyTransformationToPoints(points, T, &normalized_points);

  Vec mean, variance;
  MeanAndVarianceAlongRows(normalized_points, &mean, &variance);

  EXPECT_NEAR(0, mean(0), 1e-8);
  EXPECT_NEAR(0, mean(1), 1e-8);
  EXPECT_NEAR(2, variance(0), 1e-8);
  EXPECT_NEAR(2, variance(1), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondencesLinear) {
  int n = 8;
  Mat x1(2, n);
  x1 << 0, 0, 0, 1, 1, 1, 2, 2,
        0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1, i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondencesLinear(x1, x2, &F);

  ExpectFundamentalProperties(F, x1, x2, 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences8Point) {
  int n = 8;
  Mat x1(2, n);
  x1 << 0, 0, 0, 1, 1, 1, 2, 2,
        0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2(2, n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1, i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondences8Point(x1, x2, &F);

  ExpectFundamentalProperties(F, x1, x2, 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondencesLinearRealistic) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 F_estimated;
  FundamentalFromCorrespondencesLinear(d.x1, d.x2, &F_estimated);

  EXPECT_MATRIX_PROP(d.F, F_estimated, 1e-6);
}

TEST(Fundamental, FundamentalFromCorrespondences8PointRealistic) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 F_estimated;
  FundamentalFromCorrespondences8Point(d.x1, d.x2, &F_estimated);

  EXPECT_MATRIX_PROP(d.F, F_estimated, 1e-6);
}

// 8 points in a cube is a degenerate configuration.
// FundamentalFromCorrespondencesX return a small number when linear estimation
// of F fails.
TEST(Fundamental, FundamentalFromCorrespondences8PointDegenerate) {
  TwoViewDataSet d = TwoRealisticCameras();

  // The 8 points of a cube and their projections.
  d.X.resize(3,8);
  d.X << 0, 1, 0, 1, 0, 1, 0, 1,
         0, 0, 1, 1, 0, 0, 1, 1,
         0, 0, 0, 0, 1, 1, 1, 1;
  Project(d.P1, d.X, &d.x1);
  Project(d.P2, d.X, &d.x2);

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  double res;
  res = FundamentalFromCorrespondences8Point(d.x1, d.x2, &F_estimated);

  EXPECT_NEAR(0, res, 1e-8);
}


TEST(Fundamental, SampsonDistance2) {
  Vec3 t(1, 0, 0);
  Mat3 F = CrossProductMatrix(t); // Fundametal matrix corresponding to pure
                                  // translation.

  Vec2 x0(0, 0), y0(  0,   0); // Good match (at infinity).
  Vec2 x1(0, 0), y1(100,   0); // Good match (no vertical disparity).
  Vec2 x2(0, 0), y2(0.0, 0.1); // Small error (a bit of vertical disparity).
  Vec2 x3(0, 0), y3(  0,   1); // Bigger error.
  Vec2 x4(0, 0), y4(  0,  10); // Biggest error.
  Vec2 x5(0, 0), y5(100,  10); // Biggest error with horitzontal disparity.

  Vec6 dists;
  dists << SampsonDistance2(F, x0, y0),
           SampsonDistance2(F, x1, y1),
           SampsonDistance2(F, x2, y2),
           SampsonDistance2(F, x3, y3),
           SampsonDistance2(F, x4, y4),
           SampsonDistance2(F, x5, y5);

  VLOG(1) << "SampsonDistance2: " << dists.transpose();

  // The expected distance are two times (one per image) the distance from the
  // point to the reprojection of the best triangulated point.  For this
  // particular example this reprojection is the midpoint between the point and
  // the epipolar line.
  EXPECT_EQ(0, dists[0]);
  EXPECT_EQ(0, dists[1]);
  EXPECT_EQ(2 * Square(0.1 / 2), dists[2]);
  EXPECT_EQ(2 * Square(1.0 / 2), dists[3]);
  EXPECT_EQ(2 * Square(10. / 2), dists[4]);
  EXPECT_EQ(2 * Square(10. / 2), dists[5]);
}

TEST(Fundamental, SymmetricEpipolarDistance2) {
  Vec3 t(1, 0, 0);
  Mat3 F = CrossProductMatrix(t); // Fundametal matrix corresponding to pure
                                  // translation.

  Vec2 x0(0, 0), y0(  0,   0); // Good match (at infinity).
  Vec2 x1(0, 0), y1(100,   0); // Good match (no vertical disparity).
  Vec2 x2(0, 0), y2(0.0, 0.1); // Small error (a bit of vertical disparity).
  Vec2 x3(0, 0), y3(  0,   1); // Bigger error.
  Vec2 x4(0, 0), y4(  0,  10); // Biggest error.
  Vec2 x5(0, 0), y5(100,  10); // Biggest error with horitzontal disparity.

  Vec6 dists;
  dists << SymmetricEpipolarDistance2(F, x0, y0), 
           SymmetricEpipolarDistance2(F, x1, y1),
           SymmetricEpipolarDistance2(F, x2, y2),
           SymmetricEpipolarDistance2(F, x3, y3),
           SymmetricEpipolarDistance2(F, x4, y4),
           SymmetricEpipolarDistance2(F, x5, y5);

  VLOG(1) << "SymmetricEpiporalDistance2: " << dists.transpose();

  // The expected distances are two times (one per image) the distance from the
  // point to the epipolar line.
  EXPECT_EQ(0, dists[0]);
  EXPECT_EQ(0, dists[1]);
  EXPECT_EQ(2 * Square(0.1), dists[2]);
  EXPECT_EQ(2 * Square(1.0), dists[3]);
  EXPECT_EQ(2 * Square(10),  dists[4]);
  EXPECT_EQ(2 * Square(10),  dists[5]);
}

TEST(Fundamental, EssentialFromFundamental) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 E_from_Rt;
  EssentialFromRt(d.R1, d.t1, d.R2, d.t2, &E_from_Rt);

  Mat3 E_from_F;
  EssentialFromFundamental(d.F, d.K1, d.K2, &E_from_F);

  EXPECT_MATRIX_PROP(E_from_Rt, E_from_F, 1e-6);
}

TEST(Fundamental, MotionFromEssential) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 E;
  EssentialFromRt(d.R1, d.t1, d.R2, d.t2, &E);

  Mat3 R;
  Vec3 t;
  RelativeCameraMotion(d.R1, d.t1, d.R2, d.t2, &R, &t);
  NormalizeL2(&t);

  std::vector<Mat3> Rs;
  std::vector<Vec3> ts;
  MotionFromEssential(E, &Rs, &ts);
  bool one_solution_is_correct = false;
  for (size_t i = 0; i < Rs.size(); ++i) {
    if(FrobeniusDistance(Rs[i], R) < 1e-8 && DistanceL2(ts[i], t) < 1e-8) {
      one_solution_is_correct = true;
      break;
    }
  }
  EXPECT_TRUE(one_solution_is_correct);
}

TEST(Fundamental, MotionFromEssentialChooseSolution) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 E;
  EssentialFromRt(d.R1, d.t1, d.R2, d.t2, &E);

  Mat3 R;
  Vec3 t;
  RelativeCameraMotion(d.R1, d.t1, d.R2, d.t2, &R, &t);
  NormalizeL2(&t);

  std::vector<Mat3> Rs;
  std::vector<Vec3> ts;
  MotionFromEssential(E, &Rs, &ts);

  Vec2 x1, x2;
  MatrixColumn(d.x1, 0, &x1);
  MatrixColumn(d.x2, 0, &x2);
  int solution = MotionFromEssentialChooseSolution(Rs, ts, d.K1, x1, d.K2, x2);

  EXPECT_LE(0, solution);
  EXPECT_LE(solution, 3);
  EXPECT_LE(FrobeniusDistance(Rs[solution], R), 1e-8);
  EXPECT_LE(DistanceL2(ts[solution], t), 1e-8);
}

TEST(Fundamental, MotionFromEssentialAndCorrespondence) {
  TwoViewDataSet d = TwoRealisticCameras();

  Mat3 E;
  EssentialFromRt(d.R1, d.t1, d.R2, d.t2, &E);

  Mat3 R;
  Vec3 t;
  RelativeCameraMotion(d.R1, d.t1, d.R2, d.t2, &R, &t);
  NormalizeL2(&t);

  Vec2 x1, x2;
  MatrixColumn(d.x1, 0, &x1);
  MatrixColumn(d.x2, 0, &x2);

  Mat3 R_estimated;
  Vec3 t_estimated;
  MotionFromEssentialAndCorrespondence(E, d.K1, x1, d.K2, x2,
                                       &R_estimated, &t_estimated);

  EXPECT_LE(FrobeniusDistance(R_estimated, R), 1e-8);
  EXPECT_LE(DistanceL2(t_estimated, t), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences7Point) {
  const int n = 7;
  Mat x1(2, n);
  x1 << 0, 0, 0, 1, 1, 1, 2,
        0, 1, 2, 0, 1, 2, 0;

  Mat x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1, i) += 1;
  }

  std::vector<Mat3> Fvec;
  FundamentalFromCorrespondences7Point(x1, x2, &Fvec);

  for(int i = 0; i < Fvec.size(); ++i) {
    //const Mat3 &F = Fvec[i];
    ExpectFundamentalProperties(Fvec[i], x1, x2, 1e-8);
    //ExpectFundamentalProperties(F, x1, x2, 1e-8);
  }
}

TEST(Fundamental, Solver7PointRealCorrespondences) {
  const int n = 7;
  Mat x1(2, n);
  Mat x2(2, n);

  x1 <<  723, 1091, 1691, 447,  971, 1903, 1483,
         887,  699,  811, 635,   91,  447, 1555;
  x2 << 1251, 1603, 2067, 787, 1355, 2163, 1875,
        1243,  923, 1031, 484,  363,  743, 1715;

  std::vector<Mat3> Fvec;
  FundamentalFromCorrespondences7Point(x1, x2, &Fvec);

  for (int k = 0; k < Fvec.size(); ++k) {
    ExpectFundamentalProperties(Fvec[k], x1, x2, 1e-8);
  }
}

TEST(Fundamental, Solver7PointWithPointsOnTheCube) {
  TwoViewDataSet d = TwoRealisticCameras();

  // Try the 7 points of a cube and their projections, missing the last corner.
  const int n = 7;
  d.X.resize(3, n);
  d.X <<  0, 1, 0, 1, 0, 1, 0, // X,
          0, 0, 1, 1, 0, 0, 1, // Y,
          0, 0, 0, 0, 1, 1, 1; // Z.
  Project(d.P1, d.X, &d.x1);
  Project(d.P2, d.X, &d.x2);

  std::vector<Mat3> F_estimated;
  FundamentalFromCorrespondences7Point(d.x1, d.x2, &F_estimated);

  for(int k = 0; k < F_estimated.size(); ++k)  {
    ExpectFundamentalProperties(F_estimated[k], d.x1, d.x2, 1e-8);
  }
}

} // namespace
