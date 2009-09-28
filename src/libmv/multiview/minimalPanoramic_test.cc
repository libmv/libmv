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

#include <iostream>

#include "libmv/logging/logging.h"
#include "libmv/multiview/minimalPanoramic.h"
#include "libmv/multiview/projection.h"

#include "libmv/multiview/minimalPanoramic_kernel.h"


#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

TEST(MinimalPanoramic, SharedFocalEstimation)
{
  Vec2 u11,u12, u21, u22;

  u11 << 2, 2;
  u12 << 3, 3;

  u21 << 2, 3;
  u22 << 3, 4;

  vector<double> v_focals;
  F_FromCorrespondance_2points( u11, u12, u21, u22, &v_focals);

  EXPECT_EQ(2, v_focals.size());

  for(int i=0; i< v_focals.size(); ++i)
    std::cout<< " Focals("<<i<<"): " << v_focals[i] << std::endl;
}

TEST(MinimalPanoramic, Homography_GetR)
{
  // Test if we find identity transformation matrix with the same point cloud.
  Mat x1(3,3);
  x1  <<  0.5,  0.6,  0.7,
          0.5,  0.5,  0.4,
          10,    10,    10;

  Mat3 K1 = Mat3::Identity();
  Mat3 K2 = K1;

  Mat3 R;
  GetR_FixedCameraCenter( x1, x1, K1, K2, &R);
  R /= R(2,2);
  EXPECT_MATRIX_NEAR( Mat3::Identity(), R, 1e-8);
  std::cout << std::endl << "rot" << std::endl << R <<std::endl;
}

TEST(MinimalPanoramic, Homography_GetR_Test_PitchY30)
{
  const int n = 3;

  Mat x1(3,n);
  x1 << 0.5, 0.6, 0.7,
        0.5, 0.5, 0.4,
        10,   10,  10;

  Mat x2 = x1;
  const double alpha = 30.0 * 3.14159265 / 180.0;;
  Mat3 rotY;
  rotY << cos(alpha), 0, -sin(alpha), 0, 1, 0, sin(alpha), 0, cos(alpha);

  for (int i = 0; i < n; ++i) {
    x2.block<3,1>(0,i) = rotY * x1.col(i);
  }


  Mat3 K1 = Mat3::Identity();
  Mat3 K2 = K1;

  Mat3 R;
  GetR_FixedCameraCenter( x1, x2, K1, K2, &R);

  // Assert that residuals are small enough
  for(int i=0; i < n; ++i)
  {
    Vec residuals = (R * x1.col(i)) - x2.col(i);
    EXPECT_NEAR(0, abs(residuals(0)) + abs(residuals(1)) + abs(residuals(2)), 1e-6);
  }

  //-- Check that the rotation angle along Y is the expected one
  // Euler approximation :
  float PitchY = asin( R(2,0) ) * 180.0 / 3.14159265;
  EXPECT_NEAR(30,  PitchY, 1e-4);
}

TEST(MinimalPanoramic, Real_Case_Kernel)
{
  const int n = 2;
  Mat x1(2,n); //image 0.jpg
  x1<< 164, 215,
       120, 107;

  Mat x2(2,n); //image 1.jpg
  x2<<  213, 264,
        120, 107;

  vector<Mat3> v_H;
  
  libmv::panography::kernel::TwoPointSolver::Solve( x1,x2, &(v_H));

  // Todo(pmoulon) : Setup the robust case.
  //double threshold = 2 * Square(0.1);
  //libmv::panography::kernel::Kernel kernel(x1, x2);
  //Mat F;
  //vector<int> inliers;
  //F = Estimate(kernel, MLEScorer<libmv::panography::kernel::Kernel>(threshold), &inliers);
  //v_H.push_back(F);

  Mat3 H = v_H[0];

  Mat x1H, x2H;
  EuclideanToHomogeneous(x1,&x1H);
  EuclideanToHomogeneous(x2,&x2H);

  // Assert that residuals are small enough
  for(int i=0; i < n; ++i)
  {
    Vec x1Prime = H * x1H.col(i);
    Vec residuals = (x1Prime/x1Prime(2)) - x2H.col(i);
    EXPECT_MATRIX_NEAR_ZERO(residuals, 1e-5);
  }
}

