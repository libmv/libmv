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


#ifndef UI_TVR_FEATURES_H_
#define UI_TVR_FEATURES_H_

#include <vector>

#include "libmv/correspondence/kdtree.h"

// FIXME(pau): Move this to libmv, or remove it as soon as libmv has something
//             equivalent. Right now, there are two pseudo-prototypes for this
//             in libmv/correspondence. I'm writing a third one here which is
//             clearly not the best solution! So go and fix it soon!
//             My plan is to do something simple here, and then convert it
//             to the datastructures we decide to use (probably the bipartite
//             graph).

struct SurfFeature {
  float x, y, scale, orientation;
  float descriptor[64];
  float operator[](int i) const { return descriptor[i]; }
};

struct SurfFeatureSet {
  std::vector<SurfFeature> features;
  libmv::KdTree<SurfFeature, float> tree;
};

typedef std::pair<size_t, size_t> Match;

// Compute candidate matches between 2 sets of features.  Two features a and b
// are a candidate match if a is the nearest neighbor of b and b is the nearest
// neighbor of a.
void FindCandidateMatches(const SurfFeatureSet &left,
                          const SurfFeatureSet &right,
                          std::vector<Match> *matches);

#endif //UI_TVR_FEATURES_H_
