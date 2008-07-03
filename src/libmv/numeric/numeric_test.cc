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

using libmv::Mat;
using libmv::Mat34;
using libmv::Vec;
using libmv::Nullspace;

namespace {

TEST(Nullspace) {
  Mat A(3, 4);
  A = 0.76026643, 0.01799744, 0.55192142, 0.8699745 ,
      0.42016166, 0.97863392, 0.33711682, 0.14479271,
      0.51016811, 0.66528302, 0.54395496, 0.57794893;
  Vec x;
  double s = Nullspace(&A, &x);
  Close(s, 0.122287);
  Close(x(1), -0.05473917);
  Close(x(2),  0.21822937);
  Close(x(3), -0.80258116);
  Close(x(4),  0.55248805);
}

TEST(TinyMatrixNullspace) {
  Mat34 A;
  A = 0.76026643, 0.01799744, 0.55192142, 0.8699745 ,
      0.42016166, 0.97863392, 0.33711682, 0.14479271,
      0.51016811, 0.66528302, 0.54395496, 0.57794893;
  Vec x;
  double s = Nullspace(&A, &x);
  Close(s, 0.122287);
  Close(x(1), -0.05473917);
  Close(x(2),  0.21822937);
  Close(x(3), -0.80258116);
  Close(x(4),  0.55248805);
}

}  // namespace
