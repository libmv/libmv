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

#include <set>

#include "libmv/correspondence/bipartite_graph.h"
#include "libmv/logging/logging.h"
#include "libmv/correspondence/feature.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

class Feature;
class PointFeature;
class LineFeature;

class Matches {
 public:
  typedef int Image;
  typedef int Track;
  typedef BipartiteGraph<int, const Feature *> Graph;

  ~Matches();

  // Iterate over features, silently skiping any that are not FeatureT or
  // derived from FeatureT. 
  template<typename FeatureT>
  class Features {
   public:
    Image           image()    const { return r_.left();  }
    Track           track()    const { return r_.right(); }
    const FeatureT *feature()  const {
      return static_cast<const FeatureT *>(r_.edge());
    }
    operator bool() const { return r_; }
    void operator++() { ++r_; Skip(); }
    Features(Graph::Range range) : r_(range) { Skip(); }

   private:
    void Skip() {
      while (r_ && !dynamic_cast<const FeatureT *>(r_.edge())) ++r_;
    }
    Graph::Range r_;
  };
  typedef Features<PointFeature> Points;
  typedef Features<LineFeature> Lines;

  template<typename T>
  Features<T> All() const { return Features<T>(graph_.All()); }

  template<typename T>
  Features<T> AllReversed() const { return Features<T>(graph_.AllReversed()); }

  template<typename T>
  Features<T> InImage(Image image) const {
    return Features<T>(graph_.ToLeft(image));
  }

  template<typename T>
  Features<T> InTrack(Track track) const {
    return Features<T>(graph_.ToLeft(track));
  }

  // Does not take ownership of feature.
  void Insert(Image image, Track track, const Feature *feature) {
    graph_.Insert(image, track, feature);
    images_.insert(image);
    tracks_.insert(track);
  }

  const Feature *Get(Image image, Track track) const {
    const Feature *const *f = graph_.Edge(image, track);
    return f ? *f : NULL;
  }

  int NumTracks() const { return tracks_.size(); }
  int NumImages() const { return images_.size(); }

 private:
  Graph graph_;
  std::set<Image> images_;
  std::set<Track> tracks_;
};

// TODO(keir): This doesn't belong here. Also, it's broken for non-two views.
inline void PointMatchesAsMatrices(const Matches &c, Mat *x1, Mat *x2) {
  x1->resize(2, c.NumTracks());
  x2->resize(2, c.NumTracks());
  Mat *x[] = {x1, x2};
  int i = 0;
  // TODO(keir): This relies on the ordering; should be more explicit!
  for (Matches::Points r = c.All<PointFeature>(); r; ++r) {
    (*x[r.image()])(0, i) = r.feature()->x();
    (*x[r.image()])(1, i) = r.feature()->y();
    ++r;
    (*x[r.image()])(0, i) = r.feature()->x();
    (*x[r.image()])(1, i) = r.feature()->y();
    ++i;
  }
}

// Delete the features in a correspondences. Uses const_cast to avoid the
// constness problems. This is more intended for tests than for actual use.
void DeleteMatchFeatures(Matches *matches);

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_MATCHES_H_
