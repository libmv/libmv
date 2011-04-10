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

#include "testing/testing.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/affine.h"
namespace {
using namespace libmv;

TEST(Affine2DTest, TranslationX) {
  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         0, 1, 1;

  Mat3 AffineMat;
  EXPECT_TRUE(Affine2DFromCorrespondencesLinear(x1, x2, &AffineMat));
  VLOG(1) << "Mat Affine2D "<< std::endl <<AffineMat;
  Mat3 ground_truth;
  ground_truth << 1,0,1,
                  0,1,0,
                  0,0,1;
  EXPECT_MATRIX_NEAR(AffineMat, ground_truth,1e-8);
}

TEST(Affine2DTest, TranslationXY) {
  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         1, 2, 2;

  Mat3 affine_mat;
  EXPECT_TRUE(Affine2DFromCorrespondencesLinear(x1, x2, &affine_mat));
  VLOG(1) << "Mat Affine2D "<< std::endl << affine_mat;
  Mat3 ground_truth;
  ground_truth << 1,0,1,
                  0,1,1,
                  0,0,1;
  EXPECT_MATRIX_NEAR(affine_mat, ground_truth,1e-8);
}

TEST(Affine2DTest, Rotation45) {
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  double angle = 55.0;
  Mat3 rot;
  rot <<  cos(angle), -sin(angle), 0,
          sin(angle),  cos(angle), 0,
          0,           0,          1;
  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) * x1.col(i) ;
  }

  Mat3 affine_mat;
  EXPECT_TRUE(Affine2DFromCorrespondencesLinear(x1, x2, &affine_mat));
  VLOG(1) << "Mat Affine2D " << affine_mat<< std::endl;
  EXPECT_MATRIX_NEAR(affine_mat, rot, 1e-8);
}

TEST(Affine2DTest, Rotation45AndTranslationXY) {
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  double angle = 45.0;
  Mat3 rot;
  rot <<  cos(angle),  sin(angle), -2,
         -sin(angle),  cos(angle), 5,
          0,           0,          1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = rot.block<2,2>(0,0) *  x1.col(i);// rot
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  Mat3 affine_mat;
  EXPECT_TRUE(Affine2DFromCorrespondencesLinear(x1, x2, &affine_mat));
  VLOG(1) << "Mat Affine2D "<< std::endl << affine_mat;
  EXPECT_MATRIX_NEAR(affine_mat, rot, 1e-8);
}

TEST(Affine2DTest, AffineTransXY) {
  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;
  Mat3 m;
  m <<   3, -1,  4,
         6, -2, -3,
         0,  0,  1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i = 0; i < x2.cols(); ++i)  {
    x2.block<2,1>(0,i) = m.block<2,2>(0,0) * x1.col(i);// rot-scale
    x2.block<2,1>(0,i) += m.block<2,1>(0,2); // translation
  }

  Mat3 affine_mat;
  EXPECT_TRUE(Affine2DFromCorrespondencesLinear(x1, x2, &affine_mat));
  VLOG(1) << "Mat Affine2D "<< std::endl << affine_mat;
  EXPECT_MATRIX_NEAR(affine_mat, m, 1e-8);
}
}
