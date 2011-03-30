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
#include <iostream>
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
#include "libmv/multiview/affine_2d_kernel.h"
#include "libmv/multiview/homography_kernel.h"
#include "libmv/multiview/panography_kernel.h"
#include "libmv/multiview/robust_homography.h"
#include "libmv/multiview/robust_affine_2d.h"
#include "libmv/multiview/two_view_kernel.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

enum eGEOMETRIC_CONSTRAINT  {
  AFFINE = 0, // Used for planar stitching (infinite focal).
  HOMOGRAPHY = 1, // general 4 Point solution for computing Homography.
  MINIMAL_PANORAMIC_CASE = 2 // 2 Point solution for shared nodal point.
};

void usage() {
  LOG(ERROR) << " points_detector ImageNameA ImageNameB ImageNameOut.jpg"
    << std::endl
    << " ImageNameA  : the input image that you want stitch to B,"
    << std::endl
    << " ImageNameB  : the input image that you want stitch to A,"
    << " ImageNameOut.jpg : the localized keypoints will be displayed on it."
    << std::endl
    << " INFO : 2Point algorithm work with image with a sufficient angle \
        between view." << std::endl;
}

void Overlap_ComputeBoundingBox(int w1, int h1, int w2, int h2,
                                const Mat3 & Homography,
                                int & tx, int & ty, int & wOut, int & hOut);

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

  eGEOMETRIC_CONSTRAINT eGeometricConstraint = //MINIMAL_PANORAMIC_CASE;
                                               HOMOGRAPHY;
                                               //AFFINE;

  scoped_ptr<detector::Detector> detector(detector::CreateFastDetector(9, 20));
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
    scoped_ptr<descriptor::Describer>
      describer(descriptor::CreateSimpliestDescriber());
    describer->Describe(features, im, NULL, &descriptors);

    // Copy data.
    KeypointImgA.features.resize(descriptors.size());
    for(int i = 0;i < descriptors.size(); ++i)
    {
      KeypointFeature & feat = KeypointImgA.features[i];
      feat.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
      *(PointFeature*)(&feat) = *(PointFeature*)features[i];
    }

    DeleteElements(&features);
    DeleteElements(&descriptors);

  }

  {
    Array3Du imageTemp;
    Rgb2Gray( imageB, &imageTemp);
    libmv::vector<libmv::Feature *> features;
    Image im( new Array3Du(imageTemp) );
    detector->Detect( im, &features, NULL);

    libmv::vector<descriptor::Descriptor *> descriptors;
    scoped_ptr<descriptor::Describer>
      describer(descriptor::CreateSimpliestDescriber());
    describer->Describe(features, im, NULL, &descriptors);

    // Copy data.
    KeypointImgB.features.resize(descriptors.size());
    for(int i = 0;i < descriptors.size(); ++i)
    {
      KeypointFeature & feat = KeypointImgB.features[i];
      feat.descriptor = *(descriptor::VecfDescriptor*)descriptors[i];
      *(PointFeature*)(&feat) = *(PointFeature*)features[i];
    }

    DeleteElements(&features);
    DeleteElements(&descriptors);
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

  // Robust estimation of the Transformation matrix
  // Compute Homography matrix and inliers.
  libmv::vector<int> inliers;
  Mat3 H;

  switch( eGeometricConstraint )
  {
    case AFFINE :
      AffineFromCorrespondences2PointRobust(x[0], x[1], 1, &H, &inliers);
    break;
    case HOMOGRAPHY :
      HomographyFromCorrespondences4PointRobust(x[0], x[1], 1, &H, &inliers);
    break;
    case MINIMAL_PANORAMIC_CASE:
      HomographyFromCorrespondences2PointRobust(x[0], x[1], 1, &H, &inliers);
    break;

  }

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

  // Compute Homography matrix using all inliers.
  TwoViewPointMatchMatrices(consistent_matches, 0, 1, &x);
  libmv::vector<Mat3> Hs;

  switch( eGeometricConstraint )
  {
    case AFFINE :
      affine2D::kernel::TwoPointSolver::Solve(x[0], x[1], &Hs);
      if (Hs.size() == 0) {
        // Todo(pmoulon) call the normalized solver
        Hs.push_back(H); // Use old solution
      }
    break;
    case HOMOGRAPHY :
      homography::kernel::FourPointSolver::Solve(x[0], x[1], &Hs);
    break;
    case MINIMAL_PANORAMIC_CASE:
      Hs.push_back(H);
      std::cout << "Todo : write down focal solving from N view" <<std::endl;
      //panography::kernel::TwoPointSolver::Solve(x[0], x[1], &Hs);
    break;

  }

  if (Hs.size() < 1) {
    LOG(ERROR) << " Cannot refine a solution using all the inliers ";
    return -1;
  }

  H = Hs[0];

  LOG(INFO) << "H:\n" << H << std::endl;

  //warp imageB on ImageA.

  int tx=0, ty=0, wOut=0, hOut=0;
  Overlap_ComputeBoundingBox(imageA.Width(), imageA.Height(),
                                imageB.Width(), imageB.Height(),
                                H, tx, ty, wOut, hOut);

  Array3Du warpingImage(hOut, wOut,3);
  warpingImage.Fill(0);

  //-- Fill destination image
  //-- (Backward mapping. For the destination pixel search which pixel
  //    contribute ?).
  for(int j=0; j < hOut; ++j)
  for(int i=0; i < wOut; ++i)
  {
    //- Algo :
    // For the destination pixel (i,j) search which pixel from ImageA
    //  and ImageB contribute.
    // Perform a mean blending in the overlap zone, transfert original
    //  value in the other part.

    Vec3 Pos;
    Pos << i-tx,j-ty,1.0;
    const int xPos = Pos(0), yPos = Pos(1);

    bool bAContrib = false, bBContrib=false;
    if( imageA.Contains( yPos, xPos ) )
      bAContrib = true;

    Vec3 imagePosB = H*Pos;
    imagePosB/=imagePosB(2);
    if( imageB.Contains( imagePosB(1), imagePosB(0) ) )
      bBContrib = true;

    if(bAContrib && bBContrib)  //mean blending between ImageA and ImageB
    {
      warpingImage(j,i,0) =
        (imageA(yPos,xPos,0) +
        SampleLinear( imageB, imagePosB(1),imagePosB(0),0))/2;

      warpingImage(j,i,1) =
        (imageA(yPos,xPos,1) +
        SampleLinear( imageB, imagePosB(1),imagePosB(0),1))/2;

      warpingImage(j,i,2) =
        (imageA(yPos,xPos,2) +
        SampleLinear( imageB, imagePosB(1),imagePosB(0),2))/2;
      continue;
    }
    if(bAContrib && !bBContrib) //only ImageA contrib
    {
      warpingImage(j,i,0) = imageA(yPos,xPos,0);
      warpingImage(j,i,1) = imageA(yPos,xPos,1);
      warpingImage(j,i,2) = imageA(yPos,xPos,2);
      continue;
    }
    if(!bAContrib && bBContrib) //only ImageB contrib
    {
      warpingImage(j,i,0) = SampleLinear( imageB, imagePosB(1),imagePosB(0),0);
      warpingImage(j,i,1) = SampleLinear( imageB, imagePosB(1),imagePosB(0),1);
      warpingImage(j,i,2) = SampleLinear( imageB, imagePosB(1),imagePosB(0),2);
      continue;
    }

  }
  WriteJpg(warpingImage, string(sImageOut).c_str(), 100);

  return 0;
}

