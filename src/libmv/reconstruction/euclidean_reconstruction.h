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

#ifndef LIBMV_RECONSTRUCTION_EUCLIDEAN_RECONSTRUCTION_H_
#define LIBMV_RECONSTRUCTION_EUCLIDEAN_RECONSTRUCTION_H_

#include "libmv/reconstruction/reconstruction.h"

namespace libmv {

// Estimates the pose of the camera using the already reconstructed points.
// The method:
//  - selects the tracks that have an already reconstructed structure
//  - robustly estimates the camera extrinsic parameters (R,t) by resection
//  - creates and adds the new camera to reconstruction
//  - inserts only inliers matches into matches_inliers
// Returns true if the resection has succeed
// Returns false if 
//  - the number of reconstructed Tracks is less than 5
bool CalibratedCameraResection(const Matches &matches, 
                               Matches::ImageID image_id, 
                               const Mat3 &K, 
                               Matches *matches_inliers,
                               Reconstruction *reconstruction);
                               
// Estimates a precise initial reconstruction using the matches of two views.
// The method:
//  - selects common matches of the two images
//  - robustly estimates the fundamental matrix
//  - estimates the essential matrix from the fundamental matrix
//  - extracts the relative motion from the essential matrix
//  - if the first image has no camera, it creates the camera and initializes
//    the pose to be the world coordinate frame
//  - estimates the absolute pose of the second camera from the first pose and
//    the estimated motion.
//  - creates and adds the cameras to the reconstruction
//  - reconstructs only the inliers matches (point triangulation)
//  - performs a metric bundle adjusment
//    TODO(julien) remove outliers from matches or output matches_inliers.
// Returns true if the initial reconstruction has succeed
// Returns false if 
//  - the number of common matches is less than 7
//  - there is no solution for the relative motion from the essential matrix
bool InitialReconstructionTwoViews(const Matches &matches,
                                   Matches::ImageID image1,
                                   Matches::ImageID image2,
                                   const Mat3 &K1,
                                   const Mat3 &K2,
                                   const Vec2u &image_size1,
                                   const Vec2u &image_size2,
                                   Reconstruction *recons);
                               
// Estimates the pose of the keyframes using the already reconstructed points.
// For every keyframes (starting the first_keyframe_index th):
//  - the keyframe is localized (by resection)
//  - if the resection has not failed, the inliers tracks are reconstructed
//    by point triangulation
//  - if new points are created, a global bundle adjustment is performed
//    TODO(julien) a local bundle adjustment would be sufficient?
// The method stops when one keyframe cannot be localized (tracking lost),
// keyframe_stopped_index is the index of this keyframe.
// Returns true if all keyframes have been localized
// Returns false if one keyframe cannot be localized (tracking lost).
bool IncrementalReconstructionKeyframes(const Matches &matches,
                                        const vector<Matches::ImageID> &kframes,
                                        const int first_keyframe_index,
                                        const Mat3 &K,
                                        const Vec2u &image_size,
                                        Reconstruction *reconstruction,
                                        int *keyframe_stopped_index);
                               
// Estimates the pose of non already localized frames using the already 
// reconstructed points by resection.
// It performs also a bundle adjustment when X=10 new cameras are localized.
// TODO(julien) a local bundle adjustment would be sufficient?
// The method automatically detect the reconstruction the frame may belongs.
// NOTE: this method works only if the frame in the Matches class are ordered. 
//       If it is not the case, it will fail.
// Returns true.
bool ReconstructionNonKeyframes(const Matches &matches,
                                const Mat3 &K,
                                const Vec2u &image_size,
                                std::list<Reconstruction *> *reconstructions);

// Computes the trajectory of a camera using matches as input.
//  - First keyframes are detected according a minimum number of shared tracks
//  - Next the first two keyframes are used to estimate an initial structure
//  - Then every other keyframe are reconstructed based on an euclidean resection
//    algorithm with the previously recontructed structures and new structures
//    are estimated (points triangulation). A bundle adjustment is performed on
//    all the reconstruction each time a keyframe is localized.
//    TODO(julien) a local bundle adjustment would be sufficient?
//    TODO(julien) +a global bundle adjusment on all data at the end?
//  - In a final step, non-keyframes are localized using the resection method.
//    A bundle adjusment is periodically performed on all the data.
// In the case that the tracking is lost, a new reconstruction is created.
// TODO(julien) Add the calibration matrix K as input?
// TODO(julien) remove outliers from matches or output inliers matches.
bool EuclideanReconstructionFromVideo(
    const Matches &matches, 
    int image_width, 
    int image_height,
    double focal,
    std::list<Reconstruction *> *reconstructions);

// Computes the poses of all unordered images.
// TODO(julien) implement me.
bool EuclideanReconstructionFromImageSet(
    const Matches &matches, 
    const vector<std::pair<size_t, size_t> > &image_sizes,
    std::list<Reconstruction *> *reconstructions);
}  // namespace libmv

#endif  // LIBMV_RECONSTRUCTION_EUCLIDEAN_RECONSTRUCTION_H_
