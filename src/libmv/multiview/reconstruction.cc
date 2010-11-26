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
#include "libmv/correspondence/matches.h"
#include "libmv/multiview/autocalibration.h"
#include "libmv/multiview/bundle.h"
#include "libmv/multiview/camera.h"
#include "libmv/multiview/five_point.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/robust_resection.h"
#include "libmv/multiview/robust_euclidean_resection.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/robust_homography.h"
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
  const size_t kNumberStructuresToReserve = 1000;
  structures_ids->resize(0);
  //TODO(julien) clean this
  structures_ids->reserve(kNumberStructuresToReserve);
  vector<Vec2> xs;
  xs.reserve(kNumberStructuresToReserve);
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
  const size_t kNumberStructuresToReserve = 1000;
  structures_ids->resize(0);
  //TODO(julien) clean this
  structures_ids->reserve(kNumberStructuresToReserve);
  vector<Vec2> xs;
  xs.reserve(kNumberStructuresToReserve);
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
    Mat4X *X_world) {
  X_world->resize(4, structures_ids.size());
  PointStructure *point_s = NULL;
  for (size_t s = 0; s < structures_ids.size(); ++s) {
    point_s = dynamic_cast<PointStructure*>(
      reconstruction.GetStructure(structures_ids[s]));
    if (point_s) {
      X_world->col(s) << point_s->coords();
    }
  }
}

bool ReconstructFromTwoUncalibratedViews(const Matches &matches, 
                                         CameraID image_id1, 
                                         CameraID image_id2, 
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
  FundamentalFromCorrespondences7PointRobust(x0, x1,
                                             epipolar_threshold,
                                             &F, &feature_inliers, 
                                             1e-3);
  Mat34 P1;
  Mat34 P2;
  P1<< Mat3::Identity(), Vec3::Zero();
  PinholeCamera * pcamera = NULL;
  pcamera = dynamic_cast<PinholeCamera *>(
    reconstruction->GetCamera(image_id1));
  // If the first image has no associated camera, we choose the center of the 
  // coordinate frame
  if (!pcamera) {
    pcamera = new PinholeCamera(P1);
    reconstruction->InsertCamera(image_id1, pcamera);
    VLOG(1)   << "Add Camera ["
              << image_id1 <<"]"<< std::endl <<"P="
              << P1 << std::endl;
  } else {
    // TODO(julien) what should we do?
    // for now we enforce the first projection matrix to be the world reference
    VLOG(1)   << "Warning: the first projection matrix is overwritten to be the"
            << " world reference frame.";
    pcamera->set_projection_matrix(P1);
  }
  // Recover the second projection matrix
  ProjectionsFromFundamental(F, &P1, &P2);
  // Creates and adds the second camera
  pcamera = new PinholeCamera(P2);
  reconstruction->InsertCamera(image_id2, pcamera);
  VLOG(1)   << "Add Camera ["
            << image_id2 <<"]"<< std::endl <<"P="
              << P2 << std::endl;
            
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

uint PointStructureTriangulation(const Matches &matches, 
                                 CameraID image_id, 
                                 size_t minimum_num_views, 
                                 Reconstruction *reconstruction,
                                 vector<StructureID> *new_structures_ids) {
  // Checks that the camera is in reconstruction
  if (!reconstruction->ImageHasCamera(image_id)) {
      VLOG(1)   << "Error: the image " << image_id 
                << " has no camera." << std::endl;
    return 0;
  }
  vector<StructureID> structures_ids;
  Mat2X x_image;
  vector<Mat34> Ps; 
  vector<Vec2> xs; 
  Vec2 x;
  // Selects only the unreconstructed tracks observed in the image
  SelectUnexistingPointStructures(matches, image_id, *reconstruction,
                                  &structures_ids, &x_image);
  
  VLOG(1)   << "Points structure selected:" << x_image.cols() << std::endl;
  // Selects the point structures that are observed at least in
  // minimum_num_views images (images that have an already localized camera) 
  Mat41 X_world;
  uint number_new_structure = 0;
  if (new_structures_ids)
    new_structures_ids->reserve(structures_ids.size());
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
      // TODO(julien) apply an isotropic normalisation of 2D points 
      NViewTriangulateAlgebraic<double>(x, Ps, &X_world);
      
      // Creates an add the point structure to the reconstruction
      PointStructure * p = new PointStructure();
      p->set_coords(X_world.col(0));
      reconstruction->InsertTrack(structures_ids[t], p);
      if (new_structures_ids)
        new_structures_ids->push_back(structures_ids[t]);
      number_new_structure++;
      VLOG(1)   << "Add Point Structure ["
                << structures_ids[t] <<"] "
                << p->coords().transpose() << " ("
                << p->coords().transpose() / p->coords()[3] << ")"
                << std::endl;
    }
  }
  return number_new_structure;
}

