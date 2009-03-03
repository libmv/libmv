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

#include "third_party/glog/src/glog/logging.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

//#include "libmv/multiview/generated.cc"

namespace {
using namespace libmv;

TEST(Fundamental, FundamentalFromProjections) {
  // TODO(pau) Write test for FundamentalFromProjections

}

TEST(Fundamental, PreconditionerFromPoints) {
  int n = 4;
  Mat points(2,n);
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
  Mat x1(2,n);
  x1 << 0, 0, 0, 1, 1, 1, 2, 2,
        0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2(2,n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1,i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondencesLinear(x1, x2, &F);

  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << x1(0, i), x1(1, i), 1;
    y << x2(0, i), x2(1, i), 1;
    F_x = F * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences8Point) {
  int n = 8;
  Mat x1(2,n);
  x1 << 0, 0, 0, 1, 1, 1, 2, 2,
        0, 1, 2, 0, 1, 2, 0, 1;

  Mat x2(2,n);
  x2 = x1;
  for (int i = 0; i < n; ++i) {
    x2(1,i) += 1;
  }

  Mat3 F;
  FundamentalFromCorrespondences8Point(x1, x2, &F);

  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << x1(0, i), x1(1, i), 1;
    y << x2(0, i), x2(1, i), 1;
    F_x = F * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_NEAR(0, y_F_x(0), 1e-8);
  EXPECT_NEAR(0, y_F_x(1), 1e-8);
  EXPECT_NEAR(0, y_F_x(2), 1e-8);
  EXPECT_NEAR(0, y_F_x(3), 1e-8);
  EXPECT_NEAR(0, y_F_x(4), 1e-8);
  EXPECT_NEAR(0, y_F_x(5), 1e-8);
  EXPECT_NEAR(0, y_F_x(6), 1e-8);
  EXPECT_NEAR(0, y_F_x(7), 1e-8);

  EXPECT_NEAR(0, F.determinant(), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondencesLinearRealistic) {
  TwoViewDataSet d = TwoRealisticCameras();

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondencesLinear(d.x1, d.x2, &F_estimated);

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);
  
  // Compare with ground truth.
  EXPECT_NEAR(F_gt_norm(0, 0), F_estimated_norm(0, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 1), F_estimated_norm(0, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 2), F_estimated_norm(0, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 0), F_estimated_norm(1, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 1), F_estimated_norm(1, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 2), F_estimated_norm(1, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 0), F_estimated_norm(2, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 1), F_estimated_norm(2, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 2), F_estimated_norm(2, 2), 1e-8);

  // Check fundamental properties.
  int n = d.X.cols();
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << d.x1(0, i), d.x1(1, i), 1;
    y << d.x2(0, i), d.x2(1, i), 1;
    F_x = F_estimated * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_MATRIX_NEAR_ZERO(y_F_x, 1e-8);
  EXPECT_NEAR(0, F_estimated.determinant(), 1e-8);
}

TEST(Fundamental, FundamentalFromCorrespondences8PointRealistic) {
  TwoViewDataSet d = TwoRealisticCameras();

  // Compute fundamental matrix from correspondences.
  Mat3 F_estimated;
  FundamentalFromCorrespondences8Point(d.x1, d.x2, &F_estimated);

  // Normalize.
  Mat3 F_gt_norm, F_estimated_norm;
  NormalizeFundamental(d.F, &F_gt_norm);
  NormalizeFundamental(F_estimated, &F_estimated_norm);
  
  // Compare with ground truth.
  EXPECT_NEAR(F_gt_norm(0, 0), F_estimated_norm(0, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 1), F_estimated_norm(0, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(0, 2), F_estimated_norm(0, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 0), F_estimated_norm(1, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 1), F_estimated_norm(1, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(1, 2), F_estimated_norm(1, 2), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 0), F_estimated_norm(2, 0), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 1), F_estimated_norm(2, 1), 1e-8);
  EXPECT_NEAR(F_gt_norm(2, 2), F_estimated_norm(2, 2), 1e-8);

  // Check fundamental properties.
  int n = d.X.cols();
  Vec y_F_x(n);
  for (int i = 0; i < n; ++i) {
    Vec3 x, y, F_x;
    x << d.x1(0, i), d.x1(1, i), 1;
    y << d.x2(0, i), d.x2(1, i), 1;
    F_x = F_estimated * x;
    y_F_x(i) = y.dot(F_x);
  }
  EXPECT_MATRIX_NEAR_ZERO(y_F_x, 1e-8);
  EXPECT_NEAR(0, F_estimated.determinant(), 1e-8);
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

  double dist0 = SampsonDistance2(F, x0, y0);
  double dist1 = SampsonDistance2(F, x1, y1);
  double dist2 = SampsonDistance2(F, x2, y2);
  double dist3 = SampsonDistance2(F, x3, y3);
  double dist4 = SampsonDistance2(F, x4, y4);
  double dist5 = SampsonDistance2(F, x5, y5);

  VLOG(1) << "SampsonDistance2: "
          << dist0 << " " 
          << dist1 << " " 
          << dist2 << " " 
          << dist3 << " " 
          << dist4 << " " 
          << dist5 << "\n";

  EXPECT_EQ(0, dist0);
  EXPECT_EQ(0, dist1);
  EXPECT_EQ(2 * Square(0.1), dist2);
  EXPECT_EQ(2 * Square(1), dist3);
  EXPECT_EQ(2 * Square(10), dist4);
  EXPECT_EQ(2 * Square(10), dist5);
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

  double dist0 = SymmetricEpipolarDistance2(F, x0, y0);
  double dist1 = SymmetricEpipolarDistance2(F, x1, y1);
  double dist2 = SymmetricEpipolarDistance2(F, x2, y2);
  double dist3 = SymmetricEpipolarDistance2(F, x3, y3);
  double dist4 = SymmetricEpipolarDistance2(F, x4, y4);
  double dist5 = SymmetricEpipolarDistance2(F, x5, y5);

  VLOG(1) << "SymmetricEpiporalDistance2: "
      << dist0 << " "
      << dist1 << " "
      << dist2 << " "
      << dist3 << " "
      << dist4 << " "
      << dist5 << "\n";

  EXPECT_EQ(0, dist0);
  EXPECT_EQ(0, dist1);
  EXPECT_EQ(2 * Square(0.1), dist2);
  EXPECT_EQ(2 * Square(1), dist3);
  EXPECT_EQ(2 * Square(10), dist4);
  EXPECT_EQ(2 * Square(10), dist5);
}

TEST(Fundamental, EssentialFromFundamental) {
  TwoViewDataSet d = TwoRealisticCameras();
  
  Mat3 E_from_Rt;
  EssentialFromRt(d.R1, d.t1, d.R2, d.t2, &E_from_Rt);

  Mat3 E_from_F;
  EssentialFromFundamental(d.F, d.K1, d.K2, &E_from_F);

  Mat3 E_from_Rt_norm, E_from_F_norm;
  NormalizeFundamental(E_from_Rt, &E_from_Rt_norm);
  NormalizeFundamental(E_from_F, &E_from_F_norm);

  EXPECT_NEAR(0, FrobeniusDistance(E_from_Rt_norm, E_from_F_norm), 1e-8);
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
 
} // namespace
