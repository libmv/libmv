// Copyright (c) 2011 libmv authors.
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

#include "testing/testing.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/euclidean.h"
namespace {
using namespace libmv;

TEST(Euclidean2DTest, TranslationX) {
  Vec2 trans_gt;
  trans_gt << 1, 0;
  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         0, 1, 1;

  Mat3 euc_mat;
  EXPECT_TRUE(Euclidean2DFromCorrespondencesLinear(x1, x2, &euc_mat));
  VLOG(1) << "Mat Euclidean2D "<< std::endl <<euc_mat;
  Mat3 ground_truth;
  ground_truth << 1,0,1,
                  0,1,0,
                  0,0,1;
  EXPECT_MATRIX_NEAR(euc_mat, ground_truth,1e-8);
  double angle;
  Vec2 trans;
  ExtractEuclidean2DCoefficients(euc_mat, &trans, &angle);
  EXPECT_NEAR(angle, 0, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Euclidean2DTest, TranslationXY) {
  Vec2 trans_gt;
  trans_gt << 1, 1;
  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         1, 2, 2;

  Mat3 euc_mat;
  EXPECT_TRUE(Euclidean2DFromCorrespondencesLinear(x1, x2, &euc_mat));
  VLOG(1) << "Mat Euclidean2D "<< std::endl << euc_mat;
  Mat3 ground_truth;
  ground_truth << 1,0,1,
                 0,1,1,
                 0,0,1;
  EXPECT_MATRIX_NEAR(euc_mat, ground_truth,1e-8);
  double angle;
  Vec2 trans;
  ExtractEuclidean2DCoefficients(euc_mat, &trans, &angle);
  EXPECT_NEAR(angle, 0, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Euclidean2DTest, Rotation45) {
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double angle_gt = M_PI / 4.0;
  Mat3 rot;
  rot <<  cos(angle_gt), -sin(angle_gt), 0,
          sin(angle_gt),  cos(angle_gt), 0,
          0,             0,              1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
  }

  Mat3 euc_mat;
  EXPECT_TRUE(Euclidean2DFromCorrespondencesLinear(x1, x2, &euc_mat));
  VLOG(1) << "Mat Euclidean2D "<< std::endl << euc_mat;
  EXPECT_MATRIX_NEAR(euc_mat, rot, 1e-8);
  double angle;
  Vec2 trans;
  Vec2 trans_gt;
  trans_gt << 0, 0;
  ExtractEuclidean2DCoefficients(euc_mat, &trans, &angle);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Euclidean2DTest, RotationM90) {
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double angle_gt = -M_PI / 2.0;
  Mat3 rot;
  rot <<  cos(angle_gt), -sin(angle_gt), 0,
          sin(angle_gt),  cos(angle_gt), 0,
          0,             0,              1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
  }

  Mat3 euc_mat;
  EXPECT_TRUE(Euclidean2DFromCorrespondencesLinear(x1, x2, &euc_mat));
  VLOG(1) << "Mat Euclidean2D "<< std::endl << euc_mat;
  EXPECT_MATRIX_NEAR(euc_mat, rot, 1e-8);
  double angle;
  Vec2 trans;
  Vec2 trans_gt;
  trans_gt << 0, 0;
  ExtractEuclidean2DCoefficients(euc_mat, &trans, &angle);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Euclidean2DTest, Rotation45AndTranslationXY) {
  Vec2 trans_gt;
  trans_gt << -2, 5;
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double angle_gt = M_PI / 4.0;
  Mat3 rot;
  rot <<  cos(angle_gt), -sin(angle_gt), trans_gt(0),
          sin(angle_gt),  cos(angle_gt), trans_gt(1),
          0,             0,              1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  Mat3 euc_mat;
  EXPECT_TRUE(Euclidean2DFromCorrespondencesLinear(x1, x2, &euc_mat));
  VLOG(1) << "Mat Euclidean2D "<< std::endl << euc_mat;
  EXPECT_MATRIX_NEAR(euc_mat, rot, 1e-8);
  
  double angle;
  Vec2 trans;
  ExtractEuclidean2DCoefficients(euc_mat, &trans, &angle);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

#if 1
//! Solve a slightly similarity test issue
TEST(Euclidean2DTest, AlmostEuclidean) {
  Vec2 trans_gt;
  trans_gt << -2, -6;
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double angle_gt = M_PI / 4.0;
  const double s = 0.995;
  Mat3 rot;
  rot <<  s*cos(angle_gt),-s*sin(angle_gt), trans_gt(0),
          s*sin(angle_gt), s*cos(angle_gt), trans_gt(1),
          0,               0,                         1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  const double kPrecision = 3e-2;
  Mat3 euc_mat;
  EXPECT_TRUE(Euclidean2DFromCorrespondencesLinear(x1, x2, &euc_mat, 1e-2));
  VLOG(1) << "Mat Euclidean2D "<< std::endl << euc_mat;
  EXPECT_MATRIX_NEAR(euc_mat, rot, kPrecision);
  
  double angle;
  Vec2 trans;
  ExtractEuclidean2DCoefficients(euc_mat, &trans, &angle);
  EXPECT_NEAR(angle, angle_gt, kPrecision);
  EXPECT_MATRIX_NEAR(trans, trans_gt, kPrecision);
}
#endif
}