/**
 * Compute the dimension of the image that can contain image1 (width1,height1)
 *  and the projection of image2 : (width2,height2).
 */
void Overlap_ComputeBoundingBox(int w1, int h1, int w2, int h2,
                                const Mat3 & Homography,
                                int & tx, int & ty, int & wOut, int & hOut)
{
  //-- Initialized with the dimension of Image 1
  int minx=0, miny=0, maxx=w1, maxy=h1;

  //-- compute where the second images boxes is projected onto image1 coords:
  int xCoord[4] = {0, w2, w2, 0 };
  int yCoord[4] = {0, 0,  h2, h2};

  Mat3 Hinv(3,3);
  Hinv = Homography.inverse();
  for(int i=0; i<4; ++i)
  {
    Vec3 Pos;
    Pos << xCoord[i],yCoord[i],1.0;

    Vec3 Pos2 = Hinv * Pos;
    Vec3 posImage = Pos2/Pos2(2);

    double xT = posImage(0), yT = posImage(1);

    // xCase
    if( xT < minx)
      minx=floor(xT);
    else if( xT > maxx)
      maxx=ceil(xT);
    //yCase
    if( yT < miny)
      miny=floor(yT);
    else if( yT > maxy)
      maxy=ceil(yT);
  }
  // Use floor and ceil to be sure that we will not miss '1/2' pixel reprojection.

  // Compute the translation vector that we need to fit the two image into a big one
  tx = - minx;
  ty = - miny;
  //-- Compute the image dimension that can contain the two overlapped images
  wOut = maxx - minx;
  hOut = maxy - miny;
}
