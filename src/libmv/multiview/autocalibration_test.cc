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

#include "libmv/multiview/projection.h"
#include "libmv/multiview/autocalibration.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(AutoCalibration, K_From_AbsoluteConic) {
  Mat3 K, Kp;
  K << 10,  1, 30,
        0, 20, 40,
        0,  0,  1;

  Mat3 w = (K * K.transpose()).inverse();
  K_From_AbsoluteConic(w, &Kp);

  EXPECT_MATRIX_NEAR(K, Kp, 1e-8);
}

// Tests that K computed from the IAC has positive elements in its diagonal.
TEST(AutoCalibration, K_From_AbsoluteConic_SignedDiagonal) {
  Mat3 K, Kpositive, Kp;
  K << 10,   1, 30,
        0, -20, 40,
        0,   0,  -1;
  Kpositive << 10, -1, -30,  // K with column signs changed so that the 
                0, 20, -40,  // diagonal is positive.
                0,  0,   1;

  Mat3 w = (K * K.transpose()).inverse();
  K_From_AbsoluteConic(w, &Kp);

  EXPECT_MATRIX_NEAR(Kpositive, Kp, 1e-8);
}

TEST(AutoCalibrationLinear, Solve) {
  double width = 1000, height = 800;
  Mat3 K;
  K << width,     0,  width / 2, // 1000x800 image with 35mm equiv focal length.
           0, width, height / 2,
           0,     0,          1;

  AutoCalibrationLinear a;

  // Add cameras with random rotation and translation.
  for (int i = 0; i < 3; ++i) {
    Mat3 R = RotationAroundX(double(rand()) / RAND_MAX * 3)
           * RotationAroundY(double(rand()) / RAND_MAX * 3)
           * RotationAroundZ(double(rand()) / RAND_MAX * 3);
    Vec3 t(double(rand())/RAND_MAX,
           double(rand())/RAND_MAX,
           double(rand())/RAND_MAX);
    Mat34 P;
    P_From_KRt(K, R, t, &P);
    a.AddProjection(P, width, height);

  }
  a.ConstrainSingleCalibration();
  
  // Compute metric update transformation.
  Mat H = a.MetricTransformation();

  // Since the input was metric, the transformation should be metric.
  // The 3x3 submatrix should be orthonormal.
  Mat R = H.block<3,3>(0,0);
  Mat RRt = R * R.transpose();
  Mat Id = Mat::Identity(3,3);
  EXPECT_MATRIX_NEAR(Id, RRt, 1e-6);

  // The plane at infinity should be 0,0,0,1.
  Vec3 p = H.row(3).start<3>();
  EXPECT_NEAR(0, p.norm(), 1e-6);
}


} // namespace
