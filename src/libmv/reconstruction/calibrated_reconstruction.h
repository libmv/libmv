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

#ifndef LIBMV_MULTIVIEW_RECONSTRUCTION_CALIBRATED_H_
#define LIBMV_MULTIVIEW_RECONSTRUCTION_CALIBRATED_H_

#include "libmv/reconstruction/reconstruction.h"

namespace libmv {

// Estimates the poses of the two cameras using the fundamental and essential
// matrices.
// The method:
//    selects common matches of the two images
//    robustly estimates the fundamental matrix
//    estimates the essential matrix from the fundamental matrix
//    extracts the relative motion from the essential matrix
//    if the first image has no camera, it creates the camera and initializes
//      the pose to be the world frame
//    estimates the absolute pose of the second camera from the first pose and
//      the estimated motion.
//    creates and adds it to the reconstruction
//    inserts only inliers matches into matches_inliers
// Returns true if the pose estimation has succeed
// Returns false if 
//    the number of common matches is less than 7
//    there is no solution for the relative motion from the essential matrix
bool ReconstructFromTwoCalibratedViews(const Matches &matches, 
                                       CameraID image_id1, 
                                       CameraID image_id2, 
                                       const Mat3 &K1, 
                                       const Mat3 &K2, 
                                       Matches *matches_inliers,
                                       Reconstruction *reconstruction);

// Estimates the pose of the camera using the already reconstructed structures.
// The method:
//    selects the tracks that have an already reconstructed structure
//    robustly estimates the camera extrinsic parameters (R,t) by resection
//    creates and adds the new camera to reconstruction
//    inserts only inliers matches into matches_inliers
// Returns true if the resection has succeed
// Returns false if 
//    the number of reconstructed Tracks is less than 5
bool CalibratedCameraResection(const Matches &matches, 
                               CameraID image_id, 
                               const Mat3 &K, 
                               Matches *matches_inliers,
                               Reconstruction *reconstruction);
}  // namespace libmv

#endif  // LIBMV_MULTIVIEW_RECONSTRUCTION_CALIBRATED_H_
