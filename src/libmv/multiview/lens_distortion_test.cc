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

#include "libmv/base/vector.h"
#include "libmv/numeric/levenberg_marquardt.h"
#include "libmv/multiview/camera.h"
#include "libmv/multiview/lens_distortion.h"
#include "libmv/multiview/structure.h"
#include "libmv/multiview/test_data_sets.h"
#include "testing/testing.h"

namespace libmv {

TEST(LensDistortion, LensDistortionDistortUndistor) {
  const size_t nviews = 5;
  const size_t npoints = 20;

  NViewDataSet d = NRealisticCamerasFull(nviews, npoints);

  Vec2u size_image(d.K[0](0,2)*2., d.K[0](1,2)*2.);
  Vec radial_k,tangential_p;

  Mat2X x_distorted_undistorted[nviews];
  Vec2 x;

  //generate some distortion parameters
  vector<Vec> realistic_radial_k;
  vector<Vec> realistic_tangential_p;

  realistic_radial_k.push_back(Vec4(0.441142,-0.299391,0.000664,-0.000428));
  realistic_tangential_p.push_back(Vec());
  realistic_radial_k.push_back(Vec2(0.44,-0.299));
  realistic_tangential_p.push_back(Vec2(0.01,0.01));
  realistic_radial_k.push_back(Vec2(0.24,-0.199));
  realistic_tangential_p.push_back(Vec3(0.11,0.01,0.001));
  // TODO(julien) use some more distinct  realistic distortion data
  // TODO(pmoulon) I suggest simulate a K1 level disto, a K1-2-3, and K+Tang.

  LensDistortion p;
  vector<PinholeCameraDistortion> cameras(nviews, &p);
  vector<LensDistortion> lens_distortion(nviews, p);

  for (size_t i = 0; i < nviews; ++i) {
    cameras[i].set_projection_matrix(d.P(i));
    cameras[i].set_image_size(size_image);

    x_distorted_undistorted[i].resize(2, npoints);

    radial_k = realistic_radial_k[i%realistic_radial_k.size()];
    tangential_p = realistic_tangential_p[i%realistic_tangential_p.size()];

    lens_distortion[i].set_radial_distortion(radial_k);
    lens_distortion[i].set_tangential_distortion(tangential_p);

    cameras[i].set_lens_distortion(&lens_distortion[i]);

    // we distort the 2D points projections
    for (size_t j = 0; j < npoints; ++j) {
      x = d.x[i].col(j);

      cameras[i].ComputeDistortedCoordinates(x, &x);
      cameras[i].ComputeUndistortedCoordinates(x, &x);

      x_distorted_undistorted[i](0, j) = x(0);
      x_distorted_undistorted[i](1, j) = x(1);
    }

    // Compare with ground truth.
    EXPECT_MATRIX_NEAR(d.x[i], x_distorted_undistorted[i], 1e-8);
  }
}

}  // namespace libmv
