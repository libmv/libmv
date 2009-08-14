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

#include <algorithm>
#include <vector>

#include "libmv/base/vector.h"
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


/**
 * Intersect sorted lists. Destroys originals; leaves results as the single
 * entry in sorted_items.
 */ 
template<typename T>
void Intersect(std::vector< std::vector<T> > *sorted_items) {
  std::vector<T> tmp;
  while (sorted_items->size() > 1) {
    int n = sorted_items->size();
    std::vector<T> &s1 = (*sorted_items)[n - 1];
    std::vector<T> &s2 = (*sorted_items)[n - 2];
    tmp.resize(std::min(s1.size(), s2.size()));
    typename std::vector<T>::iterator it = std::set_intersection(
        s1.begin(), s1.end(), s2.begin(), s2.end(), tmp.begin());
    tmp.resize(int(it - tmp.begin()));
    std::swap(tmp, s2);
    tmp.resize(0);
    sorted_items->pop_back();
  }
}

/**
 * Extract matrices from a set of matches, containing the point locations. Only
 * points for tracks which appear in all images are returned in tracks.
 *
 * \param c       The matches from which to extract the points.
 * \param images  Which images to extract the points from.
 * \param xs      The resulting matrices containing the points. The entries will
 *                match the ordering of images.
 */
inline void TracksInAllImages(const Matches &c,
                              const vector<Matches::Image> &images,
                              vector<Matches::Track> *tracks) {
  if (!images.size()) {
    return;
  }
  std::vector<std::vector<Matches::Track> > all_tracks;
  all_tracks.resize(images.size());
  for (int i = 0; i < images.size(); ++i) {
    for (Matches::Points r = c.InImage<PointFeature>(images[i]); r; ++r) {
      all_tracks[i].push_back(r.track());
    }
  }
  Intersect(&all_tracks);
  CHECK(all_tracks.size() == 1);
  for (int i = 0; i < all_tracks[0].size(); ++i) {
    tracks->push_back(all_tracks[0][i]);
  }
}

/**
 * Extract matrices from a set of matches, containing the point locations. Only
 * points for tracks which appear in all images are returned in xs. Each output
 * matrix is of size 2 x N, where N is the number of tracks that are in all the
 * images.
 *
 * \param c       The matches from which to extract the points.
 * \param images  Which images to extract the points from.
 * \param xs      The resulting matrices containing the points. The entries will
 *                match the ordering of images.
 */
inline void PointMatchMatrices(const Matches &c,
                               const vector<Matches::Image> &images,
                               vector<Matches::Track> *tracks,
                               vector<Mat> *xs) {
  TracksInAllImages(c, images, tracks);

  xs->resize(images.size());
  for (int i = 0; i < images.size(); ++i) {
    (*xs)[i].resize(2, tracks->size());
    for (int j = 0; j < tracks->size(); ++j) {
      const PointFeature *f = static_cast<const PointFeature *>(
          c.Get(images[i], (*tracks)[j]));
      (*xs)[i](0, j) = f->x();
      (*xs)[i](1, j) = f->y();
    }
  }
}

// Delete the features in a correspondences. Uses const_cast to avoid the
// constness problems. This is more intended for tests than for actual use.
void DeleteMatchFeatures(Matches *matches);

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_MATCHES_H_
