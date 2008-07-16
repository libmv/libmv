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

#include <algorithm>
#include <string>
#include <vector>

//#include "libmv/correspondence/correspondence.h"
#include "libmv/correspondence/klt.h"
#include "libmv/image/image.h"
#include "libmv/image/image_pyramid.h"
#include "third_party/gflags/gflags.h"

using std::string;
using std::sort;
using std::vector;
using libmv::ByteImage;
using libmv::ImagePyramid;
using libmv::FloatImage;

DEFINE_int32(first_index, 0, "Index of the first image.");
DEFINE_int32(last_index, -1, "Index of the last image.  Use -1 to autodetect it.");

int main(int argc, char **argv) {
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);

  // This is not the place for this. I am experimenting with what sort of API
  // will be convenient for the tracking base classes.
  vector<string> files;
  for (int i = 0; i < argc; ++i) {
    files.push_back(argv[i]);
  }
  sort(files.begin(), files.end());

  if (files.size() < 2) {
    printf("Not enough files.\n");
    return 1;
  }

  ByteImage byte_image;
  FloatImage image1;
  FloatImage image2;
  ImagePyramid pyramid1(image1, 3);
  ImagePyramid pyramid2;

  for (size_t i = 0; i < files.size(); ++i) {
    // TODO(keir): Finish me.
  }

  return 0;
}


