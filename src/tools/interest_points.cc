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
#include "libmv/image/image_drawing.h"
#include "libmv/image/surf.h"
#include "libmv/image/surf_descriptor.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

void usage() {
  LOG(ERROR) << " interest_points ImageNameIn.pgm ImageNameOut.pgm " <<std::endl
    << " ImageNameIn.pgm  : the input image on which surf features will be extrated, " << std::endl
    << " ImageNameOut.pgm : the surf keypoints will be displayed on it. " << std::endl
    << " INFO : work with pgm image only." << std::endl;
}
void DrawSurfFeatures( Array3Df & im, const libmv::vector<libmv::SurfFeature> & feat);

int main(int argc, char **argv) {
  libmv::Init("Extract surf feature from an image", &argc, &argv);

  if (argc != 3 || !(GetFormat(argv[1])==Pnm && GetFormat(argv[2])==Pnm)) {
    usage();
    LOG(ERROR) << "Missing parameters or errors in the command line.";
    return 1;
  }

  // Parse input parameter
  const string sImageIn = argv[1];
  const string sImageOut = argv[2];

  Array3Du imageIn;
  if (!ReadPnm(sImageIn.c_str(), &imageIn)) {
    LOG(FATAL) << "Failed loading image: " << sImageIn;
    return 0;
  }

  Array3Df image;
  ByteArrayToScaledFloatArray(imageIn, &image);
  // TODO(pmoulon) Assert that the image value is within [0.f;255.f]
  for(int j=0; j < image.Height(); ++j)
  for(int i=0; i < image.Width(); ++i)
    image(j,i) *=255;

  libmv::vector<libmv::SurfFeature> features;
  libmv::SurfFeatures(image, 4, 4, &features);

  DrawSurfFeatures(image, features);
  if (!WritePnm(image, sImageOut.c_str())) {
    LOG(FATAL) << "Failed saving output image: " << sImageOut;
  }

  return 0;
}

void DrawSurfFeatures( Array3Df & im, const libmv::vector<libmv::SurfFeature> & feat)
{
  std::cout << feat.size() << " Detected points " <<std::endl;
  for (int i = 0; i < feat.size(); ++i)
  {
    const libmv::SurfFeature & feature = feat[i];
    const int x = feature.x();
    const int y = feature.y();
    const float scale = 2*feature.scale;
    //std::cout << i << " " << x << " " << y << " " << feature.getScale() <<std::endl;

    DrawCircle(x, y, scale, (unsigned char)255, &im);
    const float angle = feature.orientation;
    DrawLine(x, y, x + scale * cos(angle), y + scale*sin(angle),
             (unsigned char) 255, &im);
  }
}


