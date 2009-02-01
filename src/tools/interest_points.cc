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
#include <vector>

#include "libmv/correspondence/klt.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/surf.h"
#include "libmv/image/surf_descriptor.h"
#include "third_party/gflags/gflags.h"
#include "third_party/glog/src/glog/logging.h"

using namespace libmv;
using namespace std;

// TODO(keir): Replace this with Cairo.
// TODO(keir): Remove the dup vs klt.cc.
template<typename TVec2>
void DrawFeature(const TVec2 &point,
                 const Vec3 &color,
                 Array3Df *image) {
  assert(image->Depth() == 3);

  //const int cross_width = lround(9*point(0)/1.2);
  const int cross_width = lround(point(0));
  int y = lround(point(1));
  int x = lround(point(2));
  if (!image->Contains(y,x)) {
    return;
  }

  // Draw vertical line.
  for (int i = max(0, y - cross_width);
       i < min(image->Height(), y + cross_width + 1); ++i) {
    for (int k = 0; k < 3; ++k) {
      (*image)(i, x, k) = color(k);
    }
  }
  // Draw horizontal line.
  for (int j = max(0, x - cross_width);
       j < min(image->Width(), x + cross_width + 1); ++j) {
    for (int k = 0; k < 3; ++k) {
      (*image)(y, j, k) = color(k);
    }
  }
}

void WriteOutputImage(const FloatImage &image,
                      const std::vector<Vec3f> &points,
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
  green << 0, 1, 0;
  for (int i = 0; i < points.size(); ++i) {
    DrawFeature(points[i], green, &output_image);
  }
  WritePnm(output_image, output_filename);
}

int main(int argc, char **argv) {
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

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

  std::vector<Vec3f> features;
  MultiscaleDetectFeatures(image, 4, 4, &features);
  WriteOutputImage(image, features, (filename + ".out.ppm").c_str());
  LOG(INFO) << "Extracting feature descriptors.";
  Matf integral_image;
  IntegralImage(image, &integral_image);
  LOG(INFO) << "Extracting feature descriptors.";
  for (int i = 0; i < features.size(); ++i) {
    Matrix<float, 64, 1> descriptor;
    float scale = features[i](0);
    float y = features[i](1);
    float x = features[i](2);
    LOG(INFO) << "x = " << x << " y = " << y << " scale = " << scale;
    USURFDescriptor<4, 5>(integral_image,
                          features[i](2),
                          features[i](1),
                          features[i](0),
                          &descriptor);
    VLOG(1) << "Descriptor: " << descriptor.transpose();
  }
  return 0;
}