// Todo(pmoulon) Use real_case in a other file.
// dependancy over image code module
/*
//#include <string>
//using std::string;
//#include "libmv/multiview/robust_estimation.h"
//#include "libmv/image/image.h"
//#include "libmv/image/image_io.h"
//#include "libmv/image/sample.h"
TEST(MinimalPanoramic, Real_Case)
{
  //E:\2009Projects\libmv\src\libmv\multiview\panoramicSet

  const int n = 4;
  Mat x1(2,n); //image 0.jpg
  x1<< 164, 215, 144, 90,
       120, 107, 190, 195;

  Mat x2(2,n); //image 1.jpg
  x2<<  213, 267, 192, 139,
        120, 107, 190, 195;

  vector<double> v_focals;
  F_FromCorrespondance_2points( x1.col(0) , x1.col(1), x2.col(0), x2.col(1), &v_focals);

  for(int i=0; i< v_focals.size(); ++i)
    std::cout<< " Focals("<<i<<"): " << v_focals[i] << std::endl;

  Mat x1H, x2H;
  EuclideanToHomogeneous(x1,&x1H);
  EuclideanToHomogeneous(x2,&x2H);

  Mat3 K1 = Mat3::Identity() * v_focals[0];
  K1(2,2) = 1;
  Mat3 K2 = K1;

  Mat3 R;
  GetR_FixedCameraCenter( x1H, x2H, K1, K2, &R);

  std::cout << std::endl << "rot" << std::endl << R <<std::endl;
  double radianToDeg = 180/3.14;

  std::cout << "yawX" << atan2( R(2,1) , R(2,2) ) * radianToDeg <<std::endl;
  std::cout << "pitchY"<< - asin( R(2,0) ) * radianToDeg <<std::endl;
  std::cout << "roolZ" << atan2( R(1,0), R(0,0) ) * radianToDeg <<std::endl;

  Mat3 H = K2 * R * K1.inverse();
  // Assert that residuals are small enough
  for(int i=0; i < n; ++i)
  {
    Vec x1Prime = H * x1H.col(i);
    Vec residuals = (x1Prime/x1Prime(2)) - x2H.col(i);
    std::cout << std::endl << "res" << std::endl << residuals <<std::endl;
  }

  //-- Perform image reprojection :
  std::string filename0 = ("E:/2009Projects/libmv/src/libmv/multiview/panoramicSet/0.jpg");
  std::string filename1 = ("E:/2009Projects/libmv/src/libmv/multiview/panoramicSet/1.jpg");

  Array3Du image0, image1;
  ReadJpg(filename0.c_str(), &image0);
  ReadJpg(filename1.c_str(), &image1);

  Mat3 Hinv = H.inverse();
  for(int j=0; j < image0.Height(); ++j)
    for(int i=0; i < image0.Width(); ++i)
    {
      Vec3 Pos;
      Pos << i,j,1;
      Vec3 Pos2 = H * Pos;
      Vec3 posImage2 = Pos2/Pos2(2);
      if( image1.Contains( posImage2(1), posImage2(0) ) )
      {
        image0(j,i,0) = (image0(j,i,0) + SampleLinear( image1, posImage2(1),posImage2(0),0))/2;
        image0(j,i,1) = (image0(j,i,1) + SampleLinear( image1, posImage2(1),posImage2(0),1))/2;
        image0(j,i,2) = (image0(j,i,2) + SampleLinear( image1, posImage2(1),posImage2(0),2))/2;
      }
      int a=2;
    }

   std::string out_filename = ("E:/2009Projects/libmv/src/libmv/multiview/panoramicSet/out.jpg");
  EXPECT_TRUE(WriteJpg(image0, out_filename.c_str(), 100));
}

TEST(MinimalPanoramic, Real_Case_Kernel)
{
  const int n = 4;
  Mat x1(2,n); //image 0.jpg
  x1<< 164, 215, 144, 90,
       120, 107, 190, 195;

  Mat x2(2,n); //image 1.jpg
  x2<<  213, 267, 192, 139,
        120, 107, 190, 195;

  vector<Mat3> v_H;
  
  libmv::panography::kernel::TwoPointSolver::Solve( x1,x2, &(v_H));

  // Todo(pmoulon) : Setup the robust case.
  //double threshold = 2 * Square(0.1);
  //libmv::panography::kernel::Kernel kernel(x1, x2);
  //Mat F;
  //vector<int> inliers;
  //F = Estimate(kernel, MLEScorer<libmv::panography::kernel::Kernel>(threshold), &inliers);
  //v_H.push_back(F);

  Mat3 H = v_H[0];

  Mat x1H, x2H;
  EuclideanToHomogeneous(x1,&x1H);
  EuclideanToHomogeneous(x2,&x2H);

  // Assert that residuals are small enough
  for(int i=0; i < n; ++i)
  {
    Vec x1Prime = H * x1H.col(i);
    Vec residuals = (x1Prime/x1Prime(2)) - x2H.col(i);
    std::cout << std::endl << "res" << std::endl << residuals <<std::endl;
  }

  //-- Perform image reprojection :
  std::string filename0 = ("E:/2009Projects/libmv/src/libmv/multiview/panoramicSet/0.jpg");
  std::string filename1 = ("E:/2009Projects/libmv/src/libmv/multiview/panoramicSet/1.jpg");

  Array3Du image0, image1;
  ReadJpg(filename0.c_str(), &image0);
  ReadJpg(filename1.c_str(), &image1);

  Mat3 Hinv = H.inverse();
  for(int j=0; j < image0.Height(); ++j)
    for(int i=0; i < image0.Width(); ++i)
    {
      Vec3 Pos;
      Pos << i,j,1;
      Vec3 Pos2 = H * Pos;
      Vec3 posImage2 = Pos2/Pos2(2);
      if( image1.Contains( posImage2(1), posImage2(0) ) )
      {
        image0(j,i,0) = (image0(j,i,0) + SampleLinear( image1, posImage2(1),posImage2(0),0))/2;
        image0(j,i,1) = (image0(j,i,1) + SampleLinear( image1, posImage2(1),posImage2(0),1))/2;
        image0(j,i,2) = (image0(j,i,2) + SampleLinear( image1, posImage2(1),posImage2(0),2))/2;
      }
      int a=2;
    }

   std::string out_filename = ("E:/2009Projects/libmv/src/libmv/multiview/panoramicSet/out.jpg");
  EXPECT_TRUE(WriteJpg(image0, out_filename.c_str(), 100));
}*/



}
