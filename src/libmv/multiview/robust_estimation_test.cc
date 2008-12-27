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

#include "libmv/multiview/robust_estimation.h"
#include "libmv/numeric/numeric.h"
#include "testing/testing.h"

namespace {

using namespace libmv;

// Robust estimation issues.
// 
//  Inlier / outlier classification. Soft? Online? Where does the threshold
//  code from?
//
//  Computing the error; fractional? early bailout? one sample per or many?
//
//  idea: make the fitting functions all take samples as column vectors in a
//  matrix, and return a vector of results.
//

struct LineModel {
  // Perhaps "Error" doesn't belong in the model. Squared error for now.
  double Error(const Vec2 &xy) {
    double x = xy(0);
    double y = xy(1);
    double line_y = a*x + b;
    double e = y - line_y;
    return e*e;
  }
  double a, b;
};

struct LineFitter {
  void Fit(Mat2X &samples, std::vector<LineModel> *models) {
    // Standard least squares solution.
    MatX2 X(samples.cols(), 2);
    X.col(0).setOnes();
    X.col(1) = samples.row(0).transpose();
    Mat A(X.transpose() * X);
    Vec b(X.transpose() * samples.row(1).transpose());
    Vec2 ba;
    A.svd().solve(b, &ba);
    LineModel line = {ba(1), ba(0)};
    models->push_back(line);
  }
  int MinimumSamples() {
    return 2;
  }
};

// Since the line fitter isn't so simple, test it in isolation.
TEST(LineFitter, ItWorks) {
  Mat2X xy(2, 5);
  // y = 2x + 1
  xy << 1, 2, 3, 4,  5,
        3, 5, 7, 9, 11;
  std::vector<LineModel> models;
  LineFitter fitter;
  fitter.Fit(xy, &models);
  ASSERT_EQ(1, models.size());
  EXPECT_NEAR(2.0, models[0].a, 1e-9);
  EXPECT_NEAR(1.0, models[0].b, 1e-9);
}

TEST(RobustLineFitter, OutlierFree) {
  Mat2X xy(2, 5);
  // y = 2x + 1
  xy << 1, 2, 3, 4,  5,
        3, 5, 7, 9, 11;

  LineModel model = 
    Estimate<LineModel>(xy, LineFitter(), ThresholdClassifier(4), MLECost(4));
  EXPECT_NEAR(2.0, model.a, 1e-9);
  EXPECT_NEAR(1.0, model.b, 1e-9);
}

TEST(RobustLineFitter, OneOutlier) {
  Mat2X xy(2, 6);
  // y = 2x + 1 with an outlier
  xy << 1, 2, 3, 4,  5, /* outlier! */  100,
        3, 5, 7, 9, 11, /* outlier! */ -123;

  LineModel model = 
    Estimate<LineModel>(xy, LineFitter(), ThresholdClassifier(4), MLECost(4));
  EXPECT_NEAR(2.0, model.a, 1e-9);
  EXPECT_NEAR(1.0, model.b, 1e-9);
}

}  // namespace
