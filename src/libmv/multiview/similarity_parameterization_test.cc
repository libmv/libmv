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
#include "libmv/multiview/similarity_parameterization.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(Similarity2DSCParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec4 p; p << 1, 2, 3, 4;
  Vec4 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Similarity2DSCParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Similarity2DSCParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Similarity2DSCParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

TEST(Similarity2DSAParameterization, Roundtripping) {
  Mat3 h, h_roundtrip;
  Vec4 p; p << 1, 2, 3, 4;
  Vec4 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Similarity2DSAParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Similarity2DSAParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Similarity2DSAParameterization<double>::To(p_roundtrip, &h_roundtrip);

  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 2.5e-8);
}

TEST(Similarity3DEulerParameterization, Roundtripping) {
  Mat4 h, h_roundtrip;
  Vec7 p; p << 5, 2, 3, 4, 5, 6, 7;  
  Vec7 p_roundtrip;

  // Use the parameterization to get some arbitrary H matrix.
  // The initial p is ignored.
  Similarity3DEulerParameterization<double>::To(p, &h);

  // Then go from the H matrix to p
  Similarity3DEulerParameterization<double>::From(h, &p_roundtrip);

  // Now convert back to H from p 
  Similarity3DEulerParameterization<double>::To(p_roundtrip, &h_roundtrip);


  // Check that going from H to p and back to H goes in a circle.
  EXPECT_MATRIX_PROP(h, h_roundtrip, 1.5e-8);
}

} // namespace
