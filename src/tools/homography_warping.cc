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

#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/surf_detector.h"
#include "libmv/detector/detector.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/simpliest_descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"
#include "libmv/image/image.h"
#include "libmv/image/image_converter.h"
#include "libmv/image/image_drawing.h"
#include "libmv/image/image_io.h"
#include "libmv/image/sample.h"
#include "libmv/multiview/homography_kernel.h"
#include "libmv/multiview/robust_homography.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

void usage() {
  LOG(ERROR) << " points_detector ImageNameA.pgm ImageNameB.pgm ImageNameOut.pgm"
    <<std::endl
    << " ImageNameA.pgm  : the input image that you want stitch to B,"
    << std::endl
    << " ImageNameB.pgm  : the input image that you want stitch to A,"
    << " ImageNameOut.pgm : the localized keypoints will be displayed on it."
    << std::endl
    << " INFO : work with pgm image only." << std::endl;
}


int main(int argc, char **argv) {

  libmv::Init("Stitch together two images", &argc, &argv);

  if (argc != 4 ) {
    usage();
    LOG(ERROR) << "Missing parameters or errors in the command line.";
    return 1;
  }

  // Parse input parameter
  const string sImageA = argv[1];
  const string sImageB = argv[2];
  const string sImageOut = argv[3];
  //TODO(pmoulon) Add FAST/SURF points localisation method as a parameter

  Array3Du imageA;
  if (!ReadImage(sImageA.c_str(), &imageA)) {
    LOG(FATAL) << "Failed loading image: " << sImageA;
    return 0;
  }
  Array3Du imageB;
  if (!ReadImage(sImageB.c_str(), &imageB)) {
    LOG(FATAL) << "Failed loading image: " << sImageB;
    return 0;
  }

  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 30));
  //scoped_ptr<detector::Detector> detector(detector::CreateSURFDetector(4,4));

  FeatureSet KeypointImgA;
  FeatureSet KeypointImgB;
  {
    Array3Du imageTemp;
    Rgb2Gray( imageA, &imageTemp);
    libmv::vector<libmv::Feature *> features;
    Image im( new Array3Du(imageTemp) );
    detector->Detect( im, &features, NULL);

    libmv::vector<descriptor::Descriptor *> descriptors;
    scoped_ptr<descriptor::Describer> describer(descriptor::CreateSimpliestDescriber());
    describer->Describe(features, im, NULL, &descriptors);

    // Copy data.
    KeypointImgA.features.resize(descriptors.size());
    for(int i = 0;i < descriptors.size(); ++i)
    {
      KeypointFeature & feat = KeypointImgA.features[i];
      feat.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
      *(PointFeature*)(&feat) = *(PointFeature*)features[i];
    }

    DestroyDynamicDataArray( features );
    DestroyDynamicDataArray( descriptors );

  }

  {
    Array3Du imageTemp;
    Rgb2Gray( imageB, &imageTemp);
    libmv::vector<libmv::Feature *> features;
    Image im( new Array3Du(imageTemp) );
    detector->Detect( im, &features, NULL);

    libmv::vector<descriptor::Descriptor *> descriptors;
    scoped_ptr<descriptor::Describer> describer(descriptor::CreateSimpliestDescriber());
    describer->Describe(features, im, NULL, &descriptors);

    // Copy data.
    KeypointImgB.features.resize(descriptors.size());
    for(int i = 0;i < descriptors.size(); ++i)
    {
      KeypointFeature & feat = KeypointImgB.features[i];
      feat.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
      *(PointFeature*)(&feat) = *(PointFeature*)features[i];
    }

    DestroyDynamicDataArray( features );
    DestroyDynamicDataArray( descriptors );

  }

  if(KeypointImgA.features.size() > 0)  {
    // Build the kd-tree.
    KeypointImgA.tree.SetDimensions(KeypointImgA.features[0].descriptor.coords.size());
    for (int i = 0; i < KeypointImgA.features.size(); ++i) {
      KeypointImgA.tree.AddPoint(KeypointImgA.features[i].descriptor.coords.data(), i);
    }
    KeypointImgA.tree.Build(10);
  }

  if(KeypointImgB.features.size() > 0)  {
    // Build the kd-tree.
    KeypointImgB.tree.SetDimensions(KeypointImgB.features[0].descriptor.coords.size());
    for (int i = 0; i < KeypointImgB.features.size(); ++i) {
      KeypointImgB.tree.AddPoint(KeypointImgB.features[i].descriptor.coords.data(), i);
    }
    KeypointImgB.tree.Build(10);
  }

  Matches matches;
  FindCandidateMatches(KeypointImgA,
                       KeypointImgB,
                       &matches);

  libmv::vector<Mat> x;
  libmv::vector<int> tracks;
  libmv::vector<int> images;
  images.push_back(0);
  images.push_back(1);
  PointMatchMatrices(matches, images, &tracks, &x);

  //robust estimation of the homography matrix
  // Compute Homography matrix and inliers.
  libmv::vector<int> inliers;
  // TODO(pau) Expose the threshold.
  Mat3 H;
  HomographyFromCorrespondences4PointRobust(x[0], x[1], 1, &H, &inliers);
  std::cout<< inliers.size() << " inliers" << std::endl;
  if (inliers.size() < 6) {
    return -1;
  }

  Matches consistent_matches;
  // Build new correspondence graph containing only inliers.
  for (int j = 0; j < inliers.size(); ++j) {
    int k = inliers[j];
    for (int i = 0; i < 2; ++i) {
      consistent_matches.Insert(images[i], tracks[k],
          matches.Get(images[i], tracks[k]));
    }
  }
  
  // Compute Fundamental matrix using all inliers.
  TwoViewPointMatchMatrices(consistent_matches, 0, 1, &x);
  libmv::vector<Mat3> Hs;
  homography::kernel::FourPointSolver::Solve(x[0], x[1], &Hs);
  H = Hs[0];
  LOG(INFO) << "H:\n" << H << std::endl;

  //warp imageB on ImageA.

  Mat3 Hinv = H.inverse();
  for(int j=0; j < imageA.Height(); ++j)
    for(int i=0; i < imageA.Width(); ++i)
    {
      Vec3 Pos;
      Pos << i,j,1;
      Vec3 Pos2 = H * Pos;
      Vec3 posImage2 = Pos2/Pos2(2);
      if( imageA.Contains( posImage2(1), posImage2(0) ) )
      {
        imageA(j,i,0) = (imageA(j,i,0) + SampleLinear( imageB, posImage2(1),posImage2(0),0))/2;
        imageA(j,i,1) = (imageA(j,i,1) + SampleLinear( imageB, posImage2(1),posImage2(0),1))/2;
        imageA(j,i,2) = (imageA(j,i,2) + SampleLinear( imageB, posImage2(1),posImage2(0),2))/2;
      }
    }

  WriteJpg(imageA, string(sImageOut).c_str(), 100);

  return 0;
}
