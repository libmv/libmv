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
#include "libmv/multiview/affine_parameterization.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(Affine2DGenericParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec6 p; p << 1, 2, 3,
               4, 5, 6;
  Vec6 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Affine2DGenericParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Affine2DGenericParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Affine2DGenericParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Affine3DGenericParameterization, Roundtripping) {
  Mat4 h, h_roundtrip;
  Vec12 p; p << 1, 2, 3, 4,
                5, 6, 7, 8,
                9, 10, 11, 12;
  Vec12 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Affine3DGenericParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Affine3DGenericParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Affine3DGenericParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

} // namespace
