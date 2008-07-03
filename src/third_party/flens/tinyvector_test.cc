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
typedef TinyMatrix<double, 3, 4> Mat34;

namespace {

// TinyMatrix / TinyVector didn't obey the matrix / vector protocol set out by
// the rest of FLENS. These tests cover the added functionality necessary to
// make the tiny vectors and matrices compliant.
TEST(DimensionsMatchNormalProtocol) {
  Vec4 x;
  Equals(x.length(), 4);
  TinyMatrix<double, 3,4> A;
  Equals(A.numRows(), 3);
  Equals(A.numCols(), 4);
}

TEST(ResizeMatchesDenseAPI) {
  Vec4 x;
  x.resize(4);  // Does nothing, but should compile.
}

TEST(FirstRowAndColumnIsZero) {
  Vec4 x;
  Equals(x.firstIndex(), 0);
  Mat34 A;
  Equals(A.firstRow(), 0);
  Equals(A.firstCol(), 0);
}

// Copying between views and tiny matrices.

TEST(CopyFromDenseVectorToTinyVector) {
  Vec x(4);
  x = 1.0, 2.0, 3.0, 4.0;
  Vec4 y;
  y = x;
  Equals(y(0), 1.0);
  Equals(y(1), 2.0);
  Equals(y(2), 3.0);
  Equals(y(3), 4.0);
}

// For some reason I had to add an initialization to generalmatrix.tcc before 
// the following code would compile. This remains to be sure it compiles!
TEST(MatrixViewsCompileWithoutWarning) {
  Mat B(3,3);
  Mat::View vv = B(_(1,3), _(1,3));
  Equals(vv.numRows(), 3);
}

TEST(CopyFromTinyMatrixToDenseMatrixView) {
  Mat2 A;
  A(0, 0) = 1.0;
  A(0, 1) = 2.0;
  A(1, 0) = 3.0;
  A(1, 1) = 4.0;

  // Confusingly, matrices are 1-indexed by default, and use inclusive slices.
  Mat B(3,3);
  B = 0;
  B(_(1,2),_(1,2)) = A;
  Equals(B(1, 1), 1.0);
  Equals(B(1, 2), 2.0);
  Equals(B(1, 3), 0.0);
  Equals(B(2, 1), 3.0);
  Equals(B(2, 2), 4.0);
  Equals(B(2, 3), 0.0);
  Equals(B(3, 1), 0.0);
  Equals(B(3, 2), 0.0);
  Equals(B(3, 3), 0.0);
}

TEST(CopyFromDenseMatrixViewToTinyMatrix) {
  Mat B(3,3);
  B = 0;
  B(1, 1) = 1.0;
  B(1, 2) = 2.0;
  B(2, 1) = 3.0;
  B(2, 2) = 4.0;
  Mat2 A;
  A = B(_(1,2),_(1,2));
  Equals(A(0, 0), 1.0);
  Equals(A(0, 1), 2.0);
  Equals(A(1, 0), 3.0);
  Equals(A(1, 1), 4.0);
}

TEST(TinyVectorScalarInitialization) {
  Vec2 x;
  x = 4.0;
  Equals(x(0), 4.0);
  Equals(x(1), 4.0);
}

TEST(TinyMatrixScalarInitialization) {
  Mat2 A;
  A = 4.0;
  Equals(A(0, 0), 4.0);
  Equals(A(0, 1), 4.0);
  Equals(A(1, 0), 4.0);
  Equals(A(1, 1), 4.0);
}

TEST(TinyVectorListInitialization) {
  Vec4 x;
  x = 1.0, 2.0, 3.0;
  Equals(x(0), 1.0);
  Equals(x(1), 2.0);
  Equals(x(2), 3.0);
}

TEST(TinyMatrixListInitialization) {
  Mat2 A;
  A = 1.0, 2.0,
      3.0, 4.0;
  Equals(A(0, 0), 1.0);
  Equals(A(0, 1), 2.0);
  Equals(A(1, 0), 3.0);
  Equals(A(1, 1), 4.0);
}

}  // namespace
