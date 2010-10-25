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

#include <fstream>
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/matches.h"
#include "libmv/multiview/camera.h"
#include "libmv/multiview/five_point.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/robust_euclidean_resection.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/reconstruction.h"
#include "libmv/multiview/structure.h"

namespace libmv {

/// TODO(julien) put this in vector_utils.h?
/// TODO(julien) can we use Eigen::Map?
/// Convert a vector<Tvec> of vectors Tvec to a matrix Tmat
template <typename Tvec, typename Tmat>
inline void VectorToMatrix(vector<Tvec> &vs, Tmat *m) {
  Tvec v;
  m->resize(v.size(), vs.size());
  size_t c = 0;
  for (Tvec * vec = vs.begin(); vec != vs.end(); ++vec) {
    m->col(c) = *vec;
    c++;
  }
}

// Selects only the already reconstructed tracks observed in the image image_id
// and returns a vector of StructureID and their feature coordinates
void SelectExistingPointStructures(const Matches &matches, 
                                   CameraID image_id,
                                   const Reconstruction &reconstruction,
                                   vector<StructureID> *structures_ids,
                                   Mat2X *x_image) {
  const size_t kNumberTracksToReserve = 1000;
  //TODO(julien) clean this
  structures_ids->reserve(kNumberTracksToReserve);
  vector<Vec2> xs;
  xs.reserve(kNumberTracksToReserve);
  Matches::Features<PointFeature> fp =
    matches.InImage<PointFeature>(image_id);
  while (fp) {
    if (reconstruction.TrackHasStructure(fp.track())) {
      structures_ids->push_back(fp.track());
      xs.push_back(fp.feature()->coords.cast<double>());
    }
    fp.operator++();
  }
  VectorToMatrix<Vec2, Mat2X>(xs, x_image);
}

// Selects only the NOT already reconstructed tracks observed in the image
// image_id and returns a vector of StructureID and their feature coordinates
void SelectUnexistingPointStructures(const Matches &matches, 
                                    CameraID image_id,
                                    const Reconstruction &reconstruction,
                                    vector<StructureID> *structures_ids,
                                    Mat2X *x_image) {
  const size_t kNumberTracksToReserve = 1000;
  //TODO(julien) clean this
  structures_ids->reserve(kNumberTracksToReserve);
  vector<Vec2> xs;
  xs.reserve(kNumberTracksToReserve);
  Matches::Features<PointFeature> fp =
    matches.InImage<PointFeature>(image_id);
  while (fp) {
    if (!reconstruction.TrackHasStructure(fp.track())) {
      structures_ids->push_back(fp.track());
      xs.push_back(fp.feature()->coords.cast<double>());
    }
    fp.operator++();
  }
  VectorToMatrix<Vec2, Mat2X>(xs, x_image);
}

// Recover the position of the selected point structures
void MatrixOfPointStructureCoordinates(
    const vector<StructureID> &structures_ids,
    const Reconstruction &reconstruction,
    Mat3X *X_world) {
  X_world->resize(3, structures_ids.size());
  PointStructure *point_s = NULL;
  for (size_t s = 0; s < structures_ids.size(); ++s) {
    point_s = dynamic_cast<PointStructure*>(
      reconstruction.GetStructure(structures_ids[s]));
    if (point_s) {
      X_world->col(s) << point_s->coords_affine();
    }
  }
}

bool ReconstructFromTwoCalibratedViews(const Matches &matches, 
                                       CameraID image_id1, 
                                       CameraID image_id2, 
                                       const Mat3 &K1, 
                                       const Mat3 &K2, 
                                       Matches *matches_all,
                                       Reconstruction *reconstruction) {
  double epipolar_threshold = 0.5;
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
  FundamentalFromCorrespondences7PointRobust(x0,
                                             x1,
                                             epipolar_threshold,
                                             &F,
                                             &feature_inliers);
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
    LOG(INFO) << "Add Camera ["
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
  LOG(INFO) << "Add Camera ["
            << image_id2 <<"]"<< std::endl <<"R="
            << R << std::endl <<"t= "
            << t.transpose()
            << std::endl;
            
  // TODO(julien) Add only inliers matches to matches_out
  return true;
}

bool PointStructureTriangulation(const Matches &matches, 
                                 CameraID image_id, 
                                 size_t minimum_num_views, 
                                 Reconstruction *reconstruction) {
  // Checks that the camera is in reconstruction
  if (!reconstruction->ImageHasCamera(image_id)) {
      LOG(INFO) << "Error: the image " << image_id 
                << " has no camera." << std::endl;
    return false;
  }
  vector<StructureID> structures_ids;
  Mat2X x_image;
  vector<Mat34> Ps; 
  vector<Vec2> xs; 
  Vec2 x;
  // Selects only the unreconstructed tracks observed in the image
  SelectUnexistingPointStructures(matches, image_id, *reconstruction,
                                  &structures_ids, &x_image);
  // Selects the point structures that are observed at least in
  // minimum_num_views images (images that have an already localized camera) 
  Mat41 X_world;
  PinholeCamera *camera = NULL;
  for (size_t t = 0; t < structures_ids.size(); ++t) {
    Matches::Features<PointFeature> fp =
      matches.InTrack<PointFeature>(structures_ids[t]);
    Ps.clear();
    xs.clear();
    while (fp) {
      camera = dynamic_cast<PinholeCamera *>(
        reconstruction->GetCamera(fp.image()));
      if (camera) {
        Ps.push_back(camera->projection_matrix()); 
        x << fp.feature()->x(), fp.feature()->y();
        xs.push_back(x);
      }
      fp.operator++();
    }
    if (Ps.size() >= minimum_num_views) {
      Mat2X x(2, xs.size());
      VectorToMatrix<Vec2, Mat2X>(xs, &x);
      NViewTriangulate<double>(x, Ps, &X_world);
      // Creates an add the point structure to the reconstruction
      // TODO(julien) don't keep ouliers? (RMSE > threshold)
      PointStructure * p = new PointStructure();
      p->set_coords(X_world.col(0));
      reconstruction->InsertTrack(structures_ids[t], p);
      
      LOG(INFO) << "Add Point Structure ["
                << structures_ids[t] <<"] "
                << X_world.col(0).transpose() << " ("
                << X_world.col(0).transpose() / X_world(3, 0)  << ")"
                << std::endl;
    }
  }
  return true;
}

bool EuclideanCameraResection(const Matches &matches_one, 
                              CameraID image_id, 
                              const Mat3 &K, 
                              Matches *matches_all,
                              Reconstruction *reconstruction) {
  vector<StructureID> structures_ids;
  Mat2X x_image;
  Mat3X X_world;
  // Selects only the reconstructed tracks observed in the image
  SelectExistingPointStructures(matches_one, image_id, *reconstruction,
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
   
  double max_error_threshold = 0.3;
  Mat3 R;
  Vec3 t;
  vector<int> inliers;
  EuclideanResectionEPnPRobust(x_image, X_world, K, max_error_threshold,
                               &R, &t,&inliers);

  // TODO(julien) Performs non-linear optimization of the pose.
  
  // Creates a new camera and add it to the reconstruction
  PinholeCamera * camera = new PinholeCamera(K, R, t);
  reconstruction->InsertCamera(image_id, camera);
  
  LOG(INFO) << "Add Camera ["
            << image_id <<"]"<< std::endl <<"R="
            << R << std::endl <<"t= "
            << t.transpose()
            << std::endl;
  // TODO(julien) Add only inliers matches to matches_out
  return true;
}

void ExportToPLY(Reconstruction &reconstruct, std::string outFileName) {
  std::ofstream outfile;
  outfile.open(outFileName.c_str(), std::ios_base::out);
  if (outfile.is_open()) {
    outfile << "ply" << std::endl;
    outfile << "format ascii 1.0" << std::endl;
    outfile << "comment Made by libmv authors" << std::endl;
    outfile << "comment 3D points structure:" << std::endl;
    outfile << "element vertex " << reconstruct.GetNumberStructures() 
      << std::endl;
    outfile << "property float x" << std::endl;
    outfile << "property float y" << std::endl;
    outfile << "property float z" << std::endl;
    outfile << "property uchar red" << std::endl;
    outfile << "property uchar green" << std::endl;
    outfile << "property uchar blue" << std::endl;
    outfile << "comment Cameras positions:" << std::endl;
    outfile << "element vertex " << reconstruct.GetNumberCameras() << std::endl;
    outfile << "property float x" << std::endl;
    outfile << "property float y" << std::endl;
    outfile << "property float z" << std::endl;
    outfile << "property uchar red" << std::endl;
    outfile << "property uchar green" << std::endl;
    outfile << "property uchar blue" << std::endl;
    outfile << "end_header" << std::endl;
    std::map<StructureID, Structure *>::iterator track_iter =
      reconstruct.structures().begin();
    for (; track_iter != reconstruct.structures().end(); ++track_iter) {
      PointStructure * point_s = 
        dynamic_cast<PointStructure*>(track_iter->second);
      if (point_s) {
        // Exports the point affine position
        outfile << point_s->coords_affine().transpose() << " ";
        // Exports the point color
        outfile << "255 255 255" << std::endl;
      }
    }
    std::map<CameraID, Camera *>::iterator camera_iter =  
      reconstruct.cameras().begin();
    for (; camera_iter != reconstruct.cameras().end(); ++camera_iter) {
      PinholeCamera * camera_pinhole =  
        dynamic_cast<PinholeCamera *>(camera_iter->second);
      if (camera_pinhole) {
        // Exports the camera position
        outfile << camera_pinhole->position().transpose() << " ";
        // Exports the camera color
        outfile << "255 0 0" << std::endl;
      }
    }
    outfile.close();
  }
}
} // namespace libmv
