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

#ifndef LIBMV_CORRESPONDENCE_MATCHES_H_
#define LIBMV_CORRESPONDENCE_MATCHES_H_

#include <cstdio>

#include "libmv/correspondence/bipartite_graph_new.h"
#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

class Feature;
class PointFeature;
class LineFeature;

class Matches {
 friend class TracksInImagesIterator;
 public:
  template<typename FeatureT>
  class TracksInImagesIterator;

   // TODO(keir): Decide if simple numbers are good enough.
  typedef int Image;
  typedef int Track;
  typedef BipartiteGraph<Image, const Feature *, Track> Graph;

  ~Matches();

  // When iterating over features, silently skip any that are not FeatureT or
  // derive from FeatureT. Note that the duplication of this class and
  // ImageIterator compared to Track*Iterator is only necessary because C++
  // lacks template typedefs, *sigh*.
  template<typename FeatureT>
  class ImageFeatureIterator {
    friend class Matches;
   public:
    typedef typename Graph::LeftEdgeIterator Iterator;
    Image           image()    const { return iter_.left();  }
    Track           track()    const { return iter_.right(); }
    const FeatureT *feature()  const {
      return static_cast<const FeatureT *>(iter_.edge());
    }
    bool operator!=(const ImageFeatureIterator<FeatureT> &it) const {
      return iter_ != it.iter_;
    }
    ImageFeatureIterator<FeatureT> &operator++() {
      ++iter_;
      Skip<FeatureT>(&iter_, end_);
      return *this;
    }
   private:
    ImageFeatureIterator(Iterator iter, Iterator end)
        : iter_(iter), end_(end) {
      Skip<FeatureT>(&iter_, end_);
    }
    Iterator iter_;
    Iterator end_;
  };

  class ImageIterator {
    friend class Matches;
    typedef Graph::LeftIterator Iterator;
   public:
    Image operator*()  const { return *iter_; }
    void operator++()       { ++iter_; }
    bool operator!=(const ImageIterator &it) const {
      return iter_ != it.iter_;
    }
    template<typename FeatureT>
    ImageFeatureIterator<FeatureT> begin() const {
      return ImageFeatureIterator<FeatureT>(iter_.begin(), iter_.end());
    }
    template<typename FeatureT>
    ImageFeatureIterator<FeatureT> end() const {
      return ImageFeatureIterator<FeatureT>(iter_.end(), iter_.end());
    }
   private:
    ImageIterator(Iterator it) : iter_(it) {}
    Iterator iter_;
  };

  ImageIterator ImageBegin() const { return ImageIterator(graph_.LeftBegin()); }
  ImageIterator ImageEnd()   const { return ImageIterator(graph_.LeftEnd());   }

  // Same as TrackFeatureIterator but for iterating over the features in a
  // track.
  template<typename FeatureT>
  class TrackFeatureIterator {
    friend class Matches;
   public:
    typedef typename Graph::RightEdgeIterator Iterator;
    Track           image()    const { return iter_.left();  }
    Track           track()    const { return iter_.right(); }
    const FeatureT *feature()  const {
      return static_cast<const FeatureT *>(iter_.edge());
    }
    bool operator!=(const TrackFeatureIterator<FeatureT> &it) const {
      return iter_ != it.iter_;
    }
    TrackFeatureIterator<FeatureT> &operator++() {
      ++iter_;
      Skip<FeatureT>(&iter_, end_);
      return *this;
    }
   private:
    TrackFeatureIterator(Iterator iter, Iterator end)
        : iter_(iter), end_(end) {
      Skip<FeatureT>(&iter_, end_);
    }
    Iterator iter_;
    Iterator end_;
  };

  class TrackIterator {
    friend class Matches;
    typedef Graph::RightIterator Iterator;
   public:
    Track operator*()  const { return *iter_; }
    void operator++()        { ++iter_; }
    bool operator!=(const TrackIterator &it) const {
      return iter_ != it.iter_;
    }
    template<typename FeatureT>
    TrackFeatureIterator<FeatureT> begin() const {
      return TrackFeatureIterator<FeatureT>(iter_.begin(), iter_.end());
    }
    template<typename FeatureT>
    TrackFeatureIterator<FeatureT> end() const {
      return TrackFeatureIterator<FeatureT>(iter_.end(), iter_.end());
    }
   private:
    TrackIterator(Iterator it) : iter_(it) {}
    Iterator iter_;
  };

  TrackIterator TrackBegin() const { return TrackIterator(graph_.RightBegin());}
  TrackIterator TrackEnd()   const { return TrackIterator(graph_.RightEnd());  }

