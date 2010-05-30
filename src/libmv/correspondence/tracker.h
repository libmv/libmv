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

#ifndef LIBMV_CORRESPONDENCE_TRACKER_H_
#define LIBMV_CORRESPONDENCE_TRACKER_H_

#include <map>

#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/correspondence/ArrayMatcher.h"
#include "libmv/correspondence/matches.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/detector/detector.h"

namespace libmv {
namespace tracker {
  
// Abstract base classs for tracking algorithms.
// WARNING: This is at best, barely started.

// A tracker is the output of a tracking algorithm, which converts a track into
// another track when run in some context.  For example, a KLT tracking context
// is a source frame, a target frame, a window size, and a previous (point)
// tracker. Of note: The previous tracker has to be a point tracker, but that's
// all that's required. Other trackers (UKLT) will require a specific type of
// tracker as the previous position.

class Tracker {
 public:
  Tracker(detector::Detector *detector, 
          descriptor::Describer *describer,
          correspondence::ArrayMatcher<float> *matcher) : 
           detector_(detector),
           describer_(describer),
           matcher_(matcher) {};
            
  virtual ~Tracker() {}
   
  // Tracks new features between two images.
  template <typename TImage>
  bool Track(const TImage &image1, 
             const TImage &image2, 
             Matches *new_matches,
             bool keep_single_feature = true);
                     
  // Tracks all features in an image.
  template <typename TImage>
  bool Track(const TImage &image, 
             const Matches &known_matches, 
             Matches *new_matches,
             Matches::ImageID *image_id,
             bool keep_single_feature = true); 

 protected:
   scoped_ptr<detector::Detector> detector_;
   scoped_ptr<descriptor::Describer> describer_;
   scoped_ptr<correspondence::ArrayMatcher<float> > matcher_;
};

} // using namespace tracker
} // using namespace libmv

#include "tracker-inl.h"

#endif  // LIBMV_CORRESPONDENCE_TRACKER_H_
