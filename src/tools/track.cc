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
#include "libmv/image/image_sequence_io.h"
#include "libmv/image/cached_image_sequence.h"
#include "libmv/image/pyramid_sequence.h"
#include "third_party/gflags/gflags.h"

DEFINE_bool(debug_images, true, "Output debug images.");
DEFINE_double(sigma, 0.9, "Blur filter strength.");
DEFINE_int32(pyramid_levels, 4, "Number of levels in the image pyramid.");

using namespace libmv;

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

  ImageCache cache;
  ImageSequence *source = ImageSequenceFromFiles(files, &cache);
  PyramidSequence *pyramid_sequence =
      MakePyramidSequence(source, FLAGS_pyramid_levels, FLAGS_sigma);

  KLTContext klt;
  KLTContext::FeatureList features[2];

  int oldind = 0, newind = 1;

  // TODO(keir): Really have to get a scoped_ptr<> implementation!
  // Consider taking the one from boost but editing out the cruft.
  ImagePyramid *pyramid = pyramid_sequence->Pyramid(0);
  klt.DetectGoodFeatures(pyramid->Level(0), &features[oldind]);
  WriteOutputImage(pyramid->Level(0), klt, features[oldind],
                   (files[0]+".out.ppm").c_str());

  // TODO(keir): Use correspondences here!
  for (size_t i = 1; i < files.size(); ++i) {
    printf("Tracking %2d features in %s\n",
           features[oldind].size(),
           files[i].c_str());

    klt.TrackFeatures(pyramid_sequence->Pyramid(i-1),  features[oldind],
                      pyramid_sequence->Pyramid(i),   &features[newind]);

    printf("...now have %2d features.\n", features[newind].size());

    WriteOutputImage(
        pyramid_sequence->Pyramid(i)->Level(0),
        klt, features[newind], (files[i]+".out.ppm").c_str());

    std::swap(oldind, newind);
  }
  return 0;
}


