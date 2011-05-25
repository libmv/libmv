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

#include "testing/testing.h"
#include "libmv/numeric/numeric.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/multiview/bundle.h"

namespace {

using namespace libmv;

TEST(EuclideanBA, TwoViewsFull) {
  TwoViewDataSet d = TwoRealisticCameras(true);
  vector<Mat3> K(2);
  vector<Mat3> R(2);
  vector<Vec3> t(2);
  K[0] = d.K1;
  R[0] = d.R1;
  t[0] = d.t1;
  K[1] = d.K2;
  R[1] = d.R2;
  t[1] = d.t2;
  vector<Mat2X> x(2);
  x[0] = d.x1;
  x[1] = d.x2;

  Mat3X X;
  X = d.X;
  
  for (int i = 0; i < 2; ++i) {               // Add noise to motion.
    K[i](0, 0) *= 1 + (rand() / RAND_MAX - 0.5) * 0.1;
    K[i](1, 1) *= 1 + (rand() / RAND_MAX - 0.5) * 0.1;
    R[i] *= RotationAroundX((rand() / RAND_MAX - 0.5) * 0.1)
          * RotationAroundY((rand() / RAND_MAX - 0.5) * 0.1)
          * RotationAroundZ((rand() / RAND_MAX - 0.5) * 0.1);
    t[i] += Vec3::Random() * 0.1;
  }
  X += Mat3X::Random(3, X.cols()) * 0.1;  // and structure.
  
  for (int j = 0; j < 2; ++j) {                // Check there's enough error.
    Mat34 P;
    P_From_KRt(K[j], R[j], t[j], &P);
    Mat2X error = x[j] - Project(P, X);
    EXPECT_GT(FrobeniusNorm(error), 1);
  }
  
  EuclideanBAFull(x, &K, &R, &t, &X);                 // Run BA.

  for (int j = 0; j < 2; ++j) {                // Check there is no error. 
    Mat34 P;
    P_From_KRt(K[j], R[j], t[j], &P);
    Mat2X error = x[j] - Project(P, X);
    EXPECT_LT(FrobeniusNorm(error), 1e-5);
  }
}

TEST(EuclideanBA, NViews) {
  int nviews = 5;
  int npoints = 30;
  NViewDataSet  d = NRealisticCamerasSparse(nviews, npoints);
  vector<Mat3>  K = d.K;
  vector<Mat3>  R = d.R;
  vector<Vec3>  t = d.t;
  vector<Mat2X> x = d.x;
  const vector<Vecu>   &x_ids = d.x_ids;

  Mat3X X;
  X = d.X;
  
  for (int i = 0; i < nviews; ++i) {               // Add noise to motion.
    K[i](0, 0) *= 1 + (rand() / RAND_MAX - 0.5) * 0.1;
    K[i](1, 1) *= 1 + (rand() / RAND_MAX - 0.5) * 0.1;
    R[i] *= RotationAroundX((rand() / RAND_MAX - 0.5) * 0.1)
          * RotationAroundY((rand() / RAND_MAX - 0.5) * 0.1)
          * RotationAroundZ((rand() / RAND_MAX - 0.5) * 0.1);
    t[i] += Vec3::Random() * 0.1;
  }
  X += Mat3X::Random(3, X.cols()) * 0.1;  // and structure.
  
  Mat3X subX;
  Mat34 P;
  Mat2X error;
  for (int i = 0; i < nviews; ++i) {         // Check there's enough error.
    P_From_KRt(K[i], R[i], t[i], &P);
    error = x[i] - Project(P, X, x_ids[i]);
    EXPECT_GT(FrobeniusNorm(error), 1);
  }
  
  EuclideanBA(x, x_ids, &K, &R, &t, &X);                 // Run BA.

  for (int i = 0; i < nviews; ++i) {                // Check there is no error.
    P_From_KRt(K[i], R[i], t[i], &P);
    error = x[i] - Project(P, X, x_ids[i]);
    EXPECT_LT(FrobeniusNorm(error), 1e-3);
  }
}
}
