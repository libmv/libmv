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

#ifndef LIBMV_RECONSTRUCTION_MAPPING_H_
#define LIBMV_RECONSTRUCTION_MAPPING_H_

#include "libmv/reconstruction/reconstruction.h"

namespace libmv {

// Reconstructs unreconstructed point tracks observed in the image image_id
// using theirs observations (matches). 
// To be reconstructed, the tracks need to be viewed in more than
// minimum_num_views images.
// The method:
//    selects the tracks that haven't been already reconstructed
//    reconstructs the tracks into structures
//    TODO(julien) only add inliers?
//    creates and add them in reconstruction
// Returns the number of structures reconstructed and the list of triangulated
// points
uint PointStructureTriangulation(
   const Matches &matches, 
   CameraID image_id, 
   size_t minimum_num_views, 
   Reconstruction *reconstruction,
   vector<StructureID> *new_structures_ids = NULL);

// Retriangulates point tracks observed in the image image_id using theirs
// observations (matches). To be reconstructed, the tracks need to be viewed
// in more than minimum_num_views images.
// The method:
//    selects the tracks that have been already reconstructed
//    reconstructs the tracks into structures
//    TODO(julien) only add inliers?
// Returns the number of structures retriangulated
uint PointStructureRetriangulation(const Matches &matches, 
                                   CameraID image_id,  
                                   Reconstruction *reconstruction);
}  // namespace libmv

#endif  // LIBMV_RECONSTRUCTION_MAPPING_H_
