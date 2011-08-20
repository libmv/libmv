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
#include "libmv/multiview/projection.h"
#include "libmv/multiview/similarity.h"

namespace {
using namespace libmv;

TEST(Similarity2DTest, TranslationX) {
  Vec2 trans_gt;
  trans_gt << 1, 0;
  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         0, 1, 1;

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl <<sim_mat;
  Mat3 ground_truth;
  ground_truth << 1,0,1,
                  0,1,0,
                  0,0,1;
  EXPECT_MATRIX_NEAR(sim_mat, ground_truth,1e-8);
  double angle, scale;
  Vec2 trans;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, 0, 1e-8);
  EXPECT_NEAR(scale, 1, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, TranslationXY) {
  Vec2 trans_gt;
  trans_gt << 1, 1;
  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         1, 2, 2;

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  Mat3 ground_truth;
  ground_truth << 1,0,1,
                 0,1,1,
                 0,0,1;
  EXPECT_MATRIX_NEAR(sim_mat, ground_truth,1e-8);
  double angle, scale;
  Vec2 trans;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, 0, 1e-8);
  EXPECT_NEAR(scale, 1, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, Rotation45) {
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

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, rot, 1e-8);
  double angle, scale;
  Vec2 trans;
  Vec2 trans_gt;
  trans_gt << 0, 0;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_NEAR(scale, 1, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, RotationM90) {
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

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, rot, 1e-8);
  double angle, scale;
  Vec2 trans;
  Vec2 trans_gt;
  trans_gt << 0, 0;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_NEAR(scale, 1, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, Rotation45AndTranslationXY) {
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

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, rot, 1e-8);
  
  double angle, scale;
  Vec2 trans;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_NEAR(scale, 1, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, Scale3) {
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double scale_gt = 3;
  Mat3 m;
  m <<  scale_gt, 0, 0,
        0, scale_gt, 0,
        0, 0,        1;

  Mat x2 = x1;
  // Transform point from ground truth scale matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = m.block<2,2>(0,0) * x1.col(i);
  }

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, m, 1e-8);
  
  double angle, scale;
  Vec2 trans;
  Vec2 trans_gt;
  trans_gt << 0, 0;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, 0, 1e-8);
  EXPECT_NEAR(scale, scale_gt, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, Rotation90AndScale) {
  Vec2 trans_gt;
  trans_gt << 0, 0;
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double scale_gt = 3;
  const double angle_gt = M_PI / 2.0;
  Mat3 rot;
  rot <<  scale_gt*cos(angle_gt), -scale_gt*sin(angle_gt), trans_gt(0),
          scale_gt*sin(angle_gt),  scale_gt*cos(angle_gt), trans_gt(1),
          0,                      0,                       1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, rot, 1e-8);
  
  double angle, scale;
  Vec2 trans;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_NEAR(scale, scale_gt, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

TEST(Similarity2DTest, Rotation45AndTransXYAndScale) {
  Vec2 trans_gt;
  trans_gt << -2, -6;
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double scale_gt = 3;
  const double angle_gt = M_PI / 4.0;
  Mat3 rot;
  rot <<  scale_gt*cos(angle_gt), -scale_gt*sin(angle_gt), trans_gt(0),
          scale_gt*sin(angle_gt),  scale_gt*cos(angle_gt), trans_gt(1),
          0,                      0,                       1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, rot, 1e-8);
  
  double angle, scale;
  Vec2 trans;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, angle_gt, 1e-8);
  EXPECT_NEAR(scale, scale_gt, 1e-8);
  EXPECT_MATRIX_NEAR(trans, trans_gt, 1e-8);
}

