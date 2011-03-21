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
#include "libmv/correspondence/matches.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/five_point.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/robust_euclidean_resection.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/reconstruction/euclidean_reconstruction.h"
#include "libmv/reconstruction/export_blender.h"
#include "libmv/reconstruction/export_ply.h"
#include "libmv/reconstruction/image_selection.h"
#include "libmv/reconstruction/keyframe_selection.h"
#include "libmv/reconstruction/mapping.h"
#include "libmv/reconstruction/optimization.h"
#include "libmv/reconstruction/tools.h"


namespace libmv {

// Function that set the given image size to an image
inline bool SetImageSize(Reconstruction &recons,
                         Matches::ImageID image_id, 
                         const Vec2u &image_size) {
  PinholeCamera *camera = dynamic_cast<PinholeCamera*>(
    recons.GetCamera(image_id));
  if (camera) {
    camera->set_image_size(image_size); 
    return true;
  }
  return false;
}

bool CalibratedCameraResection(const Matches &matches, 
                               Matches::ImageID image_id, 
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
  
  // Create a new camera and add it to the reconstruction
  PinholeCamera * camera = new PinholeCamera(K, R, t);
  reconstruction->InsertCamera(image_id, camera);
  VLOG(1)   << "Add Camera ["
            << image_id <<"]"<< std::endl <<"R="
            << R << std::endl <<"t= "
            << t.transpose()
            << std::endl;
  //Add only inliers matches into 
  const Feature * feature = NULL;
  for (size_t s = 0; s < structures_ids.size(); ++s) {
    feature = matches.Get(image_id, structures_ids[s]);
    matches_inliers->Insert(image_id, structures_ids[s], feature);
  }
  VLOG(1)   << "Inliers added: " << structures_ids.size() << std::endl;
  return true;
}

bool InitialReconstructionTwoViews(const Matches &matches,
                                   Matches::ImageID image1,
                                   Matches::ImageID image2,
                                   const Mat3 &K1,
                                   const Mat3 &K2,
                                   const Vec2u &image_size1,
                                   const Vec2u &image_size2,
                                   Reconstruction *recons) {
  assert(image1 != image2);
  bool is_good = true;
  uint num_new_points = 0;
  Matches matches_inliers;
  
  VLOG(2) << " -- Initial Motion Estimation --  " << std::endl;
  double epipolar_threshold = 1;// epipolar error in pixels
  double outliers_probability = 1e-3;// epipolar error in pixels
  vector<Mat> xs(2);
  vector<Matches::TrackID> tracks;
  vector<Matches::ImageID> images;
  images.push_back(image1);
  images.push_back(image2);
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
                                             outliers_probability);
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
  pcamera = dynamic_cast<PinholeCamera *>(recons->GetCamera(image1));
  // If the first image has no associated camera, we choose the center of the 
  // coordinate frame
  if (!pcamera) {
    R.setIdentity();
    t.setZero();
    pcamera = new PinholeCamera(K1, R, t);
    recons->InsertCamera(image1, pcamera);
    VLOG(1)   << "Add Camera ["
              << image1 <<"]"<< std::endl <<"R="
              << R << std::endl <<"t= "
              << t.transpose()
              << std::endl;
  }
  // Recover the asolute pose: R2 = R1 * dR, t2 = R1 * dt + t1
  R = pcamera->orientation_matrix() * dR;
  t = pcamera->orientation_matrix() * dt + pcamera->position();
  
  // Creates and adds the second camera
  pcamera = new PinholeCamera(K2, R, t);
  recons->InsertCamera(image2, pcamera);
  VLOG(1)   << "Add Camera ["
            << image2 <<"]"<< std::endl <<"R="
            << R << std::endl <<"t= "
            << t.transpose()
            << std::endl;
           
  //Adds only inliers matches into 
  const Feature * feature = NULL;
  for (size_t s = 0; s < feature_inliers.size(); ++s) {
    feature = matches.Get(image1, tracks[feature_inliers[s]]);
    matches_inliers.Insert(image1, tracks[feature_inliers[s]], feature);
    feature = matches.Get(image2, tracks[feature_inliers[s]]);
    matches_inliers.Insert(image2, tracks[feature_inliers[s]], feature);
  }
  // TODO(julien) remove outliers from matches using matches_inliers.
  if (!is_good) {
    return false;                        
  }
  
