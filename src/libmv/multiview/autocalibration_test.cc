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

#include <iostream>

#include "libmv/multiview/autocalibration.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(Projection, K_From_AbsoluteConic) {
  Mat3 K, Kp;
  K << 10,  1, 30,
        0, 20, 40,
        0,  0,  1;

  Mat3 w = (K * K.transpose()).inverse();
  K_From_AbsoluteConic(w, &Kp);

  EXPECT_MATRIX_NEAR(K, Kp, 1e-8);
}

// Tests that K computed from the IAC has positive elements in its diagonal.
TEST(Projection, K_From_AbsoluteConic_SignedDiagonal) {
  Mat3 K, Kpositive, Kp;
  K << 10,   1, 30,
        0, -20, 40,
        0,   0,  -1;
  Kpositive << 10, -1, -30,  // K with column signs changed so that the 
                0, 20, -40,  // diagonal is positive.
                0,  0,   1;

  Mat3 w = (K * K.transpose()).inverse();
  K_From_AbsoluteConic(w, &Kp);

  EXPECT_MATRIX_NEAR(Kpositive, Kp, 1e-8);
}

} // namespace
