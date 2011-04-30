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
#include "libmv/multiview/euclidean_parameterization.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(Euclidean2DEulerParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec3 p; p << 1, 2, 3;
  Vec3 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Euclidean2DEulerParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Euclidean2DEulerParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Euclidean2DEulerParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Euclidean2DSCParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec4 p; p << 1, 2, 3, 4;
  Vec4 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Euclidean2DSCParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Euclidean2DSCParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Euclidean2DSCParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Euclidean3DEulerParameterization, Roundtripping) {
  Mat4 h, h_roundtrip;
  Vec6 p; p << M_PI/2, 1, 2, 3, 4, 5;
  Vec6 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Euclidean3DEulerParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Euclidean3DEulerParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Euclidean3DEulerParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Euclidean3DQuaternionParameterization, Roundtripping) {
  Mat4 h, h_roundtrip;
  Vec7 p; p << 0.5, 0.7, 1, 2, 3, 4, 5;
  Vec7 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Euclidean3DQuaternionParameterization<>::To(p, &h);

  // Then go from the H matrix to p
  Euclidean3DQuaternionParameterization<>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Euclidean3DQuaternionParameterization<>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

} // namespace