uint PointStructureRetriangulation(const Matches &matches, 
                                   CameraID image_id,  
                                   Reconstruction *reconstruction) {
  // Checks that the camera is in reconstruction
  if (!reconstruction->ImageHasCamera(image_id)) {
      VLOG(1)   << "Error: the image " << image_id 
                << " has no camera." << std::endl;
    return 0;
  }
  vector<StructureID> structures_ids;
  Mat2X x_image;
  vector<Mat34> Ps; 
  vector<Vec2> xs; 
  Vec2 x;
  // Selects only the reconstructed structures observed in the image
  SelectExistingPointStructures(matches, image_id, *reconstruction,
                                  &structures_ids, &x_image);
  Mat41 X_world;
  uint number_updated_structure = 0;
  PinholeCamera *camera = NULL;
  PointStructure *pstructure = NULL;
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
    Mat2X x(2, xs.size());
    VectorToMatrix<Vec2, Mat2X>(xs, &x);
    // TODO(julien) apply an isotropic normalisation of 2D points 
    NViewTriangulateAlgebraic<double>(x, Ps, &X_world);
    // Creates an add the point structure to the reconstruction
    pstructure = dynamic_cast<PointStructure *>(
      reconstruction->GetStructure(structures_ids[t]));
    if (pstructure) {
      // TODO(julien) don't keep ouliers? (RMSE > threshold)
      pstructure->set_coords(X_world.col(0));
      number_updated_structure++;
      VLOG(1)   << "Point structure updated ["
                << structures_ids[t] <<"] "
                << pstructure->coords().transpose() << " ("
                << pstructure->coords().transpose() / pstructure->coords()[3] 
                << ")" << std::endl;
    }
  }
  return number_updated_structure;
}

// TODO(julien) put this somewhere else...
double EstimatesRootMeanSquareError(const Matches &matches, 
                                    Reconstruction *reconstruction) {
  PinholeCamera * pcamera = NULL;
  vector<StructureID> structures_ids;
  Mat2X x_image;
  Mat4X X_world;
  double sum_rms2 = 0;
  size_t num_features = 0;
  std::map<StructureID, Structure *>::iterator stucture_iter;
  std::map<CameraID, Camera *>::iterator camera_iter = 
      reconstruction->cameras().begin();
  for (; camera_iter != reconstruction->cameras().end(); ++camera_iter) {
    pcamera = dynamic_cast<PinholeCamera *>(camera_iter->second);
    if (pcamera) {
      SelectExistingPointStructures(matches, camera_iter->first,
                                    *reconstruction, 
                                    &structures_ids,
                                    &x_image);
      MatrixOfPointStructureCoordinates(structures_ids, 
                                        *reconstruction,
                                        &X_world);
      Mat2X dx =Project(pcamera->projection_matrix(), X_world) - x_image;
      VLOG(1)   << "|Err Cam "<<camera_iter->first<<"| = " 
                << sqrt(Square(dx.norm()) / x_image.cols()) << std::endl;
      // TODO(julien) use normSquare
      sum_rms2 += Square(dx.norm());
      num_features += x_image.cols();
    }
  }
  // TODO(julien) devide by total number of features
  return sqrt(sum_rms2 / num_features);
}

