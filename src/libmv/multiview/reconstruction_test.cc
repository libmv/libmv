// Copyright (c) 2010 libmv authors.
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
#include <list>

#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/reconstruction.h"
#include "libmv/multiview/reconstruction_mapping.h"
#include "libmv/multiview/reconstruction_optimization.h"
#include "libmv/multiview/test_data_sets.h"
#include "libmv/numeric/numeric.h"
#include "libmv/multiview/random_sample.h"
#include "testing/testing.h"

namespace libmv {
namespace {

void GenerateMatchesFromNViewDataSet(const NViewDataSet &d,
                                     int noutliers,
                                     Matches *matches,
                                     std::list<Feature *> *list_features) {
  Matches::TrackID track_id;
  vector<int> wrong_matches;
  for (size_t n = 0; n < d.n; ++n) {
    //std::cout << "n -> "<< d.x[n]<< std::endl;
    // Generates wrong matches
    UniformSample(noutliers, d.X.cols(), &wrong_matches);
    //std::cout << "Features :"<<d.x[n].transpose()<<"\n";
    for (size_t p = 0; p < d.x[n].cols(); ++p) {
      PointFeature * feature = new PointFeature(d.x[n](0, p), d.x[n](1, p));
      list_features->push_back(feature);
      track_id = p;
      if (p < noutliers) {
        track_id = wrong_matches[p];
      }
      matches->Insert(n, track_id, feature);
    }
  }
}

TEST(CalibratedReconstruction, TestSynthetic6FullViews) {
  // TODO(julien) maybe a better check is the relative motion 
  int nviews = 6;
  int npoints = 100;
  int noutliers = 0.4*npoints;// 30% of outliers
  NViewDataSet d = NRealisticCamerasFull(nviews, npoints);
  
  Mat4X X;
  EuclideanToHomogeneous(d.X, &X);
  // These are the expected precision of the results
  // Dont expect much since for now
  //  - this is an incremental approach
  //  - the 3D structure is not retriangulated when new views are estimated
  //  - there is no optimization!
  const double kPrecisionOrientationMatrix = 3e-2;
  const double kPrecisionPosition          = 3e-2;
  
  Reconstruction reconstruction;
  // Create the matches
  Matches matches;
  Matches matches_inliers;
  std::list<Feature *> list_features;
  GenerateMatchesFromNViewDataSet(d, noutliers, &matches, &list_features);
  
  // We fix the gauge by setting the pose of the initial camera to the true pose
  PinholeCamera * camera0 = new PinholeCamera(d.K[0], d.R[0], d.t[0]);
  reconstruction.InsertCamera(0, camera0);
            
  std::cout << "Proceed Initial Motion Estimation" << std::endl;
  // Proceed Initial Motion Estimation
  ReconstructFromTwoCalibratedViews(matches, 0, 1,
                                    d.K[0], d.K[1],
                                    &matches_inliers, &reconstruction);
  PinholeCamera * camera = NULL;
  EXPECT_EQ(reconstruction.GetNumberCameras(), 2);
  camera = dynamic_cast<PinholeCamera *>(reconstruction.GetCamera(0));
  EXPECT_TRUE(camera != NULL);  
  EXPECT_MATRIX_PROP(camera->orientation_matrix(), d.R[0], 1e-8);
  EXPECT_MATRIX_PROP(camera->position(), d.t[0], 1e-8);
  
  double rms = RootMeanSquareError(d.x[0], X, camera->projection_matrix());
  std::cout << "RMSE Camera 0 = " << rms << std::endl;
  
  camera = dynamic_cast<PinholeCamera *>(reconstruction.GetCamera(1));
  EXPECT_TRUE(camera != NULL);
  
  // This is a monocular reconstruction
  // We fix the scale
  Mat3 dR = d.R[0].transpose()*d.R[1];
  Vec3 dt = d.R[0].transpose() * (d.t[1] - d.t[0]);  
  double dt_norm_real = dt.norm();
  dt = camera0->orientation_matrix().transpose() * 
    (camera->position() - camera0->position());
  dt *= dt_norm_real/dt.norm();
  camera->set_position(camera0->orientation_matrix() * dt
    + camera0->position());
  
  EXPECT_MATRIX_PROP(camera->orientation_matrix(), d.R[1],
                     kPrecisionOrientationMatrix);
  EXPECT_MATRIX_PROP(camera->position(), d.t[1], kPrecisionPosition);
  rms = RootMeanSquareError(d.x[1], X, camera->projection_matrix());
  std::cout << "RMSE Camera 1 = " << rms << std::endl;
  
  std::cout << "Proceed Initial Intersection" << std::endl;
  // Proceed Initial Intersection 
  uint nInliers_added = 0;
  size_t minimum_num_views_batch = 2;
  nInliers_added = PointStructureTriangulation(matches_inliers, 1, 
                                               minimum_num_views_batch, 
                                               &reconstruction); 
  ASSERT_NEAR(nInliers_added, npoints - noutliers, 1);
    
  size_t minimum_num_views_incremental = 3;  
  Mat3 R;
  Vec3 t;
  // Checks the incremental reconstruction
  for (int i = 2; i < nviews; ++i) {   
    std::cout << "Proceed Incremental Resection" << std::endl;
    // Proceed Incremental Resection 
    CalibratedCameraResection(matches, i, d.K[i],
                              &matches_inliers, &reconstruction);    
    
    EXPECT_EQ(reconstruction.GetNumberCameras(), i+1);
    camera = dynamic_cast<PinholeCamera *>(reconstruction.GetCamera(i));
    EXPECT_TRUE(camera != NULL);
    EXPECT_MATRIX_PROP(camera->orientation_matrix(), d.R[i], 
                       kPrecisionOrientationMatrix);
    EXPECT_MATRIX_PROP(camera->position(), d.t[i], kPrecisionPosition);
    
    std::cout << "Proceed Incremental Intersection" << std::endl;
    // Proceed Incremental Intersection 
    nInliers_added = PointStructureTriangulation(matches_inliers, i, 
                                                 minimum_num_views_incremental, 
                                                 &reconstruction);
    ASSERT_NEAR(nInliers_added, 0, 1);

    // TODO(julien) Check the rms with the reconstructed structure
    rms = RootMeanSquareError(d.x[i], X, camera->projection_matrix());
    std::cout << "RMSE Camera " << i << " = " << rms << std::endl;
    // TODO(julien) Check the 3D structure coordinates and inliers
  }
  // Performs bundle adjustment
  rms = MetricBundleAdjust(matches_inliers, &reconstruction);
  std::cout << " Final RMSE = " << rms << std::endl;
  // TODO(julien) Check the results of BA
  // clears the cameras, structures and features
  reconstruction.ClearCamerasMap();
  reconstruction.ClearStructuresMap();
  std::list<Feature *>::iterator features_iter = list_features.begin();
  for (; features_iter != list_features.end(); ++features_iter)
    delete *features_iter;
  list_features.clear();
}

TEST(UncalibratedReconstruction, TestSynthetic6FullViews) {
  int nviews = 6;
  int npoints = 100;
  int noutliers = 0.4*npoints;// 30% of outliers
  double rms;
  NViewDataSet d = NRealisticCamerasFull(nviews, npoints);
 
  Vec2u image_size;
  // TODO(julien) Clean this..add image size in NViewDataSet?
  image_size << 2*d.K[0](0, 2), 2*d.K[0](1, 2);
  Mat4X X;
  EuclideanToHomogeneous(d.X, &X);
    
  // These are the expected precision of the results
  // Dont expect much since for now
  //  - this is an incremental approach
  //  - the 3D structure is not retriangulated when new views are estimated
  //  - there is no optimization!
  const double kPrecisionOrientationMatrix = 3e-2;
  const double kPrecisionPosition          = 3e-2;
  
  Reconstruction reconstruction;
  // Create the matches
  Matches matches;
  Matches matches_inliers;
  std::list<Feature *> list_features;
  GenerateMatchesFromNViewDataSet(d, noutliers, &matches, &list_features);
              
  std::cout << "Proceed Initial Motion Estimation" << std::endl;
  // Proceed Initial Motion Estimation
  ReconstructFromTwoUncalibratedViews(matches, 0, 1,
                                      &matches_inliers, &reconstruction);
 
  // Set the image size need for the metric rectification
  // TODO(julien) find a way to do somewhere else..
  ((PinholeCamera*)reconstruction.GetCamera(0))->set_image_size(image_size);
  ((PinholeCamera*)reconstruction.GetCamera(1))->set_image_size(image_size);
    
  // TODO(julien) Check the two projection matrices    
  // Proceed Initial Intersection 
  size_t minimum_num_views_batch = 2;
  PointStructureTriangulation(matches_inliers, 1, 
                              minimum_num_views_batch, 
                              &reconstruction); 
     
  // TODO(julien) Check the rms with the reconstructed structure
  // TODO(julien) Check the 3D structure coordinates and inliers
  size_t minimum_num_views_incremental = 3;  
  Mat3 R;
  Vec3 t;
  // Checks the incremental reconstruction
  for (int i = 2; i < nviews; ++i) {   
    std::cout << "Proceed Incremental Resection" << std::endl;
    // Proceed Incremental Resection 
    UncalibratedCameraResection(matches, i, &matches_inliers, &reconstruction);
    // TODO(julien) Check the projection matrix    
    
    // Set the image size need for the metric rectification
    // TODO(julien) find a way to do somewhere else..
    ((PinholeCamera*)reconstruction.GetCamera(i))->set_image_size(image_size);
    
    std::cout << "Proceed Incremental Intersection" << std::endl;
    // Proceed Incremental Intersection 
    PointStructureTriangulation(matches_inliers, i, 
                                minimum_num_views_incremental, 
                                &reconstruction);

    // TODO(julien) Check the rms with the reconstructed structure
    // TODO(julien) Check the 3D structure coordinates and inliers
  }
  
  std::cout << "Bundle adjustment" << std::endl;
  // Performs bundle adjustment
  rms = MetricBundleAdjust(matches_inliers, &reconstruction);
  std::cout << " Final RMSE = " << rms << std::endl;
  // TODO(julien) Check the results of BA
  std::cout << "Metric rectification" << std::endl;
  // Metric rectification
  UpgradeToMetric(matches_inliers, &reconstruction);
  // TODO(julien) register the reconstruction so that the first camera
  // corresponds to the real pose
  // TODO(julien) Check the metric reconstruction
  
  // clears the cameras, structures and features
  reconstruction.ClearCamerasMap();
  reconstruction.ClearStructuresMap();
  std::list<Feature *>::iterator features_iter = list_features.begin();
  for (; features_iter != list_features.end(); ++features_iter)
    delete *features_iter;
  list_features.clear();
}
}  // namespace
}  // namespace libmv
