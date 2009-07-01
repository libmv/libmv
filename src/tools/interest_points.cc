// Copyright (c) 2007, 2008, 2009 libmv authors.
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

#include <algorithm>
#include <string>

#include "libmv/base/vector.h"
#include "libmv/correspondence/klt.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/surf.h"
#include "libmv/image/surf_descriptor.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

int main(int argc, char **argv) {
  libmv::Init("track a sequence", &argc, &argv);

  if (argc != 2) {
    LOG(ERROR) << "Missing an image.";
    return 1;
  }

  string filename = argv[1];

  Array3Df image;
  if (!ReadPnm(filename.c_str(), &image)) {
    LOG(FATAL) << "Failed loading image: " << filename;
    return 0;
  }

  libmv::vector<libmv::SurfFeature> features;
  libmv::SurfFeatures(image, 4, 4, &features);
  // TODO(keir): Do something with the extracted features.
  LOG(FATAL) << "Unimplemented!";
  return 0;
}
