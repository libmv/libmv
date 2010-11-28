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

#include "libmv/base/vector_utils.h"
#include "libmv/camera/pinhole_camera.h"
#include "libmv/multiview/five_point.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/robust_euclidean_resection.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/reconstruction/calibrated_reconstruction.h"
#include "libmv/reconstruction/mapping.h"
#include "libmv/reconstruction/tools.h"

namespace libmv {

bool ReconstructFromTwoCalibratedViews(const Matches &matches, 
                                       CameraID image_id1, 
                                       CameraID image_id2, 
                                       const Mat3 &K1, 
                                       const Mat3 &K2, 
                                       Matches *matches_inliers,
                                       Reconstruction *reconstruction) {
  double epipolar_threshold = 1;// in pixels
  vector<Mat> xs(2);
  vector<Matches::TrackID> tracks;
  vector<Matches::ImageID> images;
  images.push_back(image_id1);
  images.push_back(image_id2);
  PointMatchMatrices(matches, images, &tracks, &xs);
  // TODO(julien) Also remove structures that are on the same location
  if (xs[0].cols() < 7) {
    LOG(ERROR) << "Error: there are not enough common matches ("
               << xs[0].cols()<< "<7).";
    return false;
  }
  
  Mat &x0 = xs[0];
  Mat &x1 = xs[1];
  vector<int> feature_inliers;
  Mat3 F;
  // Computes fundamental matrix
  // TODO(julien) For the calibrated case, we can squeeze the fundamental using
  // directly the 5 points algorithm
  FundamentalFromCorrespondences7PointRobust(x0,x1,
                                             epipolar_threshold,
                                             &F, &feature_inliers,
                                             1e-3);
  // Only inliers are selected in order to estimation the relative motion
  Mat2X v0(2, feature_inliers.size());
  Mat2X v1(2, feature_inliers.size());
  size_t index_inlier = 0;
  for (size_t c = 0; c < feature_inliers.size(); ++c) {
    index_inlier = feature_inliers[c];
    v0.col(c) = x0.col(index_inlier);
    v1.col(c) = x1.col(index_inlier);
  }  
  Mat3 E;
  // Computes essential matrix
  EssentialFromFundamental(F, K1, K2, &E);
  
  Mat3 dR;
  Vec3 dt;
  // Recover motion between the two images
  bool is_motion_ok = MotionFromEssentialAndCorrespondence(E, K1, v0.col(0), 
                                                           K2, v1.col(0),       
                                                           &dR, &dt);
  if (!is_motion_ok) {
    LOG(ERROR) << "Error: the motion cannot be estimated.";
    return false;
  }
  
  Mat3 R;
  Vec3 t;
  PinholeCamera * pcamera = NULL;
  pcamera = dynamic_cast<PinholeCamera *>(
    reconstruction->GetCamera(image_id1));
  // If the first image has no associated camera, we choose the center of the 
  // coordinate frame
  if (!pcamera) {
    R.setIdentity();
    t.setZero();
    pcamera = new PinholeCamera(K1, R, t);
    reconstruction->InsertCamera(image_id1, pcamera);
    VLOG(1)   << "Add Camera ["
              << image_id1 <<"]"<< std::endl <<"R="
              << R << std::endl <<"t= "
              << t.transpose()
              << std::endl;
  }
  // Recover the asolute pose: R2 = R1 * dR, t2 = R1 * dt + t1
  R = pcamera->orientation_matrix() * dR;
  t = pcamera->orientation_matrix() * dt + pcamera->position();
  
  // Creates and adds the second camera
  pcamera = new PinholeCamera(K2, R, t);
  reconstruction->InsertCamera(image_id2, pcamera);
  VLOG(1)   << "Add Camera ["
            << image_id2 <<"]"<< std::endl <<"R="
            << R << std::endl <<"t= "
            << t.transpose()
            << std::endl;
           
  //Adds only inliers matches into 
  const Feature * feature = NULL;
  for (size_t s = 0; s < feature_inliers.size(); ++s) {
    feature = matches.Get(image_id1, tracks[feature_inliers[s]]);
    matches_inliers->Insert(image_id1, tracks[feature_inliers[s]], feature);
    feature = matches.Get(image_id2, tracks[feature_inliers[s]]);
    matches_inliers->Insert(image_id2, tracks[feature_inliers[s]], feature);
  }
  VLOG(1)   << "Inliers added: " << feature_inliers.size() << std::endl;
  return true;
}

bool CalibratedCameraResection(const Matches &matches, 
                               CameraID image_id, 
                               const Mat3 &K, 
                               Matches *matches_inliers,
                               Reconstruction *reconstruction) {
  double rms_inliers_threshold = 1;// in pixels
  vector<StructureID> structures_ids;
  Mat2X x_image;
  Mat4X X_world;
  // Selects only the reconstructed tracks observed in the image
  SelectExistingPointStructures(matches, image_id, *reconstruction,
                                &structures_ids, &x_image);
 
  // TODO(julien) Also remove structures that are on the same location
  if (structures_ids.size() < 5) {
    LOG(ERROR) << "Error: there are not enough points to estimate the pose ("
               << structures_ids.size() << "<5).";
    // We need at least 5 tracks in order to do resection
    return false;
  }
  MatrixOfPointStructureCoordinates(structures_ids, *reconstruction, &X_world);
  CHECK(x_image.cols() == X_world.cols());
 
  Mat3X X;
  HomogeneousToEuclidean(X_world, &X);
  Mat3 R;
  Vec3 t;
  vector<int> inliers;
  EuclideanResectionEPnPRobust(x_image, X, K, rms_inliers_threshold,
                               &R, &t, &inliers, 1e-3);

  // TODO(julien) Performs non-linear optimization of the pose.
  
  // Creates a new camera and add it to the reconstruction
  PinholeCamera * camera = new PinholeCamera(K, R, t);
  reconstruction->InsertCamera(image_id, camera);
  VLOG(1)   << "Add Camera ["
            << image_id <<"]"<< std::endl <<"R="
            << R << std::endl <<"t= "
            << t.transpose()
            << std::endl;
  //Adds only inliers matches into 
  const Feature * feature = NULL;
  for (size_t s = 0; s < structures_ids.size(); ++s) {
    feature = matches.Get(image_id, structures_ids[s]);
    matches_inliers->Insert(image_id, structures_ids[s], feature);
  }
  VLOG(1)   << "Inliers added: " << structures_ids.size() << std::endl;
  return true;
}
} // namespace libmv
