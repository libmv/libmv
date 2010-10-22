// Copyright (c) 2009 libmv authors.
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

#include "euclidean_resection.h"
#include "libmv/numeric/numeric.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "testing/testing.h"

using namespace libmv::resection;
using namespace libmv;

// Generates all necessary inputs and desigred outputs for
// the EuclideanResection.
void CreateCameraSystem(const Mat3& KK,
                        const Mat3X& x_image,
                        const Vec& X_distances,
                        const Mat3& R_input,
                        const Vec3& T_input,
                        Mat2X *x_cam,
                        Mat3X *X_world,
                        Mat3  *R_expected,
                        Vec3  *T_expected) {
  int num_points = x_image.cols();
  Mat3X x_unit_cam(3,num_points);
  x_unit_cam = KK.inverse() * x_image;
  // Normalized camera coordinates to be used as an input to the PnP function.
  *x_cam = x_unit_cam.block(0,0,2,num_points);
  //instead of NormalizeColumnVectors(&x_unit_cam);
  for (int i = 0; i < num_points; ++i){
    x_unit_cam.col(i).normalize();
  }

  // Create the 3D points int he camera system.
  Mat X_camera(3, num_points);
  for (int ii = 0; ii < num_points; ++ii) {
    X_camera.col(ii) = X_distances(ii) * x_unit_cam.col(ii);
  }

  //Apply the transformation to the camera 3D points
  Mat translation_matrix(3, num_points);
  translation_matrix.row(0).setConstant(T_input(0));
  translation_matrix.row(1).setConstant(T_input(1));
  translation_matrix.row(2).setConstant(T_input(2));

  *X_world = R_input * X_camera + translation_matrix;
  // Expected variables for comparison.
  *R_expected = R_input.transpose();
  *T_expected = *R_expected * ( - T_input);
};


TEST(AbsoluteOrientation, QuaternionSlution) {
  srand((unsigned)time(0));
  int num_points = 4;
  Mat X;
  Mat Xp;
  X = 100 * Mat::Random(3,num_points);
  // Create random translation and rotation.
  Mat3 R_input;
  R_input = Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitZ())
          * Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitY())
          * Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitZ());
  Vec3 t_input;
  t_input.setRandom();
  t_input=100*t_input;
  Mat translation_matrix(3, num_points);
  translation_matrix.row(0).setConstant(t_input(0));
  translation_matrix.row(1).setConstant(t_input(1));
  translation_matrix.row(2).setConstant(t_input(2));
  // Create Xp as Xp = R * X + t.
  Xp = R_input * X + translation_matrix;
  // Output variables.
  Mat3 R;
  Vec3 t;

  AbsoluteOrientation(X,Xp,&R,&t);

  EXPECT_MATRIX_NEAR(t, t_input, 1e-6);
  EXPECT_MATRIX_NEAR(R, R_input, 1e-8);

}

