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

#ifndef LIBMV_CORRESPONDENCE_CORRESPONDENCE_H_
#define LIBMV_CORRESPONDENCE_CORRESPONDENCE_H_

#include <cstdio>

#include "libmv/correspondence/bipartite_graph.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

class Feature;
class PointFeature;
class LineFeature;

 // TODO(keir): Decide if simple numbers are good enough.
typedef int ImageID;
typedef int TrackID;

class Correspondences {
 public:
  typedef BipartiteGraph<ImageID, Feature *, TrackID> CorrespondenceGraph;

  ~Correspondences();

  class Iterator {
    friend class Correspondences;
   public:
    ImageID image()    const { return iter_.left(); }
    TrackID track()    const { return iter_.right(); }
    Feature *feature() const { return iter_.edge(); }
    bool Done()        const { return iter_.Done(); }
    void Next()              { iter_.Next(); }
    void DeleteFeature()     { iter_.DeleteEdge(); }
   private:
    Iterator(CorrespondenceGraph::Iterator iter) : iter_(iter) {}
    CorrespondenceGraph::Iterator iter_;
  };

  // Transfers ownership of feature to *this.
  void Insert(ImageID image_id, TrackID track_id, Feature *feature) {
    correspondences_.Insert(image_id, feature, track_id);
  }

  Iterator ScanAllFeatures() {
    return Iterator(correspondences_.ScanAllEdges());
  }

  Iterator ScanFeaturesForImage(ImageID image) {
    return Iterator(correspondences_.ScanEdgesForLeftNode(image));
  }

private:
  CorrespondenceGraph correspondences_;
};

// A view of correspondences such that only features who have a run-time type
// of SpecificFeature are ever returned from iterations over it.
template<typename SpecificFeature>
class CorrespondencesView {
 public:
   CorrespondencesView(Correspondences *correspondences)
       : correspondences_(correspondences) {}

  class Iterator {
    friend class CorrespondencesView<SpecificFeature>;
   public:
    ImageID image() const { return iter_.image(); }
    TrackID track() const { return iter_.track(); }
    SpecificFeature *feature() const {
      return static_cast<SpecificFeature *>(iter_.feature());
    }
    bool Done() { return iter_.Done(); }
    void Next() {
      if (!iter_.Done()) {
        iter_.Next();
        SkipOtherFeatures();
      }
    }
   private:
    void SkipOtherFeatures() {
      while (!iter_.Done() &&
             !dynamic_cast<SpecificFeature *>(iter_.feature())) {
        iter_.Next();
      }
    }
    Iterator(Correspondences::Iterator iter)
        : iter_(iter) {
      SkipOtherFeatures();
    }

    Correspondences::Iterator iter_;
  };

  Iterator ScanAllFeatures() {
    return Iterator(correspondences_->ScanAllFeatures());
  }

  Iterator ScanFeaturesForImage(ImageID image) {
    return Iterator(correspondences_->ScanFeaturesForImage(image));
  }

private:
  Correspondences *correspondences_;
};

typedef CorrespondencesView<PointFeature> PointCorrespondences;
typedef CorrespondencesView<LineFeature> LineCorrespondences;

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_CORRESPONDENCE_H_
