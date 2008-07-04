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
//
// Test libmv's tiny matrix extensions to FLENS.

#include "testing/testing.h"
#include "third_party/flens/flens.h"

using flens::TinyMatrix;
using flens::TinyVector;
using flens::_;

typedef flens::DenseVector<flens::Array<double> > Vec;
typedef flens::GeMatrix<flens::FullStorage<double, flens::ColMajor> > Mat;

typedef TinyVector<double, 2> Vec2;
typedef TinyVector<double, 4> Vec4;
typedef TinyMatrix<double, 2, 2> Mat2;
typedef TinyMatrix<double, 3, 3> Mat3;
typedef TinyMatrix<double, 3, 4> Mat34;

namespace {

// TinyMatrix / TinyVector didn't obey the matrix / vector protocol set out by
// the rest of FLENS. These tests cover the added functionality necessary to
// make the tiny vectors and matrices compliant.
TEST(TinyVector, DimensionsMatchNormalProtocol) {
  Vec4 x;
  EXPECT_EQ(x.length(), 4);
}

TEST(TinyMatrix, DimensionsMatchNormalProtocol) {
  Mat34 A;
  EXPECT_EQ(A.numRows(), 3);
  EXPECT_EQ(A.numCols(), 4);
}

TEST(TinyVector, ResizeMatchesDenseAPI) {
  Vec4 x;
  x.resize(4);  // Does nothing, but should compile.
}

TEST(TinyVector, FirstRowAndColumnIsZero) {
  Vec4 x;
  EXPECT_EQ(x.firstIndex(), 0);
}

TEST(TinyMatrix, FirstRowAndColumnIsZero) {
  Mat34 A;
  EXPECT_EQ(A.firstRow(), 0);
  EXPECT_EQ(A.firstCol(), 0);
}

// Copying between views and tiny matrices.

TEST(TinyVector, CopyFromDenseVector) {
  Vec x(4);
  x = 1.0, 2.0, 3.0, 4.0;
  Vec4 y;
  y = x;
  EXPECT_EQ(y(0), 1.0);
  EXPECT_EQ(y(1), 2.0);
  EXPECT_EQ(y(2), 3.0);
  EXPECT_EQ(y(3), 4.0);
}

// For some reason I had to add an initialization to generalmatrix.tcc before 
// the following code would compile. This remains to be sure it compiles!
TEST(TinyMatrix, MatrixViewsCompileWithoutWarning) {
  Mat B(3,3);
  Mat::View vv = B(_(1,3), _(1,3));
  EXPECT_EQ(vv.numRows(), 3);
}

TEST(TinyMatrix, CopyToDenseMatrixView) {
  Mat2 A;
  A(0, 0) = 1.0;
  A(0, 1) = 2.0;
  A(1, 0) = 3.0;
  A(1, 1) = 4.0;

  // Confusingly, matrices are 1-indexed by default, and use inclusive slices.
  Mat B(3,3);
  B = 0;
  B(_(1,2),_(1,2)) = A;
  EXPECT_EQ(B(1, 1), 1.0);
  EXPECT_EQ(B(1, 2), 2.0);
  EXPECT_EQ(B(1, 3), 0.0);
  EXPECT_EQ(B(2, 1), 3.0);
  EXPECT_EQ(B(2, 2), 4.0);
  EXPECT_EQ(B(2, 3), 0.0);
  EXPECT_EQ(B(3, 1), 0.0);
  EXPECT_EQ(B(3, 2), 0.0);
  EXPECT_EQ(B(3, 3), 0.0);
}

TEST(TinyMatrix, CopyFromDenseMatrixView) {
  Mat B(3,3);
  B = 0;
  B(1, 1) = 1.0;
  B(1, 2) = 2.0;
  B(2, 1) = 3.0;
  B(2, 2) = 4.0;
  Mat2 A;
  A = B(_(1,2),_(1,2));
  EXPECT_EQ(A(0, 0), 1.0);
  EXPECT_EQ(A(0, 1), 2.0);
  EXPECT_EQ(A(1, 0), 3.0);
  EXPECT_EQ(A(1, 1), 4.0);
}

TEST(TinyVector, ScalarInitialization) {
  Vec2 x;
  x = 4.0;
  EXPECT_EQ(x(0), 4.0);
  EXPECT_EQ(x(1), 4.0);
}

TEST(TinyMatrix, ScalarInitialization) {
  Mat2 A;
  A = 4.0;
  EXPECT_EQ(A(0, 0), 4.0);
  EXPECT_EQ(A(0, 1), 4.0);
  EXPECT_EQ(A(1, 0), 4.0);
  EXPECT_EQ(A(1, 1), 4.0);
}

TEST(TinyVector, ListInitialization) {
  Vec4 x;
  x = 1.0, 2.0, 3.0;
  EXPECT_EQ(x(0), 1.0);
  EXPECT_EQ(x(1), 2.0);
  EXPECT_EQ(x(2), 3.0);
}

TEST(TinyMatrix, ListInitialization) {
  Mat2 A;
  A = 1.0, 2.0,
      3.0, 4.0;
  EXPECT_EQ(A(0, 0), 1.0);
  EXPECT_EQ(A(0, 1), 2.0);
  EXPECT_EQ(A(1, 0), 3.0);
  EXPECT_EQ(A(1, 1), 4.0);
}

TEST(TinyMatrix, GEMMSimpleCase) {
  Mat2 A, B, C;
  A = 1.0, 2.0,
      3.0, 4.0;
  B = 4.0, 5.0,
      6.0, 7.0;
  C = A * B;
  EXPECT_EQ(C(0, 0), 16.0);
  EXPECT_EQ(C(0, 1), 19.0);
  EXPECT_EQ(C(1, 0), 36.0);
  EXPECT_EQ(C(1, 1), 43.0);
}

TEST(TinyMatrix, GEMMTransposeA) {
  Mat2 A, B, C;
  A = 1.0, 2.0,
      3.0, 4.0;
  B = 4.0, 5.0,
      6.0, 7.0;
  C = transpose(A) * B;
  EXPECT_EQ(C(0, 0), 22.0);
  EXPECT_EQ(C(0, 1), 26.0);
  EXPECT_EQ(C(1, 0), 32.0);
  EXPECT_EQ(C(1, 1), 38.0);
}

TEST(TinyMatrix, GEMMTransposeB) {
  Mat34 A, B;
  Mat3 C;
  A = 0,  1,  2,  3,
      4,  5,  6,  7,
      8,  9, 10, 11;
  B = 1,  2,  3,  4,
      5,  6,  7,  8,
      9, 10, 11, 12;
  C = A * transpose(B);

  EXPECT_EQ(C(0, 0),  20);
  EXPECT_EQ(C(0, 1),  44);
  EXPECT_EQ(C(0, 2),  68);
  EXPECT_EQ(C(1, 0),  60);
  EXPECT_EQ(C(1, 1), 148);
  EXPECT_EQ(C(1, 2), 236);
  EXPECT_EQ(C(2, 0), 100);
  EXPECT_EQ(C(2, 1), 252);
  EXPECT_EQ(C(2, 2), 404);
}

}  // namespace