  SetImageSize(*recons, image1, image_size1);
  SetImageSize(*recons, image2, image_size2);
  
  VLOG(2) << " -- Initial Intersection --  " << std::endl;
  num_new_points = PointStructureTriangulationCalibrated(matches_inliers, 
                                                         image1,
                                                         2,
                                                         recons);
  VLOG(2) << num_new_points << " points reconstructed." << std::endl;
  
  ExportToBlenderScript(*recons, "init.py");
  
  // Performs projective bundle adjustment
  if (num_new_points > 0) {
    VLOG(2) << " -- Bundle adjustment --  " << std::endl;
    MetricBundleAdjust(matches_inliers, recons);
    ExportToBlenderScript(*recons, "init-ba.py");
    // TODO(julien) Remove outliers RemoveOutliers() + BA again
  }
  return is_good;
}

bool IncrementalReconstructionKeyframes(const Matches &matches,
                                        const vector<Matches::ImageID> &kframes,
                                        const int first_keyframe_index,
                                        const Mat3 &K,
                                        const Vec2u &image_size,
                                        Reconstruction *reconstruction,
                                        int *keyframe_stopped_index) {
  bool is_good = true;
  int keyframe_index = first_keyframe_index;
  int min_num_views_for_triangulation = 2;
  uint num_new_points = 0;
  Matches::ImageID image_id;
  // Estimates the pose every other images by resection-intersection
  for (; keyframe_index < kframes.size(); ++keyframe_index) {
    *keyframe_stopped_index = keyframe_index;
    Matches matches_inliers; 
    image_id = kframes[keyframe_index];
    VLOG(2) << " -- Incremental Resection --  " << std::endl;
    is_good = CalibratedCameraResection(matches, 
                                        image_id, K,
                                        &matches_inliers, 
                                        reconstruction);   
    if (!is_good) {
      VLOG(1) << "[Warning] Tracking lost!" << std::endl;
      // The resection has returned an error: 
      // we cannot estimate the camera pose by resection so we create 
      // a new reconstruction (since the new one will not share the same
      // scale and coordinate frame.
      return false;
    }
    SetImageSize(*reconstruction, image_id, image_size);
    // TODO(julien) remove outliers from matches using matches_inliers.
  
    VLOG(2) << " -- Incremental Intersection --  " << std::endl;
    num_new_points = PointStructureTriangulationCalibrated(
                     matches, 
                     image_id,
                     min_num_views_for_triangulation, 
                     reconstruction);    
    VLOG(2) << num_new_points << " points reconstructed." << std::endl;
    
    // Performs a bundle adjustment
    if (num_new_points > 0) {
      VLOG(2) << " -- Bundle adjustment --  " << std::endl;
      MetricBundleAdjust(matches, reconstruction);
      // TODO(julien) is a local BA sufficient here?
      // TODO(julien) Remove outliers RemoveOutliers() + BA again
    }
  }
  return true;
}

