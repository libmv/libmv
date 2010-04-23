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
#include "libmv/multiview/camera.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/structure.h"
#include "libmv/multiview/test_data_sets.h"
#include "testing/testing.h"

namespace libmv {

TEST(CameraModel, PinholeCameraProjection) {
  const int nviews = 8;
  const int npoints = 20;
  
  NViewDataSet d = NRealisticCameras(nviews, npoints);
 
  vector<PinholeCamera> cameras(nviews);
  
  for (size_t i = 0; i < nviews; ++i) {
    if (i < nviews/3.) {
      cameras[i].SetFocal(d.K[i](0, 0), d.K[i](1, 1));
      cameras[i].set_principal_point(Vec2(d.K[i](0, 2), d.K[i](1, 2)));
      cameras[i].set_skew_factor(d.K[i](0, 1));
      cameras[i].set_orientation_matrix(d.R[i]);
      cameras[i].set_position(d.t[i]);
    } else if (i < 2.*nviews/3.) {
      cameras[i].set_intrinsic_matrix(d.K[i]);
      cameras[i].set_orientation_matrix(d.R[i]);
      cameras[i].set_position(d.t[i]);
    } else {
      cameras[i].set_projection_matrix(d.P(i));
    }
    // Compare with ground truth.
    EXPECT_MATRIX_NEAR(d.K[i], cameras[i].intrinsic_matrix(), 1e-8);
    EXPECT_MATRIX_NEAR(d.R[i], cameras[i].orientation_matrix(), 1e-8);
    EXPECT_MATRIX_NEAR(d.t[i], cameras[i].position(), 1e-8);
  }
  
  PointStructure point_3d;
  Vec3           point_3d_coords;
  PointFeature   point_2d;
  Vec2           point_2d_gt;
  Vec2           point_2d_coords;
  for (size_t i = 0; i < nviews; ++i) {
    for (size_t j = 0; j < d.X.cols(); ++j) {
      MatrixColumn(d.x[i], j, &point_2d_gt);
      MatrixColumn(d.X, j, &point_3d_coords);
      
      point_3d.set_coords(point_3d_coords);
      cameras[i].ProjectPointStructure(point_3d, &point_2d);
      point_2d_coords << point_2d.coords.cast<double>();
      
      EXPECT_NEAR(0, DistanceLInfinity(point_2d_gt, 
                                       point_2d_coords),
                                       1e-4);
    }
  }
}

}  // namespace libmv