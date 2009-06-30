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

#include <cmath>

#include "libmv/numeric/numeric.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/test_data_sets.h"

namespace libmv {

TwoViewDataSet TwoRealisticCameras(bool same_K) {
  TwoViewDataSet d;

  d.K1 << 320,   0, 160,
            0, 320, 120,
            0,   0,   1;
  if (same_K) {
    d.K2 = d.K1;
  } else {
    d.K2 << 360,   0, 170,
              0, 360, 110,
              0,   0,   1;
  }
  d.R1 = RotationAroundZ(-0.1);
  d.R2 = RotationAroundX(-0.1);
  d.t1 << 1, 1, 10;
  d.t2 << -2, -1, 10;
  P_From_KRt(d.K1, d.R1, d.t1, &d.P1);
  P_From_KRt(d.K2, d.R2, d.t2, &d.P2);

  FundamentalFromProjections(d.P1, d.P2, &d.F);
  
  d.X.resize(3, 30);
  d.X.setRandom();

  Project(d.P1, d.X, &d.x1);
  Project(d.P2, d.X, &d.x2);
  
  return d;
}

// TODO(keir): Make this more configurable.
const int fx     =  1000;
const int fy     =  1000;
const int cx     =   500;
const int cy     =   500;
const double dist   =  1.5;
const double jitter_amount = 0.01;

NViewDataSet NRealisticCameras(int nviews, int npoints) {
  NViewDataSet d;
  d.n = nviews;

  d.X.resize(3, npoints);
  d.X.setRandom();
  d.X *= 0.6;

  for (int i = 0; i < nviews; ++i) {
    Vec3 camera_center, t, jitter, lookdir;

    double theta = i * 2 * M_PI / nviews;
    camera_center << sin(theta), 0.0, cos(theta);
    camera_center *= dist;
    d.C[i] = camera_center;

    jitter.setRandom();
    jitter *= jitter_amount / camera_center.norm();
    lookdir = -camera_center + jitter;

    d.K[i] << fx,  0, cx,
               0, fy, cy,
               0,  0,  1;
    d.R[i] = LookAt(lookdir);
    d.t[i] = -d.R[i] * camera_center;
    d.x[i] = Project(d.P(i), d.X);
  }
  return d;
}

}  // namespace libmv