bool UncalibratedCameraResection(const Matches &matches, 
                                 CameraID image_id, 
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
  if (structures_ids.size() < 6) {
    LOG(ERROR) << "Error: there are not enough points to estimate the "
               << "projection matrix(" << structures_ids.size() << "<6).";
    // We need at least 6 tracks in order to do resection
    return false;
  }
  
  MatrixOfPointStructureCoordinates(structures_ids, *reconstruction, &X_world);
  CHECK(x_image.cols() == X_world.cols());
  
  Mat34 P;
  vector<int> inliers;
  ResectionRobust(x_image, X_world, rms_inliers_threshold, &P,&inliers, 1e-3);

  // TODO(julien) Performs non-linear optimization of the pose.
  
  // Creates a new camera and add it to the reconstruction
  PinholeCamera * camera = new PinholeCamera(P);
  reconstruction->InsertCamera(image_id, camera);
  
  VLOG(1)   << "Add Camera ["
            << image_id <<"]"<< std::endl <<"P="
            << P << std::endl;
  //Adds only inliers matches into 
  const Feature * feature = NULL;
  for (size_t s = 0; s < structures_ids.size(); ++s) {
    feature = matches.Get(image_id, structures_ids[s]);
    matches_inliers->Insert(image_id, structures_ids[s], feature);
  }
  VLOG(1)   << "Inliers added: " << structures_ids.size() << std::endl;
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

bool UpgradeToMetric(const Matches &matches, 
                     Reconstruction *reconstruction) { 
  double rms = EstimatesRootMeanSquareError(matches, reconstruction);
  VLOG(1)   << "Upgrade to Metric - Initial RMS:" << rms << std::endl;
  AutoCalibrationLinear auto_calibration_linear;
  uint image_width = 0;
  uint image_height = 0;
  PinholeCamera * pcamera = NULL;
  std::map<CameraID, Camera *>::iterator camera_iter =
    reconstruction->cameras().begin();
  for (; camera_iter != reconstruction->cameras().end(); ++camera_iter) {
    pcamera = dynamic_cast<PinholeCamera *>(camera_iter->second);
    if (pcamera) {
      image_width = pcamera->image_width();
      image_height = pcamera->image_height();
      // Avoid to have null values of image width and height
      // TODO(julien) prefer an assert?
      if (!image_width  && !image_height) {
        image_width  = 640;
        image_height = 480;
      }
      auto_calibration_linear.AddProjection(pcamera->projection_matrix(),
                                            image_width, image_height);
    }
  } 
  // TODO(julien) Put the following in a function.
  // Upgrade the reconstruction to metric using {Pm, Xm} = {P*H, H^{-1}*X}
  Mat4 H = auto_calibration_linear.MetricTransformation();
  VLOG(1)   << "Rectification H = " << H << "\n";
  if (isnan(H.sum())) {
    LOG(ERROR) << "Warning: The metric rectification cannot be applied, the "  
               << "matrix contains NaN values.\n";
    return false;
  }
  Mat34 P;
  camera_iter = reconstruction->cameras().begin();
  for (; camera_iter != reconstruction->cameras().end(); ++camera_iter) {
    pcamera = dynamic_cast<PinholeCamera *>(camera_iter->second);
    if (pcamera) {
      P = pcamera->projection_matrix() * H;
      pcamera->set_projection_matrix(P);
      P_From_KRt(pcamera->intrinsic_matrix(),  
                 pcamera->orientation_matrix(),
                 pcamera->position(), &P);
      // TODO(julien) change this.
      pcamera->set_projection_matrix(P);
    }
  }
  Mat4 H_inverse = H.inverse();
  PointStructure * pstructure = NULL;
  std::map<StructureID, Structure *>::iterator stucture_iter =
    reconstruction->structures().begin();
  for (; stucture_iter != reconstruction->structures().end(); ++stucture_iter) {
    pstructure = dynamic_cast<PointStructure *>(stucture_iter->second);
    if (pstructure) {
      pstructure->set_coords(H_inverse * pstructure->coords());
    }
  }
  // TODO(julien) Performs metric bundle adjustment
  BundleAdjust(matches, reconstruction);
  return true;
}

double BundleAdjust(const Matches &matches, 
                    Reconstruction *reconstruction) {
  double rms = 0, rms0 = EstimatesRootMeanSquareError(matches, reconstruction);
  VLOG(1)   << "Initial RMS = " << rms0 << std::endl;
  size_t ncamera = reconstruction->GetNumberCameras();
  size_t nstructure = reconstruction->GetNumberStructures();
  vector<Mat2X> x(ncamera);
  vector<Vecu>   x_ids(ncamera);
  vector<Mat3>  Ks(ncamera);
  vector<Mat3>  Rs(ncamera);
  vector<Vec3>  ts(ncamera);
  Mat3X         X(3, nstructure);
  vector<StructureID> structures_ids;
  std::map<StructureID, uint> map_structures_ids;
  
  size_t str_id = 0;
  PointStructure *pstructure = NULL;
  std::map<StructureID, Structure *>::iterator str_iter =  
    reconstruction->structures().begin();
  for (; str_iter != reconstruction->structures().end(); ++str_iter) {
    pstructure = dynamic_cast<PointStructure *>(str_iter->second);
    if (pstructure) {
      X.col(str_id) = pstructure->coords_affine();
      map_structures_ids[str_iter->first] = str_id;
      str_id++;
    } else {
      LOG(FATAL) << "Error: the bundle adjustment cannot handle non point "
                 << "structure.";
      return 0;
    }
  }
  
  PinholeCamera * pcamera = NULL;
  size_t cam_id = 0;
  std::map<CameraID, Camera *>::iterator cam_iter =  
    reconstruction->cameras().begin();
  for (; cam_iter != reconstruction->cameras().end(); ++cam_iter) {
    pcamera = dynamic_cast<PinholeCamera *>(cam_iter->second);
    if (pcamera) {
      pcamera->GetIntrinsicExtrinsicParameters(&Ks[cam_id],
                                               &Rs[cam_id],
                                               &ts[cam_id]);
      SelectExistingPointStructures(matches, cam_iter->first,
                                    *reconstruction,
                                    &structures_ids, &x[cam_id]);
      x_ids[cam_id].resize(structures_ids.size());
      for (size_t s = 0; s < structures_ids.size(); ++s) {
        x_ids[cam_id][s] = map_structures_ids[structures_ids[s]];
      }
      //VLOG(1)   << "x_ids = " << x_ids[cam_id].transpose()<<"\n";
      cam_id++;
    } else {
      LOG(FATAL) << "Error: the bundle adjustment cannot handle non pinhole "
                 << "cameras.";
      return 0;
    }
  }
  // Performs metric bundle adjustment
  rms = EuclideanBA(x, x_ids, &Ks, &Rs, &ts, &X, eBUNDLE_METRIC);
  // Copy the results only if it's better
  if (rms < rms0) {
    cam_id = 0;
    cam_iter = reconstruction->cameras().begin();
    for (; cam_iter != reconstruction->cameras().end(); ++cam_iter) {
      pcamera = dynamic_cast<PinholeCamera *>(cam_iter->second);
      if (pcamera) {
        pcamera->SetIntrinsicExtrinsicParameters(Ks[cam_id],
                                                 Rs[cam_id],
                                                 ts[cam_id]);
        cam_id++;
      }
    }
    str_id = 0;
    str_iter = reconstruction->structures().begin();
    for (; str_iter != reconstruction->structures().end(); ++str_iter) {
      pstructure = dynamic_cast<PointStructure *>(str_iter->second);
      if (pstructure) {
        pstructure->set_coords_affine(X.col(str_id));
        str_id++;
      }
    }
  }
  //rms = EstimatesRootMeanSquareError(matches, reconstruction);
  VLOG(1)   << "Final RMS = " << rms << std::endl;
  return rms;
}

void SelectEfficientImageOrder(
    const Matches &matches, 
    std::list<vector<Matches::ImageID> >*connected_graph_list) {
  typedef std::pair<Matches::ImageID, Matches::ImageID> ImagesTypePairs;
  typedef vector<ImagesTypePairs > ImagesPairs;
  double h, score, max_error_h = 1;
  Mat3 H;  
 
  // TODO(julien) Find connected graph and do the rest of every graph
  {
    // Selects the two views that are not too closed but share common tracks
    size_t i = 0;
    double score_max_1 = 0;
    ImagesTypePairs image_pair_max_1;
    vector<Mat2X> xs(matches.NumImages());
    std::set<Matches::ImageID>::const_iterator image_iter1 =
      matches.get_images().begin();
    std::set<Matches::ImageID>::const_iterator image_iter2;
    image_pair_max_1 = ImagesTypePairs(*matches.get_images().begin(),
                                       *(++matches.get_images().begin()));
    vector<Mat> xs2;
    // HACK(julien) we force to keep the first image 
    // TODO(julien) implements a better selection
    //for (;image_iter1 != matches.get_images().end(); ++image_iter1) {
    {
      image_iter2 = image_iter1;
      image_iter2++;
      for (;image_iter2 != matches.get_images().end(); ++image_iter2) {
        TwoViewPointMatchMatrices(matches, *image_iter1, *image_iter2, &xs2);
        if (xs2[0].cols() >= 4) {
          h = HomographyFromCorrespondences4PointRobust(xs2[0], xs2[1], 
                                                        max_error_h, 
                                                        &H, NULL, 1e-2);
          // the score is homography x number of matches
          //TODO(julien) Actually it should be the median of the homography...
          score = h * xs2[0].cols();
          i++;
          VLOG(1)   << "Score["<<i<<"] = " << score <<"\n";
          if (score > score_max_1) {
            score_max_1 = score;
            image_pair_max_1 = ImagesTypePairs(*image_iter1, *image_iter2);
            VLOG(1)   << " max score found !\n";
          }
        }
      }
    }
    vector<Matches::ImageID> v(matches.NumImages());
    if (score_max_1 != 0) {
      v[0] = image_pair_max_1.first;
      v[1] = image_pair_max_1.second;
      i = 2;
    } else {
      i = 0;
    }
    // Fill the rest of images (not ordered)
    // TODO(julien) maybe we can do better than a non ordered list here?
    image_iter1 = matches.get_images().begin();
    for (;image_iter1 != matches.get_images().end(); ++image_iter1) {
      if (score_max_1 == 0 || (*image_iter1 != v[0] && *image_iter1 != v[1])) {
        v[i] = *image_iter1;
        ++i;
      }
    }
    connected_graph_list->push_back(v);
  }
}
} // namespace libmv
