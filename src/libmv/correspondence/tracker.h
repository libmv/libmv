// Copyright (c) 2007, 2008 libmv authors.
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

#ifndef LIBMV_CORRESPONDENCE_TRACKER_H_
#define LIBMV_CORRESPONDENCE_TRACKER_H_

#include <map>

#include "libmv/correspondence/matches.h"

namespace libmv {

// Abstract base classs for tracking algorithms.
// WARNING: This is at best, barely started.

// A tracker is the output of a tracking algorithm, which converts a track into
// another track when run in some context.  For example, a KLT tracking context
// is a source frame, a target frame, a window size, and a previous (point)
// tracker. Of note: The previous tracker has to be a point tracker, but that's
// all that's required. Other trackers (UKLT) will require a specific type of
// tracker as the previous position.
//
// TODO(keir): This class is cleary not thought out yet!
//
// Note: A tracker has no notion of correspondence; it is up to the calling
// code to handle such details.

// Tracks points between images.
class Tracker {
 public:
  // Correspondences is the only connection to the outside world?
  Tracker(Correspondences *correspondences);
  void TrackBetween(Image *image1, Image *image2);
};

// TODO (?) This class is cleary not thought out yet!
// feature 
//   track to (tracker *)
//     (now have concrete class; either pontfeat or uklt feat or line feat)
//     tracker->track(*this)
// 
//     need to pass in a lazily loaded image sequence.



// Anything needed to find a tracker in an image, given other stuff (other
// trackers, image derivatives, image cutouts, motion models, kdtrees of image
// feature vectors, etc).
class TrackManager {
  void SetDetector(Detector *detector);
  void SetTracker(Tracker *Tracker);
  void AddImage();
  void TrackBetweenImages(image1, image2);
}

} // using namespace libmv

#endif  // LIBMV_CORRESPONDENCE_TRACKER_H_
