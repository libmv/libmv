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

#ifndef LIBMV_MULTIVIEW_RECONSTRUCTION_H_
#define LIBMV_MULTIVIEW_RECONSTRUCTION_H_

#include <cstdio>
#include <map>

#include "libmv/correspondence/bipartite_graph.h"
#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"
#include "libmv/multiview/camera.h"
#include "libmv/multiview/structure.h"

namespace libmv {

typedef unsigned int FrameID;
typedef unsigned int TrackID;

// Use cases:
//
// Reconstruction is a subset of the tracks.  For a single track, there is a 3D
// point. For a single camera there are many points.
//
// For bundling: Must map between a single vector of parameters and the
// reconstruction. Can do this post-reconstruction if data is a pointer inside
// camera and structures to an eigen vector or array.
//
// Perhaps reconstruction error should be done separately; by binding 'error
// blocks' which combine camera, feature, and structure.
//
// For outputting to a blender script:
//
//   Iterate over (derived) cameras
//   Iterate over (derived) points
//
// For bundling:
// 
//   Iterate over features, and access corresponding structure / camera.
//
// For resection:
//
//   1. Find a image that has no reconstructed camera.
//   2. Find the points that have a corresponding Structure; store this map.
//   3. Recover camera via resection and the Feature<->Structure
//      correspondences.
//
//   For 1: Loop through each Image, see if there is a Camera.
//     
//     Need fast 'Image to Camera or NULL" on Reconstruction.
//
//   For 2: Loop through Tracks matching Features in this Image, checking for
//   Structure.
//
//     Need fast "Feature to Track or NULL" on Matches. (TracksInImagesFI)
//     Need fast "Track to Structure or NULL" query on Reconstruction.
//
// For intersection:
//
//   Find sets of Images that have Features with no corresponding Structure.
//   That is to say, for a set of Images that have Cameras reconstructed, find
//   tracks that are not yet reconstructed that are visible in these images.
//
//   To do this: Use TracksInImagesIterator over the set of Images. Then check
//   if that track is in the reconstruction.
//
//     Need fast "Track has Structure?" query.
//     Need fast "Image has Camera?" query.
//   

// Possible signatures:
//
//   ReconstructFromTwoViews(Matches, ID, ID, Matches *, Reconstruction *);
//   ReconstructFromThreeViews(Matches, ID, ID, ID, Matches *, Reconstruction *);
//   CameraResection(Matches, ID, Matches *, Reconstruction *);
//   MergeReconstructions(Matches, Reconstruction &, Reconstruction &, Matches *, Reconstruction *);
//   BundleAdjust(Matches, Reconstruction *);

// The reconstruction takes ownership of camera and structure.
class Reconstruction {
 public:
   
  Reconstruction() {}
  ~Reconstruction() {}

  void InsertCamera(FrameID id, Camera *camera) {
    std::map<FrameID, Camera *>::iterator it = cameras_.find(id);
    if (it != cameras_.end()) {
      delete it->second;
      it->second = camera;
    } else {
      cameras_[id] = camera;
    }
  }
  void InsertTrack(TrackID id, Structure *structure) {
    std::map<FrameID, Structure *>::iterator it = structures_.find(id);
    if (it != structures_.end()) {
      delete it->second;
      it->second = structure; 
    } else {
      structures_[id] = structure;
    }
  }
  
  Camera * GetCamera(FrameID id) {
    std::map<FrameID, Camera *>::iterator it = cameras_.find(id);
    if (it != cameras_.end()) {
      LOG(ERROR) << " No camera exists with this ID." << std::endl;
      return NULL;
    } else {
      return cameras_[id];
    }
  }
  Structure * GetStructure(TrackID id) {
    std::map<TrackID, Structure *>::iterator it = structures_.find(id);
    if (it != structures_.end()) {
      LOG(ERROR) << " No Structure exists with this ID." << std::endl;
      return NULL;
    } else {
      return structures_[id];
    }
  }
  
  void ClearCamerasMap() {
    std::map<FrameID, Camera *>::iterator it = cameras_.begin();
    for (; it != cameras_.end(); ++it) {
      delete it->second;
    }
    cameras_.clear();
  }
  void ClearStructuresMap() {
    std::map<FrameID, Structure *>::iterator it = structures_.begin();
    for (; it != structures_.end(); ++it) {
      delete it->second;
    }
    structures_.clear();
  }
  
  size_t GetNumberCameras() const    { return cameras_.size(); }
  size_t GetNumberStructures() const { return structures_.size(); }

  std::map<FrameID, Camera *> &cameras()  { return cameras_; }
  std::map<TrackID, Structure *> &structures() { return structures_; }
  
 private:
  std::map<FrameID, Camera *> cameras_;
  std::map<TrackID, Structure *> structures_;
};

}  // namespace libmv

#endif  // LIBMV_MULTIVIEW_RECONSTRUCTION_H_
