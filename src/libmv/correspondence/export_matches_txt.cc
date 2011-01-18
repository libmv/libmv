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

#include "libmv/correspondence/export_matches_txt.h"

namespace libmv {

void ExportMatchesToTxt(const Matches &matches, std::string out_file_name) {
  std::ofstream outfile;
  outfile.open(out_file_name.c_str(), std::ios_base::out);
  if (outfile.is_open()) {
    std::set<Matches::ImageID>::const_iterator iter_image =
        matches.get_images().begin();
    for (; iter_image != matches.get_images().end(); ++iter_image) {
      // Exports points
      Matches::Features<PointFeature> pfeatures =
          matches.InImage<PointFeature>(*iter_image);
      while(pfeatures) {
        outfile << *iter_image << " " << pfeatures.track() << " " 
                << pfeatures.feature()->x() << " "
                << pfeatures.feature()->y() << std::endl;
        pfeatures.operator++();
      }
    }
    outfile.close();
  }
}
} // namespace libmv
