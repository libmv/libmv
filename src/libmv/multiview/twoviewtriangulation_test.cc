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
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "libmv/multiview/twoviewtriangulation.h"
#include "testing/testing.h"

namespace {
using namespace libmv;

// Because the TwoViewTriangulationByPlanes works by minimizing the
// error in the first image it is enough to test the function by moving
// x1 in a perperdicular diraction away from the epipolar line.
TEST(Triangulation, TriangulateByPlanes) {
  TwoViewDataSet d = TwoRealisticCameras();

  // Compute essential matrix.
  Mat3 E;
  EssentialFromFundamental(d.F, d.K1, d.K2, &E);
  Mat3 K1_inverse = d.K1.inverse();
  Mat3 K2_inverse = d.K2.inverse();
  //Transform the system so that camera 1 is in its caninical form [I|0]
  Eigen::Transform< double, 3 > Hcanonical = Eigen::Translation3d(d.t1)*d.R1;
  Hcanonical = Hcanonical.inverse();

  Mat34 P2;
  P2.block<3,3>(0,0) = d.R2;
  P2.block<3,1>(0,3) = d.t2;

  P2 = P2*Hcanonical;

  srand ( time(NULL) );

  for (int i = 0; i < d.X.cols(); ++i) {

    Vec2 x1, x2;
    MatrixColumn(d.x1, i, &x1);
    MatrixColumn(d.x2, i, &x2);
    x1 = ImageToNormImageCoordinates(K1_inverse,x1);
    x2 = ImageToNormImageCoordinates(K2_inverse,x2);

    // Moving x1 along a line perpendicular to the epipolar line epl1.
    Vec3 x1_homogenious = EuclideanToHomogeneous(x1);
    Vec3 x2_homogenious = EuclideanToHomogeneous(x2);
    Vec3 epl1 = E.transpose()*x2_homogenious;
    epl1[2] = 0;
    Vec3 l_x1 = x1_homogenious.cross(epl1);
    Vec2 r_l; //a vectror along the l_x1 line
    r_l[0] =  -l_x1[1];
    r_l[1] =  l_x1[0];
    r_l.normalize();
    double scale = 10*rand()/(double)RAND_MAX;
    Vec2 x1_offset = x1 + scale*r_l;

    Vec3 X_estimated, X_reference;
    MatrixColumn(d.X, i, &X_reference);
    TwoViewTriangulationByPlanes(x1_offset, x2, P2, E, &X_estimated);
    X_estimated = Hcanonical*X_estimated;
    EXPECT_NEAR(0, DistanceLInfinity(X_estimated, X_reference), 1e-6);
  }
}

TEST(Triangulation, TwoViewTriangulationIdeal) {
  TwoViewDataSet d = TwoRealisticCameras();

  // Compute essential matrix.
  Mat3 E;
  EssentialFromFundamental(d.F, d.K1, d.K2, &E);
  Mat3 K1_inverse = d.K1.inverse();
  Mat3 K2_inverse = d.K2.inverse();

  //Transform the system so that camera 1 is in its caninical form [I|0]
  Eigen::Transform< double, 3 > Hcanonical = Eigen::Translation3d(d.t1)*d.R1;
  Hcanonical = Hcanonical.inverse();

  Mat34 P2;
  P2.block<3,3>(0,0) = d.R2;
  P2.block<3,1>(0,3) = d.t2;

  P2 = P2*Hcanonical;

  for (int i = 0; i < d.X.cols(); ++i) {

    Vec2 x1, x2;
    MatrixColumn(d.x1, i, &x1);
    MatrixColumn(d.x2, i, &x2);
    x1 = ImageToNormImageCoordinates(K1_inverse,x1);
    x2 = ImageToNormImageCoordinates(K2_inverse,x2);

    Vec3 X_estimated, X_gt;
    MatrixColumn(d.X, i, &X_gt);
    TwoViewTriangulationIdeal(x1, x2, P2, E, &X_estimated);
    X_estimated = Hcanonical*X_estimated;
    EXPECT_NEAR(0, DistanceLInfinity(X_estimated, X_gt), 1e-8);
  }
}


} // namespace