#if 1
//! Solve a slightly affine test issue
TEST(Similarity2DTest, AlmostEuclidean) {
  Vec2 trans_gt;
  trans_gt << -2, -6;
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  const double scale_gt = 2;
  const double angle_gt = M_PI / 4.0;
  Mat3 rot;
  rot <<  scale_gt*cos(angle_gt)+0.02, -scale_gt*sin(angle_gt), trans_gt(0),
          scale_gt*sin(angle_gt)+0.01, scale_gt*cos(angle_gt), trans_gt(1),
          0,                      0,                       1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i);
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  const double kPrecision = 3e-2;
  Mat3 sim_mat;
  EXPECT_TRUE(Similarity2DFromCorrespondencesLinear(x1, x2, &sim_mat, 1e-2));
  VLOG(1) << "Mat Similarity2D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, rot, kPrecision);
  
  double angle, scale;
  Vec2 trans;
  ExtractSimilarity2DCoefficients(sim_mat, &trans, &angle, &scale);
  EXPECT_NEAR(angle, angle_gt, kPrecision);
  EXPECT_NEAR(scale, scale_gt, kPrecision);
  EXPECT_MATRIX_NEAR(trans, trans_gt, kPrecision);
}
#endif

TEST(Similarity3DTest, TranslationX) {
  Mat x1(3, 4);
  x1 <<  0, 1, 2, 3,
         0, 5, 1, 3,
         0, 1, 7, 3;

  Mat x2(3, 4);
  x2 <<  0, 1, 2, 3,
         0, 5, 1, 3,
         1, 2, 8, 4;

  Mat4 sim_mat;
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similitude3D "<< std::endl <<sim_mat;
  Mat4 ground_truth;
  ground_truth << 1,0,0,0,
                  0,1,0,0,
                  0,0,1,1,
                  0,0,0,1;
  EXPECT_MATRIX_NEAR(sim_mat, ground_truth,1e-8);
}

TEST(Similarity3DTest, TranslationXYZ) {
  Mat x1(3, 4);
  x1 <<  0, 1, 2, 3,
         0, 5, 1, 3,
         0, 1, 7, 3;

  Mat x2(3, 4);
  x2 <<  2, 3, 4, 5,
        -1, 4, 0, 2,
         1, 2, 8, 4;

  Mat4 sim_mat;
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similitude3D "<< std::endl << sim_mat;
  Mat4 ground_truth;
  ground_truth << 1,0,0, 2,
                  0,1,0,-1,
                  0,0,1, 1,
                  0,0,0, 1;
  EXPECT_MATRIX_NEAR(sim_mat, ground_truth,1e-8);
}

TEST(Similarity3DTest, Rotation90Z) {
  Mat x1(3, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3,
         0, 2, 0, 1;

  Mat4 M;
  /*
  M = AngleAxisd(45.0, Vector3f::UnitZ());*/
  // Rotation on x 
  double angle = M_PI / 2.0;
  M   <<  1,          0,           0, 0,
          0, cos(angle), -sin(angle), 0,
          0, sin(angle),  cos(angle), 0,
          0,          0,           0, 1;
  Mat x2 = x1;
  // Transform point from ground truth matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<3,1>(0,i) =  M.block<3,3>(0,0) * x1.col(i) ;
  }

  Mat4 sim_mat;
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similitude3D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, M, 1e-8);;
}

TEST(Similarity3DTest, Rotation45AndTranslationXY) {
  Mat x1(3, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3,
         0, 2, 0, 1;
  Mat4 M;
  M.setIdentity();
  /*
  M = AngleAxisd(45.0, Vector3f::UnitZ())
    * AngleAxisd(25.0, Vector3f::UnitX())
    * AngleAxisd(5.0, Vector3f::UnitZ());*/
      
  // Rotation on x + translation
  double angle = 45.0;
  Mat4 rot;
  rot <<  1,          0,           0, 1,
          0, cos(angle), -sin(angle), 3,
          0, sin(angle),  cos(angle),-2,
          0,          0,           0, 1;
  M *= rot;
  // Rotation on y
  angle = 25.0;
  rot <<  cos(angle), 0, sin(angle),  0,
          0,          1,          0,  0,
         -sin(angle), 0, cos(angle),  0,
          0,           0,          0, 1;
  M *= rot;
  // Rotation on z
  angle = 5.0;
  rot <<  cos(angle), -sin(angle), 0, 0,
          sin(angle),  cos(angle), 0, 0,
          0,           0,          1, 0,
          0,           0,          0, 1;
  M *= rot;
  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<3,1>(0,i) = M.block<3,3>(0,0) * x1.col(i);
    x2.block<3,1>(0,i) += M.block<3,1>(0,3); // translation
  }
  Mat4 sim_mat;
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similitude3D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, M, 1e-8);
}

