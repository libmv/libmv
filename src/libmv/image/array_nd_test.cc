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

#include "libmv/image/array_nd.h"
#include "testing/testing.h"

using libmv::ArrayND;

namespace {

TEST(ArrayND, EmptyConstructor) {
  ArrayND<int,2> a;

  EXPECT_EQ(0, a.Shape(0));
  EXPECT_EQ(0, a.Shape(1));
}

TEST(ArrayND, IndexConstructor) {
  int s[] = {1, 2, 3};
  ArrayND<int,3>::Index shape(s);
  ArrayND<int,3> a(shape);

  EXPECT_EQ(1, a.Shape(0));
  EXPECT_EQ(2, a.Shape(1));
  EXPECT_EQ(3, a.Shape(2));
}

TEST(ArrayND, PointerConstructor) {
  int s[] = {1, 2, 3};
  ArrayND<int,3> a(s);

  EXPECT_EQ(1, a.Shape(0));
  EXPECT_EQ(2, a.Shape(1));
  EXPECT_EQ(3, a.Shape(2));
}

TEST(ArrayND, View) {
  int s[] = {3};
  ArrayND<int,1> a(s);
  ArrayND<int,1> b = a.View();

  EXPECT_EQ(a.Shape(0), b.Shape(0));
  EXPECT_EQ(a.Stride(0), b.Stride(0));
  EXPECT_EQ(a.Data(), b.Data());
  EXPECT_FALSE(b.OwnData());
}

TEST(ArrayND, Size) {
  int s[] = {1, 2, 3};
  ArrayND<int,3>::Index shape(s);
  ArrayND<int,3> a(shape);

  int l[] = {0, 1, 2};
  ArrayND<int,3>::Index last(l);

  EXPECT_EQ(a.Size(), a.Offset(last)+1);
  EXPECT_TRUE(a.Contains(last));
  EXPECT_FALSE(a.Contains(shape));
}

TEST(ArrayND, Parenthesis) {
  typedef ArrayND<int, 2>::Index Index;

  int s[] = {3, 3};
  ArrayND<int,2> a(s);

  *(a.Data()+0) = 0;
  *(a.Data()+5) = 5;

  int i1[] = {0, 0};
  EXPECT_EQ(0, a(Index(i1)));
  int i2[] = {1, 2};
  EXPECT_EQ(5, a(Index(i2)));
}

TEST(ArrayND, 1DConstructor) {
  ArrayND<int,1> a(3);

  EXPECT_EQ(3, a.Shape(0));
}

TEST(ArrayND, 2DConstructor) {
  ArrayND<int,2> a(1,2);

  EXPECT_EQ(1, a.Shape(0));
  EXPECT_EQ(2, a.Shape(1));
}

TEST(ArrayND, 3DConstructor) {
  ArrayND<int,3> a(1,2,3);

  EXPECT_EQ(1, a.Shape(0));
  EXPECT_EQ(2, a.Shape(1));
  EXPECT_EQ(3, a.Shape(2));
}

TEST(ArrayND, 1DAccessor) {
  ArrayND<int,1> a(3);
  a(0) = 1;
  a(1) = 2;

  EXPECT_EQ(1, a(0));
  EXPECT_EQ(2, a(1));
  EXPECT_EQ(1, *(a.Data()));
  EXPECT_EQ(2, *(a.Data() + a.Stride(0)));
}

TEST(ArrayND, 2DAccessor) {
  ArrayND<int,2> a(3,3);
  a(0,0) = 1;
  a(1,1) = 2;

  EXPECT_EQ(1, a(0,0));
  EXPECT_EQ(2, a(1,1));
  EXPECT_EQ(1, *(a.Data()));
  EXPECT_EQ(2, *(a.Data() + a.Stride(0) + a.Stride(1)));
}

TEST(ArrayND, 3DAccessor) {
  ArrayND<int,3> a(3,3,3);
  a(0,0,0) = 1;
  a(1,1,1) = 2;

  EXPECT_EQ(1, a(0,0,0));
  EXPECT_EQ(2, a(1,1,1));
  EXPECT_EQ(1, *(a.Data()));
  EXPECT_EQ(2, *(a.Data() + a.Stride(0) + a.Stride(1) + a.Stride(2)));
}

TEST(ArrayND, CopyFrom) {
  ArrayND<int,3> a(2,2,1);
  a(0,0, 0) = 1;
  a(0,1, 0) = 2;
  a(1,0, 0) = 3;
  a(1,1, 0) = 4;
  ArrayND<float,3> b;
  b.CopyFrom(a);
  EXPECT_FLOAT_EQ(1.f, b(0,0, 0));
  EXPECT_FLOAT_EQ(2.f, b(0,1, 0));
  EXPECT_FLOAT_EQ(3.f, b(1,0, 0));
  EXPECT_FLOAT_EQ(4.f, b(1,1, 0));
}

}  // namespace
