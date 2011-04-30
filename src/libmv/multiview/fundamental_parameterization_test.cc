// Copyright (c) 2009 libmv authors.
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
#include "libmv/multiview/fundamental_parameterization.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

TEST(FundamentalParameterization, Roundtripping) {
  Mat3 f, f_roundtrip;
  Vec9 p; p << 1, 2, 3, 4,  // u
               0.,          // s
               5, 6, 7, 1;  // vt
  p.head<4>().normalize();
  p.tail<4>().normalize();
  Vec9 p_roundtrip;

  // Use the parameterization to get some arbitrary F matrix which satisfies
  // the rank 2 constraint. The initial p is ignored.
  FundamentalRank2Parameterization<double>::To(p, &f);

  // Then go from the F matrix to p
  FundamentalRank2Parameterization<double>::From(f, &p_roundtrip);

  // If this were a minimizer, then the minimizer would run over p.

  // Now convert back to F from p 
  FundamentalRank2Parameterization<double>::To(p_roundtrip, &f_roundtrip);

  LG << "p " << p.transpose();
  LG << "f\n" << f;
  LG << "p_roundtrip " << p_roundtrip.transpose();
  LG << "f_roundtrip\n" << f_roundtrip;

  // Check that going from F to p and back to F goes in a circle.
  EXPECT_MATRIX_PROP(f, f_roundtrip, 1.5e-8);
}

} // namespace
