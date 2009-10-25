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

#include "libmv/multiview/2DAffine.h"
#include "testing/testing.h"
#include "libmv/logging/logging.h"
#include <iostream>
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
  EXPECT_TRUE(Affine2D_FromCorrespondencesLinear( x1, x2, &AffineMat));
  std::cout << "Mat Affine2D "<< std::endl <<AffineMat;
  Mat3 GroundTruth;
  GroundTruth << 1,0,1,
                 0,1,0,
                 0,0,1;
  EXPECT_MATRIX_NEAR(AffineMat, GroundTruth,1e-8);
}

TEST(Affine2DTest, TranslationXY) {

  Mat x1(2, 3);
  x1 <<  0, 1, 2,
         0, 1, 1;

  Mat x2(2, 3);
  x2 <<  1, 2, 3,
         1, 2, 2;

  Mat3 AffineMat;
  EXPECT_TRUE(Affine2D_FromCorrespondencesLinear( x1, x2, &AffineMat));
  std::cout << "Mat Affine2D "<< std::endl << AffineMat;
  Mat3 GroundTruth;
  GroundTruth << 1,0,1,
                 0,1,1,
                 0,0,1;
  EXPECT_MATRIX_NEAR(AffineMat, GroundTruth,1e-8);
}

TEST(Affine2DTest, Rotation45) {

  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  double angle = 45.0;
  Mat3 rot;
  rot <<  cos(angle), -sin(angle), 0,
          sin(angle),  cos(angle), 0,
          0,           0,          1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i=0; i < x2.cols(); ++i)
  {
    x2.block<2,1>(0,i) = x1.col(i).transpose() * rot.block<2,2>(0,0);
  }

  Mat3 AffineMat;
  EXPECT_TRUE(Affine2D_FromCorrespondencesLinear( x1, x2, &AffineMat));
  std::cout << "Mat Affine2D "<< std::endl << AffineMat;
  EXPECT_MATRIX_NEAR(AffineMat, rot, 1e-8);
}

TEST(Affine2DTest, Rotation45AndTranslationXY) {

  Mat x1(2, 4);
  x1 <<  0, 1, 2, 5,
         0, 1, 2, 3;

  double angle = 45.0;
  Mat3 rot;
  rot <<  cos(angle), -sin(angle), -2,
          sin(angle),  cos(angle), 5,
          0,           0,          1;

  Mat x2 = x1;
  // Transform point from ground truth rotation matrix
  for(int i=0; i < x2.cols(); ++i)
  {
    x2.block<2,1>(0,i) = x1.col(i).transpose() * rot.block<2,2>(0,0);// rot
    x2.block<2,1>(0,i) += rot.block<2,1>(0,2); // translation
  }

  Mat3 AffineMat;
  EXPECT_TRUE(Affine2D_FromCorrespondencesLinear( x1, x2, &AffineMat));
  std::cout << "Mat Affine2D "<< std::endl << AffineMat;
  EXPECT_MATRIX_NEAR(AffineMat, rot, 1e-8);
}

}
