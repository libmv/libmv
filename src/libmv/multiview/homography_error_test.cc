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

#include "libmv/logging/logging.h"
#include "libmv/multiview/homography_error.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

using testing::Types;

namespace {

using namespace libmv;
using namespace libmv::homography::homography2D;

TEST(Homography2D, AsymmetricError) {
  Mat3 H;
  H << 1, 0, -4,
       0, 1,  5,
       0, 0,  1;
  // Define a set of points.
  Mat x(2, 9), xh;
  x << 0, 0, 0, 1, 1, 1, 2, 2, 2,
       0, 1, 2, 0, 1, 2, 0, 1, 2;
  EuclideanToHomogeneous(x, &xh);
  Mat x2h_gt(3, 9);
  x2h_gt = H * xh;
  Mat x2h(2, 9), x2;
  const double cst_diff = 1.5;
  Vec2 err; err << cst_diff, cst_diff;
  x2h = x2h_gt;
  x2h.block<2, 9>(0, 0).array() += cst_diff;//1e-3 * Eigen::Matrix<double, 2, 9>::Random();
  HomogeneousToEuclidean(x2h, &x2);

  Mat2X dx1, dx2, dx3, dx4;
  AsymmetricError::Residuals(H, x, x2h, &dx1);
  AsymmetricError::Residuals(H, x, x2,  &dx2);
  AsymmetricError::Residuals(H, xh, x2h,&dx3);
  AsymmetricError::Residuals(H, xh, x2, &dx4);
  double norm1,norm2,norm3,norm4;
  norm1 = AsymmetricError::Error(H, x, x2h);
  norm2 = AsymmetricError::Error(H, x, x2);
  norm3 = AsymmetricError::Error(H, xh, x2h);
  norm4 = AsymmetricError::Error(H, xh, x2);

  double cst_diff2 = cst_diff * cst_diff;
  cst_diff2 = (cst_diff2 + cst_diff2)* 9;
  for (int i = 0; i < x.cols(); i++) {
    EXPECT_MATRIX_NEAR(dx1.col(i), err, 1e-8);
    EXPECT_MATRIX_NEAR(dx2.col(i), err, 1e-8);
    EXPECT_MATRIX_NEAR(dx3.col(i), err, 1e-8);
    EXPECT_MATRIX_NEAR(dx4.col(i), err, 1e-8);
    
    EXPECT_NEAR(norm1, cst_diff2, 1e-8);
    EXPECT_NEAR(norm2, cst_diff2, 1e-8);
    EXPECT_NEAR(norm3, cst_diff2, 1e-8);
    EXPECT_NEAR(norm4, cst_diff2, 1e-8);
  }
}
// TODO(julien) Make tests for symmetric errors

TEST(Homography2D, AlgebraicError) {
  Mat3 H;
  H << 1, 0, -4,
       0, 1,  5,
       0, 0,  1;
  // Define a set of points.
  Mat x(2, 9), xh;
  x << 0, 0, 0, 1, 1, 1, 2, 2, 2,
       0, 1, 2, 0, 1, 2, 0, 1, 2;
  EuclideanToHomogeneous(x, &xh);
  Mat x2h_gt(3, 9);
  x2h_gt = H * xh;
  Mat x2h(2, 9), x2;
  const double cst_diff = 1.5;
  Vec2 err; err << cst_diff, cst_diff;
  x2h = x2h_gt;
  x2h.block<2, 9>(0, 0).array() += cst_diff;
  HomogeneousToEuclidean(x2h, &x2);

  Mat3X dx1, dx2, dx3, dx4;
  AlgebraicError::Residuals(H, x, x2h, &dx1);
  AlgebraicError::Residuals(H, x, x2,  &dx2);
  AlgebraicError::Residuals(H, xh, x2h,&dx3);
  AlgebraicError::Residuals(H, xh, x2, &dx4);
  double norm1,norm2,norm3,norm4;
  norm1 = AlgebraicError::Error(H, x, x2h);
  norm2 = AlgebraicError::Error(H, x, x2);
  norm3 = AlgebraicError::Error(H, xh, x2h);
  norm4 = AlgebraicError::Error(H, xh, x2);
  
  double norm_err = 1707.75;
  for (int i = 0; i < x.cols(); i++) {
    EXPECT_MATRIX_NEAR(dx1.col(i), dx2.col(i), 1e-8);
    EXPECT_MATRIX_NEAR(dx2.col(i), dx3.col(i), 1e-8);
    EXPECT_MATRIX_NEAR(dx3.col(i), dx4.col(i), 1e-8);
    
    // TODO(julien) Also check the dx values.
    
    EXPECT_NEAR(norm1, norm_err, 1e-8);
    EXPECT_NEAR(norm2, norm_err, 1e-8);
    EXPECT_NEAR(norm3, norm_err, 1e-8);
    EXPECT_NEAR(norm4, norm_err, 1e-8);
  }
}
}  // namespace
