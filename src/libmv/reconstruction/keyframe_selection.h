// Copyright (c) 2010, 2011 libmv authors.
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

#ifndef LIBMV_RECONSTRUCTION_KEYFRAME_SELECTION_H_
#define LIBMV_RECONSTRUCTION_KEYFRAME_SELECTION_H_

#include "libmv/reconstruction/reconstruction.h"

namespace libmv {

// This method selects only 'keyframes' according to the number of shared tracks
// - the first image is selected
// - then an image is selected if the next image has not enough shared tracks
//   with the last keyframe
// min_matches_pc represents the minimum percentage of shared tracks between the
// current image and the previous keyframe. If the number of shared tracks drops
// below min_matches_pc *100% of the keyframe total number of tracks or is less
// than min_num_matches, then the previous image is selected as a keyframe.
void SelectKeyframesBasedOnMatchesNumber(const Matches &matches, 
                                         vector<Matches::ImageID> *keyframes,
                                         float min_matches_pc = 0.15,
                                         int min_num_matches = 50);
}  // namespace libmv

#endif  // LIBMV_RECONSTRUCTION_KEYFRAME_SELECTION_H_

