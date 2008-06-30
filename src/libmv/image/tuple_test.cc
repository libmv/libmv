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

#include "libmv/image/tuple.h"
#include "testing/testing.h"

using libmv::Tuple;

namespace {

TEST(InitConstantValue) {
  Tuple<int,3> t(5);
  for(int i=0; i<3; i++)
    Check(t(i) == 5);
}

TEST(InitFromPointer) {
  float vals[3] = {1.0f, 2.0f, 3.0f};

  Tuple<float,3> t(vals);
  for(int i=0; i<3; i++)
    Check(t(i) == vals[i]);

  Tuple<int,3> b(t);
  for(int i=0; i<3; i++)
    Check(b(i) == int(vals[i]));
}

TEST(Swap) {
  unsigned char vala[3] = {1,2,3};
  unsigned char valb[3] = {4,5,6};

  Tuple<unsigned char,3> a(vala);
  Tuple<unsigned char,3> b(valb);

  a.Swap(b);

  for(int i=0; i<3; i++)
    Check(a[i] == int(valb[i]));
  for(int i=0; i<3; i++)
    Check(b(i) == int(vala[i]));
}


}  // namespace
