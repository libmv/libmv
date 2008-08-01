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

#include "libmv/correspondence/correspondence.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/klt.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_pyramid.h"
#include "third_party/gflags/gflags.h"

DEFINE_bool(debug_images, true, "Output debug images.");

using libmv::Array3Df;
using libmv::FloatImage;
using libmv::ImagePyramid;
using libmv::KLTContext;
using libmv::Vec3;
using std::sort;
using std::string;
using std::vector;

void WriteOutputImage(const FloatImage &image,
                      const KLTContext &klt,
                      const KLTContext::FeatureList &features,
                      const char *output_filename) {
  FloatImage output_image(image.Height(), image.Width(), 3);
  for (int i = 0; i < image.Height(); ++i) {
    for (int j = 0; j < image.Width(); ++j) {
      output_image(i,j,0) = image(i,j);
      output_image(i,j,1) = image(i,j);
      output_image(i,j,2) = image(i,j);
    }
  }

  Vec3 green;
  green = 0, 1, 0;
  klt.DrawFeatureList(features, green, &output_image);

  WritePnm(output_image, output_filename);
}

int main(int argc, char **argv) {
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);

  // TODO(keir): Resurrect this! Need to use new image pyramids.
  
  /*
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

  int oldind = 0, newind = 1;
  Array3Df image[2];
  ImagePyramid *pyramid[2];
  KLTContext::FeatureList features[2];
  KLTContext klt;

  ReadPnm(files[0].c_str(), &image[newind]);
  pyramid[newind].Init(image[newind], 3);

  klt.DetectGoodFeatures(pyramid[newind].Level(0), &features[newind]);

  WriteOutputImage(image[newind], klt, features[newind],
                   (files[0]+".out.ppm").c_str());

  // TODO(keir): Use correspondences here!
  for (size_t i = 1; i < files.size(); ++i) {
    std::swap(oldind, newind);

    printf("Tracking %s\n", files[i].c_str());

    ReadPnm(files[i].c_str(), &image[newind]);
    pyramid[newind].Init(image[newind], 3);

    klt.TrackFeatures(pyramid[oldind], features[oldind],
                      pyramid[newind], &features[newind]);

    WriteOutputImage(image[newind], klt, features[newind],
                    (files[i]+".out.ppm").c_str());

    // TODO(keir): Finish me.
  }
  */

  return 0;
}


