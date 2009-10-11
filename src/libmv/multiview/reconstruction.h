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

#include "libmv/correspondence/bipartite_graph_new.h"
#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

typedef Camera;
typedef Structure;

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
  void Insert(FrameID id, const Camera *camera) {
    map<FrameID, Camera *>::iterator it = cameras.find(id);
    if (it != cameras.end()) {
      delete it->second;
    }
    it->second = camera;
  }
  void Insert(TrackID id, const Structure *structure) {
    map<FrameID, Structure *>::iterator it = structure.find(id);
    if (it != structure.end()) {
      delete it->second;
    }
    it->second = structure;
  }

 private:
  map<FrameID, Camera *> cameras;
  map<TrackID, Structure *> structure;
};

}  // namespace libmv

#endif  // LIBMV_MULTIVIEW_RECONSTRUCTION_H_