bool ReconstructionNonKeyframes(const Matches &matches,
                                const Mat3 &K,
                                const Vec2u &image_size,
                                std::list<Reconstruction *> *reconstructions) {
  bool is_recons_ok = true;
  // Perform a bundle adjustment every X new cameras
  int num_new_cameras_to_proceed_ba = 10; 
  bool do_bundle_adjustment = false;
  bool is_frame_in_current_recons = false;
  bool is_frame_in_next_recons = false;
  std::set<Matches::ImageID>::const_iterator img_iter = 
    matches.get_images().begin();
  std::list<Reconstruction *>::iterator recons_iter = reconstructions->begin();
  std::list<Reconstruction *>::iterator recons_iter_next = recons_iter;
  recons_iter_next++;
  int cpt_image_for_ba = 0, cpt_i = 0;
  for (; img_iter != matches.get_images().end(); ++img_iter) {
    if (cpt_image_for_ba < num_new_cameras_to_proceed_ba - 1) {
      do_bundle_adjustment = false;
      cpt_image_for_ba++;
    } else {
      do_bundle_adjustment = true;
      cpt_image_for_ba = 0;
    }
    // If the image is not a keyframe, it should belong to the same 
    // reconstruction than the previous image.
    // We try to find if the image has an already reconstructed camera for 
    // the current reconstruction and the next one
    is_frame_in_current_recons = (*recons_iter)->ImageHasCamera(*img_iter);
    is_frame_in_next_recons = recons_iter_next != reconstructions->end() && 
                                 (*recons_iter_next)->ImageHasCamera(*img_iter);
    if (!is_frame_in_current_recons && !is_frame_in_next_recons) {
        VLOG(2) << " -- Incremental Resection --  " << std::endl;
        Matches matches_inliers; 
        is_recons_ok = CalibratedCameraResection(matches, 
                                                 *img_iter, K,
                                                 &matches_inliers, 
                                                 *recons_iter);   
        if (is_recons_ok) {        
          // TODO(julien) remove outliers from matches using matches_inliers.
          // Performs a bundle adjustment
          if (do_bundle_adjustment) {
            VLOG(2) << " -- Bundle adjustment --  " << std::endl;
            MetricBundleAdjust(matches, *recons_iter);
            // TODO(julien) should be removed when camera only are optimized
            // TODO(julien) Remove outliers RemoveOutliers() + BA again
          }
          SetImageSize(**recons_iter, *img_iter, image_size);
          std::stringstream s;
          s << "out-noKF-" << cpt_i << ".py";
          ExportToBlenderScript(**recons_iter, s.str());
        } else {
          VLOG(1) << "[Warning] Image " << *img_iter
                  << " cannot be localized!" << std::endl;
        }
        cpt_i++;
    } else if(is_frame_in_next_recons) {
      // In this case, the current image is the first keyframe of the next 
      // reconstruction so we increment the iterators.
      recons_iter++;
      recons_iter_next++;
    }
  }
  return true;
}

bool EuclideanReconstructionFromVideo(
    const Matches &matches, 
    int image_width, 
    int image_height,
    double focal,
    std::list<Reconstruction *> *reconstructions) {
  if (matches.NumImages() < 2)
    return false;
  Vec2u image_size;
  image_size << image_width, image_height;
  double cu = image_width/2 - 0.5,  cv = image_height/2 - 0.5;
  Mat3 K;  K << focal, 0, cu, 0, focal, cv, 0,   0,   1;
  
  VLOG(2) << "Selecting keyframes." << std::endl;
  libmv::vector<Matches::ImageID> keyframes;
  SelectKeyframesBasedOnMatchesNumber(matches, &keyframes);
  
  VLOG(2) << "Keyframe list: ";
  for (int i = 0; i < keyframes.size(); ++i)
    VLOG(2) << keyframes[i] << " ";
  VLOG(2) << std::endl;
  
  if (keyframes.size() < 2) {
    VLOG(1) << "Not enough keyframes! " << std::endl;
    return false;
  }
   
  bool recons_ok = true;  
  bool all_keyframes_reconstructed = false;  
  int keyframe_index = 0;
  do {
    Reconstruction *cur_recons = new Reconstruction();
    if (keyframe_index + 1 >= keyframes.size())
      break;
    recons_ok = InitialReconstructionTwoViews(matches,
                                              keyframes[keyframe_index],
                                              keyframes[keyframe_index + 1],
                                              K,K,
                                              image_size, image_size,
                                              cur_recons);    
    keyframe_index++;
    if (recons_ok) {    
      keyframe_index++;
      // If an initial reconstruction has been done, we keep it and
      // compute the next poses by intersection-resection
      reconstructions->push_back(cur_recons);
      all_keyframes_reconstructed = 
       IncrementalReconstructionKeyframes(matches,
                                          keyframes,
                                          keyframe_index,
                                          K, image_size,
                                          cur_recons,
                                          &keyframe_index);
      std::stringstream s;
        s << "out-" << keyframe_index << ".py";
      ExportToBlenderScript(*cur_recons, s.str());
    } else {
      // If the initial reconstruction can be estimated between the 
      // 2 first views, we try with the second image and the third (etc.)
    }
  }  while (!all_keyframes_reconstructed && 
            (keyframe_index < keyframes.size() - 1));
  
  // Reconstructs non-keyframes by resection
  // NOTE(julien) are we sure that matches->images is ordered? it's a std:set?
  // if not the following non-keyframes reconstruction should be changed.
  VLOG(2) << " Non-keyframe reconstruction  " << std::endl;
  ReconstructionNonKeyframes(matches,
                             K, image_size,
                             reconstructions);
  return true;
}
} // namespace libmv
