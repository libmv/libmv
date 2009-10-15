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

#include "libmv/logging/logging.h"
#include "libmv/multiview/panography.h"
#include "libmv/multiview/panography_kernel.h"
#include "libmv/multiview/projection.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace libmv {
namespace {

TEST(Panography, PrintSomeSharedFocalEstimationValues) {
  Vec2 u11, u12, u21, u22;

  u11 << 2, 2;
  u12 << 3, 3;
  u21 << 2, 3;
  u22 << 3, 4;

  vector<double> fs;
  F_FromCorrespondance_2points( u11, u12, u21, u22, &fs);

  // TODO(pmoulon): Fix this test!
}

TEST(Panography, GetR_FixedCameraCenterWithIdentity) {
  Mat x1(3,3);
  x1  <<  0.5,  0.6,  0.7,
          0.5,  0.5,  0.4,
         10.0, 10.0, 10.0;

  Mat3 K1 = Mat3::Identity();
  Mat3 K2 = K1;

  Mat3 R;
  GetR_FixedCameraCenter(x1, x1, K1, K2, &R);
  R /= R(2,2);
  EXPECT_MATRIX_NEAR( Mat3::Identity(), R, 1e-8);
  LOG(INFO) << "R \n" << R;
}

TEST(Panography, Homography_GetR_Test_PitchY30) {
  int n = 3;

  Mat x1(3,n);
  x1 << 0.5, 0.6, 0.7,
        0.5, 0.5, 0.4,
        10,   10,  10;

  Mat x2 = x1;
  const double alpha = 30.0 * M_PI / 180.0;;
  Mat3 rotY;
  rotY << cos(alpha), 0, -sin(alpha),
               0,     1,      0,
          sin(alpha), 0,  cos(alpha);

  for (int i = 0; i < n; ++i) {
    x2.block<3,1>(0, i) = rotY * x1.col(i);
  }

  Mat3 K1 = Mat3::Identity();
  Mat3 K2 = K1;

  Mat3 R;
  GetR_FixedCameraCenter(x1, x2, K1, K2, &R);

  // Assert that residuals are small enough
  for (int i = 0; i < n; ++i) {
    Vec residuals = (R * x1.col(i)) - x2.col(i);
    EXPECT_NEAR(0, residuals.norm(), 1e-6);
  }

  // Check that the rotation angle along Y is the expected one.
  // Use the euler approximation to recover the angle.
  double pitch_y = asin(R(2,0)) * 180.0 / M_PI;
  EXPECT_NEAR(30, pitch_y, 1e-4);
}

TEST(MinimalPanoramic, Real_Case_Kernel)
{
  int n = 2;
  Mat x1(2,n);  // From image 0.jpg
  x1<< 164, 215,
       120, 107;

  Mat x2(2,n);  // From image 1.jpg
  x2<<  213, 264,
        120, 107;

  vector<Mat3> Hs;
  
  libmv::panography::kernel::TwoPointSolver::Solve(x1, x2, &Hs);

  LOG(INFO) << "Got " << Hs.size() << " solutions.";
  for (int j = 0; j < Hs.size(); ++j) {
    Mat3 H = Hs[j];
    
    Mat x1h, x2h;
    EuclideanToHomogeneous(x1,&x1h);
    EuclideanToHomogeneous(x2,&x2h);
    
    // Assert that residuals are small enough
    for (int i = 0; i < n; ++i) {
      Vec x1p = H * x1h.col(i);
      Vec residuals = x1p/x1p(2) - x2h.col(i);
      EXPECT_MATRIX_NEAR_ZERO(residuals, 1e-5);
    }
  }
}

}  // namespace
}  // namespace libmv

// TODO(pmoulon): Add a real test case based on images.
// TODO(pmoulon): Add a check for the actual f value for the real images.
// TODO(pmoulon): Add a test that has some inliers and outliers.