  int NumFeatures() const { return graph_.NumEdges(); }
  int NumImages()   const { return graph_.NumLeftNodes(); }
  int NumTracks()   const { return graph_.NumRightNodes(); }

  // Does not take ownership of feature, which must remain valid for the life
  // of the correspondences.
  void Insert(Image image, Track track, const Feature *feature) {
    graph_.Insert(image, feature, track);
  }

  template<typename FeatureT>
  class TracksInImagesFeatureIterator;

  template<typename FeatureT>
  class TracksInImagesIterator {
   friend class Matches;
   public:
    Track operator*()  const { return *iter_; }
    bool operator!=(const TracksInImagesIterator &other) {
      // FIXME: Dirty hack alert.
      (void) other;
      return iter_ != matches_.TrackEnd();
    }
    void operator++() {
      ++iter_;
      Skip();
    }
    TracksInImagesFeatureIterator<FeatureT> begin() const {
      return TracksInImagesFeatureIterator<FeatureT>(iter_.begin<FeatureT>(),
                                                     iter_.end<FeatureT>(),
                                                     images_);
    }
    TracksInImagesFeatureIterator<FeatureT> end() const {
      return TracksInImagesFeatureIterator<FeatureT>(iter_.end<FeatureT>(),
                                                     iter_.end<FeatureT>(),
                                                     images_);
    }
   private:
    TracksInImagesIterator(const Matches &matches,
                           const std::set<Image> &images)
        : matches_(matches), images_(images), iter_(matches.TrackBegin()) {
      Skip();
    }
    void Skip() {
      while (iter_ != matches_.TrackEnd() &&
             !TrackIsInAllImages(*iter_))
        ++iter_;
    }
    bool TrackIsInAllImages(Track track) {
      LOG(INFO) << "Checking track: " << track;
      LOG(INFO) << "Images.size(): " << images_.size();
      for (std::set<Image>::const_iterator it = images_.begin();
           it != images_.end(); ++it) {
        // TODO(keir): Expose enough of the bipartite graph to do this faster.
        // A better way would be to cache the track -> images map and only hit
        // that, rather than hitting the image map for each image.
        const Feature *feature;
        if (!matches_.graph_.GetEdge(*it, track, &feature)) {
          return false;
        }
        // Only take the advertised feature type.
        if (!dynamic_cast<const FeatureT *>(feature)) {
          return false;
        }
      }
      return true;
    }
    const Matches &matches_;
    const std::set<Image> &images_;
    TrackIterator iter_;
  };

  template<typename FeatureT>
  TracksInImagesIterator<FeatureT> TracksInImagesBegin(
      const std::set<Image> &images) const {
    return TracksInImagesIterator<FeatureT>(*this, images);
  }

  template<typename FeatureT>
  TracksInImagesIterator<FeatureT> TracksInImagesEnd(
      const std::set<Image> &images) const {
    return TracksInImagesIterator<FeatureT>(*this, images);
  }

  // Same as TrackFeatureIterator but for iterating over the features in a
  // track.
  template<typename FeatureT>
  class TracksInImagesFeatureIterator {
    friend class TracksInImagesIterator<FeatureT>;
   public:
    typedef TrackFeatureIterator<FeatureT> Iterator;
    Track           image()    const { return iter_.image();   }
    Track           track()    const { return iter_.track();   }
    const FeatureT *feature()  const { return iter_.feature(); }
    bool operator!=(const TracksInImagesFeatureIterator<FeatureT> &it) const {
      return iter_ != it.iter_;
    }
    TracksInImagesFeatureIterator<FeatureT> &operator++() {
      ++iter_;
      Skip();
      return *this;
    }
   private:
    TracksInImagesFeatureIterator(Iterator iter,
                                  Iterator end,
                                  const std::set<Image> &images)
        : iter_(iter), end_(end), images_(images) {
      Skip();
    }
    void Skip() {
      while (iter_ != end_ && !images_.count(iter_.image())) {
        ++iter_;
      }
    }
    Iterator iter_;
    Iterator end_;
    const std::set<Image> &images_;
  };

  // TODO(keir): Add ImagesInTracksIterator.
  // TODO(keir): Add ImagesInTracksFeatureIterator.

 private:
  template<typename FeatureT, typename IteratorT>
  static void Skip(IteratorT *iter, const IteratorT &end) {
    while (*iter != end && !dynamic_cast<const FeatureT *>(iter->edge())) {
      ++(*iter);
    }
  }
  Graph graph_;
};

// Delete the features in a correspondences. Uses const_cast to avoid the
// constness problems. This is more intended for tests than for actual use.
void DeleteMatchFeatures(Matches *matches);

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_MATCHES_H_
