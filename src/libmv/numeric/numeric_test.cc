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

#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

using namespace libmv;
/*
//using libmv::Mat;
using libmv::Mat2;
using libmv::Mat34;
using libmv::Vec;
using libmv::Vec2;
using libmv::Nullspace;
using libmv::Transpose;
*/

namespace {

TEST(Numeric, Nullspace) {
  Mat A(3, 4);
  A = 0.76026643, 0.01799744, 0.55192142, 0.8699745,
      0.42016166, 0.97863392, 0.33711682, 0.14479271,
      0.51016811, 0.66528302, 0.54395496, 0.57794893;
  Vec x;
  double s = Nullspace(&A, &x);
  EXPECT_NEAR(s, 0.122287, 1e-7);
  EXPECT_NEAR(x(0), -0.05473917, 1e-7);
  EXPECT_NEAR(x(1),  0.21822937, 1e-7);
  EXPECT_NEAR(x(2), -0.80258116, 1e-7);
  EXPECT_NEAR(x(3),  0.55248805, 1e-7);
}

TEST(Numeric, TinyMatrixNullspace) {
  Mat34 A;
  A = 0.76026643, 0.01799744, 0.55192142, 0.8699745,
      0.42016166, 0.97863392, 0.33711682, 0.14479271,
      0.51016811, 0.66528302, 0.54395496, 0.57794893;
  Vec x;
  double s = Nullspace(&A, &x);
  EXPECT_NEAR(s, 0.122287, 1e-7);
  EXPECT_NEAR(x(0), -0.05473917, 1e-7);
  EXPECT_NEAR(x(1),  0.21822937, 1e-7);
  EXPECT_NEAR(x(2), -0.80258116, 1e-7);
  EXPECT_NEAR(x(3),  0.55248805, 1e-7);
}

TEST(Numeric, TinyMatrixSquareTranspose) {
  Mat2 A;
  A = 1.0, 2.0, 3.0, 4.0;
  libmv::TransposeInPlace(&A);
  EXPECT_EQ(1.0, A(0, 0));
  EXPECT_EQ(3.0, A(0, 1));
  EXPECT_EQ(2.0, A(1, 0));
  EXPECT_EQ(4.0, A(1, 1));
}

TEST(Numeric, NormalizeL1) {
  Vec2 x;
  x = 1, 2;
  double l1 = NormalizeL1(&x);
  EXPECT_DOUBLE_EQ(3., l1);
  EXPECT_DOUBLE_EQ(1./3., x(0));
  EXPECT_DOUBLE_EQ(2./3., x(1));
}

TEST(Numeric, NormalizeL2) {
  Vec2 x;
  x = 1, 2;
  double l2 = NormalizeL2(&x);
  EXPECT_DOUBLE_EQ(sqrt(5.0), l2);
  EXPECT_DOUBLE_EQ(1./sqrt(5.), x(0));
  EXPECT_DOUBLE_EQ(2./sqrt(5.), x(1));
}

TEST(Numeric, Diag) {
  Vec x(2);
  x = 1, 2;
  Mat D;
  D = Diag(x);
  EXPECT_EQ(1, D(0,0));
  EXPECT_EQ(0, D(0,1));
  EXPECT_EQ(0, D(1,0));
  EXPECT_EQ(2, D(1,1));
}

TEST(Numeric, DeterminantSlow) {
  Mat A(2, 2);
  A =  1, 2,
      -1, 3;
  double detA = DeterminantSlow(A);
  EXPECT_NEAR(5, detA, 1e-8); 
  
  Mat B(4,4);
  B =  0,  1,  2,  3,
       4,  5,  6,  7,
       8,  9, 10, 11,
      12, 13, 14, 15;
  double detB = DeterminantSlow(B);
  EXPECT_NEAR(0, detB, 1e-8); 

  Mat3 C;
  C =  0, 1, 2,
       3, 4, 5,
       6, 7, 1;
  double detC = DeterminantSlow(C);
  EXPECT_NEAR(21, detC, 1e-8); 
}

TEST(Numeric, InverseSlow) {
  Mat A(2, 2), A1, I;
  A =  1, 2,
      -1, 3;
  InverseSlow(A, &A1);
  I = A * A1;

  EXPECT_NEAR(1, I(0,0), 1e-8); 
  EXPECT_NEAR(0, I(0,1), 1e-8); 
  EXPECT_NEAR(0, I(1,0), 1e-8); 
  EXPECT_NEAR(1, I(1,1), 1e-8); 

  Mat B(4,4), B1;
  B =  0,  1,  2,  3,
       4,  5,  6,  7,
       8,  9,  2, 11,
      12, 13, 14,  4;
  InverseSlow(B, &B1);
  I = B * B1;
  EXPECT_NEAR(1, I(0,0), 1e-8); 
  EXPECT_NEAR(0, I(0,1), 1e-8); 
  EXPECT_NEAR(0, I(0,2), 1e-8); 
  EXPECT_NEAR(0, I(1,0), 1e-8); 
  EXPECT_NEAR(1, I(1,1), 1e-8); 
  EXPECT_NEAR(0, I(1,2), 1e-8); 
  EXPECT_NEAR(0, I(2,0), 1e-8); 
  EXPECT_NEAR(0, I(2,1), 1e-8); 
  EXPECT_NEAR(1, I(2,2), 1e-8); 
}

TEST(Numeric, MeanAndVarianceAlongRows) {
  int n = 4;
  Mat points(2,n);
  points = 0, 0, 1, 1,
           0, 2, 1, 3; 

  Vec mean, variance;
  MeanAndVarianceAlongRows(points, &mean, &variance);

  EXPECT_NEAR(0.5, mean(0), 1e-8);
  EXPECT_NEAR(1.5, mean(1), 1e-8);
  EXPECT_NEAR(0.25, variance(0), 1e-8);
  EXPECT_NEAR(1.25, variance(1), 1e-8);
}

TEST(Numeric, HorizontalStack) {
  Mat x(2,1), y(2,1), z;
  x = 1, 2;
  y = 3, 4;

  HorizontalStack(x, y, &z);

  EXPECT_EQ(2, z.numCols());
  EXPECT_EQ(2, z.numRows());
  EXPECT_EQ(1, z(0,0));
  EXPECT_EQ(2, z(1,0));
  EXPECT_EQ(3, z(0,1));
  EXPECT_EQ(4, z(1,1));
}

TEST(Numeric, VerticalStack) {
  Mat x(1,2), y(1,2), z;
  x = 1, 2;
  y = 3, 4;
  VerticalStack(x, y, &z);

  EXPECT_EQ(2, z.numCols());
  EXPECT_EQ(2, z.numRows());
  EXPECT_EQ(1, z(0,0));
  EXPECT_EQ(2, z(0,1));
  EXPECT_EQ(3, z(1,0));
  EXPECT_EQ(4, z(1,1));
}

TEST(Numeric, CrossProduct) {
  Vec3 x, y, z;
  x = 1, 0, 0;
  y = 0, 1, 0;
  z = 0, 0, 1;
  Vec3 xy = CrossProduct(x, y);
  Vec3 yz = CrossProduct(y, z);
  Vec3 zx = CrossProduct(z, x);
  EXPECT_NEAR(0, DistanceLInfinity(xy, z), 1e-8);
  EXPECT_NEAR(0, DistanceLInfinity(yz, x), 1e-8);
  EXPECT_NEAR(0, DistanceLInfinity(zx, y), 1e-8);
}

TEST(Numeric, CrossProductMatrix) {
  Vec3 x, y;
  x = 1, 2, 3;
  y = 2, 3, 4;
  Vec3 xy = CrossProduct(x, y);
  Vec3 yx = CrossProduct(y, x);
  Mat3 X = CrossProductMatrix(x);
  Vec3 Xy, Xty;
  Xy = X * y;
  Xty = transpose(X) * y;
  EXPECT_NEAR(0, DistanceLInfinity(xy, Xy), 1e-8);
  EXPECT_NEAR(0, DistanceLInfinity(yx, Xty), 1e-8);
}

TEST(Numeric, MatrixColumn) {
  Mat A2(2,3);
  Vec2 v2;
  A2 = 1, 2, 3,
       4, 5, 6;
  MatrixColumn(A2, 1, &v2);
  EXPECT_EQ(2, v2(0));
  EXPECT_EQ(5, v2(1));

  Mat A3(3,3);
  Vec3 v3;
  A3 = 1, 2, 3,
       4, 5, 6,
       7, 8, 9;
  MatrixColumn(A3, 1, &v3);
  EXPECT_EQ(2, v3(0));
  EXPECT_EQ(5, v3(1));
  EXPECT_EQ(8, v3(2));

  Mat A4(4,3);
  Vec4 v4;
  A4 =  1,  2,  3,
        4,  5,  6,
        7,  8,  9,
       10, 11, 12;
  MatrixColumn(A4, 1, &v4);
  EXPECT_EQ( 2, v4(0));
  EXPECT_EQ( 5, v4(1));
  EXPECT_EQ( 8, v4(2));
  EXPECT_EQ(11, v4(3));
}

// This gives a compile error.
//TEST(Numeric, TinyMatrixView) {
//  Mat34 P;
//  Mat K;
//  K = P(_, _(0, 2));
//}

// This gives a compile error.
//TEST(Numeric, Mat3MatProduct) {
//  Mat3 A;
//  Mat B, C;
//  C = A * B;
//}

// This gives a compile error.
//TEST(Numeric, Vec3Negative) {
//  Vec3 x;
//  Vec3 y;
//  x = -y;
//}

// This gives a compile error.
//TEST(Numeric, Vec3VecInteroperability) {
//  Vec3 x;
//  Vec y;
//  x = y + y;
//}

// This segfaults inside lapack.
//TEST(Numeric, DeterminantLU7) {
//  Mat A(5, 5);
//  A =  1, 0, 0, 0, 0,
//       0, 1, 0, 0, 0,
//       0, 0, 1, 0, 0,
//       0, 0, 0, 1, 0,
//       0, 0, 0, 0, 1;
//  double detA = DeterminantLU(&A);
//  EXPECT_NEAR(5, detA, 1e-8); 
//}

// This segfaults inside lapack.
//TEST(Numeric, DeterminantLU) {
//  Mat A(2, 2);
//  A =  1, 2,
//      -1, 3;
//  double detA = DeterminantLU(&A);
//  EXPECT_NEAR(5, detA, 1e-8); 
//}

// This does unexpected things.
//TEST(Numeric, InplaceProduct) {
//  Mat K(2,2), S(2,2);
//  K = 1, 0,
//      0, 1;
//  S = 1, 0,
//      0, 1;
//  K = K * S; // This sets K to zero without warning!
//  EXPECT_NEAR(1, K(0,0), 1e-8);
//  EXPECT_NEAR(0, K(0,1), 1e-8);
//  EXPECT_NEAR(0, K(1,0), 1e-8);
//  EXPECT_NEAR(1, K(1,1), 1e-8);
//}

}  // namespace
