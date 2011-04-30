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
#include "libmv/multiview/rotation_parameterization.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(Rotation2DEulerParameterization, Roundtripping) {
  Mat2 h, h_roundtrip;
  double p =  0.6;
  double p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Rotation2DEulerParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Rotation2DEulerParameterization<double>::From(h, &p_roundtrip);
  
  // Now convert back to H from p 
  Rotation2DEulerParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Rotation2DSCParameterization, Roundtripping) {
  Mat2 h, h_roundtrip;
  Vec2 p; p << 1, 2;
  Vec2 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Rotation2DSCParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Rotation2DSCParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Rotation2DSCParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Rotation3DEulerParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec3 p; p << 1, M_PI/2, 3;
  Vec3 p_roundtrip;
  
  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Rotation3DEulerParameterization<double>::To(p, &h);
  
  // Then go from the H matrix to p
  Rotation3DEulerParameterization<double>::From(h, &p_roundtrip);
  
  // Now convert back to H from p 
  Rotation3DEulerParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Rotation3DQuaternionParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec4 p; p << 0.5, 1, 2, 3;
  p /= p.norm();
  Vec4 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Rotation3DQuaternionParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Rotation3DQuaternionParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Rotation3DQuaternionParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Rotation3DAngleAxisParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec4 p; p << 0.5, 1, 2, 3;
  p.segment<3>(1) /= p.segment<3>(1).norm();
  Vec4 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Rotation3DAngleAxisParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Rotation3DAngleAxisParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Rotation3DAngleAxisParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Rotation3DExponentialMapParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec3 p; p << 1, 2, 3;
  Vec3 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Rotation3DExponentialMapParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Rotation3DExponentialMapParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Rotation3DExponentialMapParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}
} // namespace
