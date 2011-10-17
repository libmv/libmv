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

#ifndef LIBMV_CORRESPONDENCE_IMPORT_MATCHES_TXT_H_
#define LIBMV_CORRESPONDENCE_IMPORT_MATCHES_TXT_H_

#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/matches.h"

namespace libmv {
// Imports the matches from a TXT format file
// Each line corresponds to a correspondence. 
// The output format of point features is:
// <ImageID> <TrackID> <x> <y>
void ImportMatchesFromTxt(const std::string &input_file, 
                          Matches *matches,
                          KeypointFeatureSet *feature_set);
}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_IMPORT_MATCHES_TXT_H_
