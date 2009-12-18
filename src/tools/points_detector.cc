// Copyright (c) 2009 libmv authors.
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
#include <time.h>

#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/correspondence/feature.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/simpliest_descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/star_detector.h"
#include "libmv/detector/surf_detector.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_drawing.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

void usage() {
  LOG(ERROR) << " points_detector ImageNameIn.pgm ImageNameOut.pgm " <<std::endl
    << " ImageNameIn.pgm  : the input image on which surf features will be extrated, " << std::endl
    << " ImageNameOut.pgm : the localized keypoints will be displayed on it. " << std::endl
    << " INFO : work with pgm image only." << std::endl;
}

/// Draw feature position, scale and orientation over a given image.
template <typename Image>
void DrawFeatures( Image & im, const libmv::vector<libmv::Feature *> & feat);

/// Export descriptor data as image.
void SaveDescriptorAsPatches( const libmv::vector<descriptor::Descriptor *> & desc);


int main(int argc, char **argv) {

  libmv::Init("Extract points feature from an image", &argc, &argv);

  if (argc != 3 || !(GetFormat(argv[1])==Pnm && GetFormat(argv[2])==Pnm)) {
    usage();
    LOG(ERROR) << "Missing parameters or errors in the command line.";
    return 1;
  }

  // Parse input parameter
  const string sImageIn = argv[1];
  const string sImageOut = argv[2];
  //TODO(pmoulon) Add FAST/SURF points localisation method as a parameter

  Array3Du imageIn;
  if (!ReadPnm(sImageIn.c_str(), &imageIn)) {
    LOG(FATAL) << "Failed loading image: " << sImageIn;
    return 0;
  }

  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 30));
  //scoped_ptr<detector::Detector> detector(detector::CreateStarDetector());
  //scoped_ptr<detector::Detector> detector(detector::CreateSURFDetector(4,4));

  libmv::vector<libmv::Feature *> features;
  Image im( new Array3Du(imageIn) );

  clock_t startTime = clock();

  detector->Detect( im, &features, NULL);

  std::cout << " Keypoint localization time : " << clock() - startTime << " clocks " << endl;
  std::cout << " Keypoint localization time : " << (float)(clock() - startTime) / CLOCKS_PER_SEC << "s"<< endl;

  bool bExportDescToDisk = false;
  if( bExportDescToDisk )
  {
    libmv::vector<descriptor::Descriptor *> descriptors;
    scoped_ptr<descriptor::Describer> descriptorInterface(descriptor::CreateSimpliestDescriber());
    descriptorInterface->Describe(features, im, NULL, &descriptors);

    SaveDescriptorAsPatches(descriptors);
  }


  DrawFeatures( imageIn, features);
  if (!WritePnm(imageIn, sImageOut.c_str())) {
    LOG(FATAL) << "Failed saving output image: " << sImageOut;
  }

  DeleteElements(&features);

  return 0;
}


template <typename Image>
void DrawFeatures( Image & im, const libmv::vector<libmv::Feature *> & feat)
{
  std::cout << feat.size() << " Detected points " <<std::endl;
  for (int i = 0; i < feat.size(); ++i)
  {
    const libmv::PointFeature * feature = dynamic_cast<libmv::PointFeature *>( feat[i] );
    const int x = feature->x();
    const int y = feature->y();
    const float scale = feature->scale;

    DrawCircle(x, y, scale, (unsigned char)255, &im);
    const float angle = feature->orientation;
    DrawLine(x, y, x + scale * cos(angle), y + scale *sin(angle),
             (unsigned char) 255, &im);
  }
}

void SaveDescriptorAsPatches( const libmv::vector<descriptor::Descriptor *> & desc)
{
  for (int i = 0; i < desc.size(); ++i) {
    const descriptor::VecfDescriptor * array = dynamic_cast<descriptor::VecfDescriptor *>( desc[i] );
    if(array) {
      const int size = sqrt((double)(array->coords.size()));
      Array3Df ima(size,size);
      for(int j=0; j< size; ++j)
      for(int i=0; i< size; ++i)
      {
        ima(j, i) = array->coords(j*size + i);
      }
      //-- Save the patch as a image
      ostringstream os;
      os << "./Patch_000" << i << ".jpg";
      WriteJpg(ima, os.str().c_str(), 100);
    }
  }
}



