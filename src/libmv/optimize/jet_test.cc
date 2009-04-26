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

#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"
#include "libmv/optimize/jet.h"
#include "testing/testing.h"

using namespace libmv;

TEST(JetTest, SimpleJet) {
  Jet<2> x(4.5);
  EXPECT_EQ(4.5, x.x);
  EXPECT_EQ(0.0, x.d[0]);
  EXPECT_EQ(0.0, x.d[1]);
}

TEST(JetTest, ProductRule) {
  Jet<1> x(6, 0);
  Jet<1> fx = x*x;  // f(x) = x^2, df/dx = 2x

  EXPECT_EQ(36, fx.x);
  EXPECT_EQ(12, fx.d[0]);
}

TEST(JetTest, ProductRulePartials) {
  Jet<3> x(6, 0);
  Jet<3> y(3, 1);
  Jet<3> z(5, 2);
  Jet<3> f = x*x*y*z;

  EXPECT_EQ(540, f.x);
  EXPECT_EQ(180, f.d[0]);
  EXPECT_EQ(180, f.d[1]);
  EXPECT_EQ(108, f.d[2]);
}

TEST(JetTest, ProductRuleSecondDerivative) {
  Jet<1, Jet<1> > x(3);

  // There is a small amount of waste, in that the first derivative gets
  // calculated twice. It may be possible for a sufficiently clever person
  // to fix the jet code so that the real and duals are split types, and avoid
  // calculating the first derivative entirely, when the item of interest is
  // the hessian.
  x.x.d[0] = 1.0;
  x.d[0].x = 1.0;

  Jet<1, Jet<1> > fx = x*x;

  EXPECT_EQ(9, fx.x.x);
  EXPECT_EQ(6, fx.x.d[0]);     // First derivative.
  EXPECT_EQ(6, fx.d[0].x);     // First derivative (again).
  EXPECT_EQ(2, fx.d[0].d[0]);  // Second derivative.
}

// TODO(keir): It's possible to only compute the upper triangular part of the
// hessian, but it involves making a specialized "hessian jet" type that does
// both first and second derivatives at the same time.
TEST(JetTest, ProductRuleHessian) {
  Jet<3, Jet<3> > x(3); x.x.d[0] = 1.0; x.d[0].x = 1.0;
  Jet<3, Jet<3> > y(2); y.x.d[1] = 1.0; y.d[1].x = 1.0;
  Jet<3, Jet<3> > z(4); z.x.d[2] = 1.0; z.d[2].x = 1.0;
  Jet<3, Jet<3> > constant(100);

  Jet<3, Jet<3> > f = x*x + y*z + constant;

  EXPECT_EQ(117, f.x.x);      // f(3, 2, 4)

  EXPECT_EQ(6, f.x.d[0]);     // Gradient.
  EXPECT_EQ(4, f.x.d[1]);
  EXPECT_EQ(2, f.x.d[2]);

  EXPECT_EQ(6, f.d[0].x);     // Gradient (again).
  EXPECT_EQ(4, f.d[1].x);
  EXPECT_EQ(2, f.d[2].x);

  EXPECT_EQ(2, f.d[0].d[0]);  // Hessian.
  EXPECT_EQ(0, f.d[0].d[1]);  // All zero except for dxx, dyz, and dzy
  EXPECT_EQ(0, f.d[0].d[2]);
  EXPECT_EQ(0, f.d[1].d[0]);
  EXPECT_EQ(0, f.d[1].d[1]);
  EXPECT_EQ(1, f.d[1].d[2]);
  EXPECT_EQ(0, f.d[2].d[0]);
  EXPECT_EQ(1, f.d[2].d[1]);
  EXPECT_EQ(0, f.d[2].d[2]);
}

TEST(JetTest, QuotentRulePartials) {
  Jet<3> x(1, 0);
  Jet<3> y(3, 1);
  Jet<3> z(5, 2);
  Jet<3> f = (x + Jet<3>(2)) / (y + z*z + Jet<3>(4));

  EXPECT_EQ(3./32.,           f.x);
  EXPECT_EQ(1./32.,           f.d[0]);
  EXPECT_EQ(-3./32./32.,      f.d[1]);
  EXPECT_EQ(-3./32./32.*2*5,  f.d[2]);
}
