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

#include "libmv/base/vector.h"
#include "libmv/correspondence/kdtree.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/matches.h"
#include "libmv/image/surf.h"

struct SurfFeatureSet {
  libmv::vector<libmv::SurfFeature> features;
  libmv::KdTree<libmv::SurfFeature, float> tree;
};

// Compute candidate matches between 2 sets of features.  Two features a and b
// are a candidate match if a is the nearest neighbor of b and b is the nearest
// neighbor of a.
void FindCandidateMatches(const SurfFeatureSet &left,
                          const SurfFeatureSet &right,
                          libmv::Matches *matches);

// Computes the fundamental matrix given a set of candidate correspondences.
// Outputs the fundamental matrix and the set of inlier correspondences.
void ComputeFundamental(libmv::Matches &candidate,
                        libmv::Mat3 *F,
                        libmv::Matches *robust);

#endif //UI_TVR_FEATURES_H_