TEST(EuclideanResection, Points4KnownImagePointsRandomTranslationRotation) {

  // In this test only the translation and rottion are random.
  // The image points are selected from a real case and are well conditioned.
  Vec2i image_dimensions;
  image_dimensions << 1600, 1200;
  srand((unsigned)time(0));

  Mat3 KK;
  KK << 2796.813000676695538, 0,                     804.489474977057966,
        0 ,                   2796.483860262341295,  641.641715857649729,
        0,                    0,                     1;

  int num_points = 4;
  Mat3X x_image(3, num_points);
  // Image points.
  x_image << 1164.06, 734.948, 749.599, 430.727,
             681.386, 844.59, 496.315,  580.775,
             1,       1,      1,        1;

  // Normalized camera coordinates to be used as an input to the PnP function.
  Mat2X x_cam;
  Vec X_distances(num_points); // a vector of the 4 distances to the 3D points
  X_distances << 100 * Vec::Random(num_points).cwise().abs();
  // Transformation variables
  Mat3 R_input;
  R_input = Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitZ())
          * Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitY())
          * Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitZ());
  Vec3 T_input;
  T_input.setRandom();
  T_input=100 * T_input;

  Mat3 R_expected;
  Vec3 T_expected;
  Mat3X X_world;
  // Create the camera system.
  CreateCameraSystem(KK, x_image, X_distances, R_input, T_input,
                     &x_cam, &X_world, &R_expected, &T_expected);
  Mat3 R_output;
  Vec3 T_output;
  EuclideanResection(x_cam, X_world, 
                     &R_output, &T_output,
                     eRESECTION_ANSARDANIILIDIS);
 
  EXPECT_MATRIX_NEAR(T_output, T_expected, 1e-5);
  EXPECT_MATRIX_NEAR(R_output, R_expected, 1e-7);
  
  R_output.setIdentity();
  T_output.setZero();
  
  // For now, EPnP has no non-linear optimization and is not precise enough
  // with only 4 points.
  /*
  EuclideanResection(x_cam, X_world, 
                     &R_output, &T_output,
                     eRESECTION_EPNP);

  EXPECT_MATRIX_NEAR(T_output, T_expected, 1e-5);
  EXPECT_MATRIX_NEAR(R_output, R_expected, 1e-7);*/
}

TEST(EuclideanResection, Points6AllRandomInput) {

  int num_points = 6;
  Vec2i image_dimensions;
  image_dimensions << 1600, 1200;
  srand((unsigned)time(0));

  Mat3 KK;
  KK << 2796.813000676695538, 0,                     804.489474977057966,
        0 ,                   2796.483860262341295,  641.641715857649729,
        0,                    0,                     1;

  Mat3X x_image(3, num_points);
  // Randomly create the image points.
  x_image.row(0) = image_dimensions(0) * Vec::Random(num_points).cwise().abs()
                        + Vec::Random(num_points);
  x_image.row(1) = image_dimensions(1) * Vec::Random(num_points).cwise().abs()
                        + Vec::Random(num_points);
  x_image.row(2).setOnes();

  // Normalized camera coordinates to be used as an input to the PnP function.
  Mat2X x_cam;
  Vec X_distances(num_points); // a vector of the 4 distances to the 3D points
  X_distances << 100 * Vec::Random(num_points).cwise().abs();
  // Transformation variables.
  Mat3 R_input;
  R_input = Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitZ())
          * Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitY())
          * Eigen::AngleAxisd(rand(), Eigen::Vector3d::UnitZ());
  Vec3 T_input;
  T_input.setRandom();
  T_input=100 * T_input;

  Mat3 R_expected;
  Vec3 T_expected;
  Mat3X X_world;
  // Create the camera system.
  CreateCameraSystem(KK, x_image, X_distances, R_input, T_input,
                     &x_cam, &X_world, &R_expected, &T_expected);
  Mat3 R_output;
  Vec3 T_output;
  EuclideanResection(x_cam, X_world, 
                     &R_output, &T_output,
                     eRESECTION_ANSARDANIILIDIS);
 
  EXPECT_MATRIX_NEAR(T_output, T_expected, 1e-5);
  EXPECT_MATRIX_NEAR(R_output, R_expected, 1e-7);
  
  R_output.setIdentity();
  T_output.setZero();
  
  EuclideanResection(x_cam, X_world, 
                     &R_output, &T_output,
                     eRESECTION_EPNP);
 
  EXPECT_MATRIX_NEAR(T_output, T_expected, 1e-5);
  EXPECT_MATRIX_NEAR(R_output, R_expected, 1e-7);
  
  R_output.setIdentity();
  T_output.setZero();
  
  EuclideanResection(x_image, X_world, KK,
                     &R_output, &T_output);
 
  EXPECT_MATRIX_NEAR(T_output, T_expected, 1e-5);
  EXPECT_MATRIX_NEAR(R_output, R_expected, 1e-7);
}