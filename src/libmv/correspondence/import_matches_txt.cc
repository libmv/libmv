// Copyright (c) 2011 libmv authors.
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

#include <fstream>

#include "libmv/correspondence/import_matches_txt.h"

namespace libmv {

void ImportMatchesFromTxt(const std::string &input_file, 
                          Matches *matches,
                          FeatureSet *feature_set) {
  Matches::ImageID image_id = 0;
  Matches::TrackID track_id = 0;
  float x = 0, y = 0;
  // Reserves 1e6 features to avoid problems during the resize
  feature_set->features.reserve(1e6);
  std::ifstream infile;
  infile.open(input_file.c_str(), std::ios_base::in);
  while (infile.good()) {
    infile >> image_id >> track_id >> x >> y;
    KeypointFeature pf;
    pf.coords << x, y;
    feature_set->features.push_back(pf);
    matches->Insert(image_id, track_id, 
                    &feature_set->features.back());
  }
  infile.close();
}
} // namespace libmv
