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

#include "libmv/base/vector.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/sixpointnview.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

TEST(SixPointNView, ThreeView) {
  int nviews = 3;
  int npoints = 6;
  NViewDataSet d = NRealisticCamerasFull(nviews, npoints);

  Mat2X xs = HStack(HStack(d.x[0], d.x[1]), d.x[2]);
  vector<SixPointReconstruction> reconstructions;
  SixPointNView(xs, &reconstructions);

  LOG(INFO) << "Got " << reconstructions.size() << " reconstructions.";
  for (int i = 0; i < reconstructions.size(); ++i) {
    Mat4X X = reconstructions[i].X;
    for (int j = 0; j < 3; ++j) {
      Mat34 &P = reconstructions[i].P[j];
      Mat2X x = Project(P, X);
      Mat2X error = x - d.x[j];
      LOG(INFO) << "Camera: " << j << "has errors: \n"
                << "error: \n" << error.colwise().norm();
      // Note that this error bound could probably be made smaller, but it
      // would require preconditioning the points to make them distributed
      // around 0 with magnitude sqrt(2), rather than the test data
      // distribution which has points from roughly 0 to 1000.
      EXPECT_MATRIX_NEAR_ZERO(error, 1e-9);
    }
  }
}

} // namespace
