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

TEST(ArrayNDEmptyConstructor) {
  ArrayND<int,2> a;

  Check(a.Shape(0) == 0);
  Check(a.Shape(1) == 0);
}

TEST(ArrayNDIndexConstructor) {
  int s[] = {1, 2, 3};
  ArrayND<int,3>::Index shape(s);
  ArrayND<int,3> a(shape);

  Check(a.Shape(0) == 1);
  Check(a.Shape(1) == 2);
  Check(a.Shape(2) == 3);
}

TEST(ArrayNDPointerConstructor) {
  int s[] = {1, 2, 3};
  ArrayND<int,3> a(s);

  Check(a.Shape(0) == 1);
  Check(a.Shape(1) == 2);
  Check(a.Shape(2) == 3);
}

TEST(ArrayNDView) {
  int s[] = {3};
  ArrayND<int,1> a(s);
  ArrayND<int,1> b = a.View();

  Check(a.Shape(0) == b.Shape(0));
  Check(a.Stride(0) == b.Stride(0));
  Check(a.Data() == b.Data());
  Check(!b.OwnData());
}

TEST(ArrayNDSize) {
  int s[] = {1, 2, 3};
  ArrayND<int,3>::Index shape(s);
  ArrayND<int,3> a(shape);

  int l[] = {0, 1, 2};
  ArrayND<int,3>::Index last(l);

  Check(a.Size() == a.Offset(last)+1);
  Check(a.Contains(last));
  Check(!a.Contains(shape));
}

TEST(ArrayNDParenthesis) {
  typedef ArrayND<int, 2>::Index Index;

  int s[] = {3, 3};
  ArrayND<int,2> a(s);

  *(a.Data()+0) = 0;
  *(a.Data()+5) = 5;

  int i1[] = {0, 0};
  Check(a(Index(i1)) == 0);
  int i2[] = {1, 2};
  Check(a(Index(i2)) == 5);
}

}  // namespace