TEST(Similarity3DTest, Scale3) {
  Mat x1(3, 4);
  x1 <<  0, 1, 1, 5,
         0, 5, 2, 3,
         0, 2, 0, 1;
  const double scale_gt = 3;
  Mat4 m;
  m <<  scale_gt, 0, 0, 0,
        0, scale_gt, 0, 0,
        0, 0, scale_gt, 0,
        0, 0,        0, 1;

  Mat x2 = x1;
  // Transform point from ground truth scale matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<3,1>(0,i) = m.block<3,3>(0,0) * x1.col(i);
  }
  Mat4 sim_mat;
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similarity3D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, m, 1e-8);
}

TEST(Similarity3DTest, RotationTranslationScale) {
  Mat x1(3, 4);
  x1 <<  0, 1, 1, 5,
         0, 5, 2, 3,
         0, 2, 0, 1;
  Mat4 M;
  M.setIdentity();
  /*
  M = AngleAxisd(45.0, Vector3f::UnitZ())
    * AngleAxisd(25.0, Vector3f::UnitX())
    * AngleAxisd(5.0, Vector3f::UnitZ());*/
      
  // Rotation on x + translation
  double angle = 45.0;
  Mat4 rot;
  rot <<  1,          0,           0, 1,
          0, cos(angle), -sin(angle), 3,
          0, sin(angle),  cos(angle),-2,
          0,          0,           0, 1;
  M *= rot;
  // Rotation on y + scale
  double scale = 0.3;
  angle = 25.0;
  rot <<  scale*cos(angle), 0, scale*sin(angle),  0,
          0,            scale,                0,  0,
          scale*-sin(angle),0, scale*cos(angle),  0,
          0,                0,                0,  1;
  M *= rot;
  // Rotation on z
  angle = 5.0;
  rot <<  cos(angle), -sin(angle), 0, 0,
          sin(angle),  cos(angle), 0, 0,
          0,           0,          1, 0,
          0,           0,          0, 1;
  M *= rot;
  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<3,1>(0,i) = M.block<3,3>(0,0) * x1.col(i);
    x2.block<3,1>(0,i) += M.block<3,1>(0,3); // translation
  }
  Mat4 sim_mat;
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, &sim_mat));
  VLOG(1) << "Mat Similitude3D "<< std::endl << sim_mat;
  EXPECT_MATRIX_NEAR(sim_mat, M, 1e-8);
}

TEST(Similarity3DTest, CoplanarRealCase) {
  Mat x1(3, 4);
  x1 <<  0, 0, 0, 0.28,
         0, 0, 0, 0,
         0, 0.15, 0.34, 0;
 
  Mat x2t(4, 3);
  x2t << 
    -1.19925, -0.332088 , -1.82672,
    -1.18777, -0.268836, -1.58292,
    -1.17311, -0.193347, -1.28043,
    -1.13389, 0.172132, -1.93862;
  
  const double precision = 2e-2;
  
  Mat x2(3, 4);
  x2 = x2t.transpose();
  Mat4 sim_mat; 
  EXPECT_TRUE(Similarity3DFromCorrespondencesLinear(x1, x2, 
                                                    &sim_mat,
                                                    precision));
  VLOG(1) << "Mat Similitude3D "<< std::endl << sim_mat;
  
  Mat x2h(4,4), x1p;
  EuclideanToHomogeneous(x2, &x2h);
  HomogeneousToEuclidean(sim_mat.inverse() * x2h, &x1p);
  EXPECT_MATRIX_NEAR(x1, x1p, precision);
}
}
