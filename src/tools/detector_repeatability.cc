// Copyright (c) 2010 libmv authors.
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
#include <iostream>

#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/star_detector.h"
#include "libmv/detector/surf_detector.h"
#include "libmv/image/image.h"
#include "libmv/image/image_converter.h"
#include "libmv/image/image_drawing.h"
#include "libmv/image/image_io.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

void usage() {
  LOG(ERROR) << " repreatability ImageReference ImageA ImageB ... " <<std::endl
    << " ImageReference  : the input image on which features will be extrated,"
    << std::endl
    << " ImageA : an image to test repeatability with ImageReference"
    << std::endl
    << " ImageB : an image to test repeatability with ImageReference"
    << std::endl
    << " ... : images names to test repeatability with ImageReference"
    << std::endl
    << " Gound truth transformation between ImageReference and ImageX is named\
       in ImageX.txt"
    << " INFO : !!! experimental !!! ." << std::endl;
}


enum eDetectorType
{
  FAST_DETECTOR = 0,
  SURF_DETECTOR = 1,
  STAR_DETECTOR = 2
};

// Detect features over the image im with the choosen detector type.
bool detectFeatures(const eDetectorType DetectorType,
                    const Image & im,
                    libmv::vector<libmv::Feature *> * featuresOut);

// Search how many feature are considered as repeatable.
// Ground Truth transformation is encoded in a matrix saved into a file.
// Example : ImageNameB.jpg.txt encode the transformation matrix from
//   imageReference to ImageNameB.jpg.
bool testRepeatability(const libmv::vector<libmv::Feature *> & featuresA,
                       const libmv::vector<libmv::Feature *> & featuresB,
                       const string & ImageNameB,
                       const Image & imageB,
                       libmv::vector<double> * exportedData = NULL,
                       ostringstream * stringStream = NULL);

int main(int argc, char **argv) {

  libmv::Init("Extract features from on images series and test detector \
              repeatability", &argc, &argv);

  if (argc < 3 || 
      (GetFormat(argv[1])==Unknown && GetFormat(argv[2])==Unknown)) {
    usage();
    LOG(ERROR) << "Missing parameters or errors in the command line.";
    return 1;
  }

  // Parse input parameter.
  const string sImageReference = argv[1];
  
  ByteImage byteImage;
  if ( 0 == ReadImage( sImageReference.c_str(), &byteImage) )  {
    LOG(ERROR) << "Invalid inputImage.";
    return 1;
  }
  Image imageReference(new ByteImage(byteImage));
  if( byteImage.Depth() == 3)
  {
    // Convert Image to desirable format => uchar 1 gray channel
    ByteImage byteImageGray;
    Rgb2Gray(byteImage, &byteImageGray);
    imageReference=Image(new ByteImage(byteImageGray));
  }

  eDetectorType DETECTOR_TYPE = FAST_DETECTOR;

  libmv::vector<libmv::Feature *> featuresRef;
  if ( !detectFeatures( DETECTOR_TYPE,
                     imageReference,
                     &featuresRef))
  {
    LOG(ERROR) << "No feature found on Reference Image.";
    return 1;
  }
  
  libmv::vector<double> repeatabilityStat;
  // Run repeatability test on the N remaining images.
  for(int i = 2; i < argc; ++i)
  {
    const string sImageToCompare = argv[i];
    libmv::vector<libmv::Feature *> featuresToCompare;
    
    if ( 0 == ReadImage( sImageToCompare.c_str(), &byteImage) )  {
      LOG(ERROR) << "Invalid inputImage (Image to compare to reference).";
      return 1;
    }
    Image imageToCompare(new ByteImage(byteImage));
    if( byteImage.Depth() == 3)
    {
      // Convert Image to desirable format => uchar 1 gray channel
      ByteImage byteImageGray;
      Rgb2Gray(byteImage, &byteImageGray);
      imageToCompare = Image(new ByteImage(byteImageGray));
    }
    if (detectFeatures(DETECTOR_TYPE,
                    imageToCompare,
                    &featuresToCompare))
    {
      testRepeatability( featuresRef,
                         featuresToCompare,
                         sImageToCompare + string(".txt"),
                         imageToCompare,
                         &repeatabilityStat);
    }
    else
    {
      LOG(INFO) << "Image : " << sImageToCompare 
        << " have no feature detected with the choosen detector.";
    }
    DeleteElements(&featuresToCompare);
  }
  // Export data :
  ofstream fileStream("Repeatability.xls");
  fileStream << "RepeatabilityStats" << endl;
  fileStream << "ImageName \t Feature In Reference \t Features In ImageName \
                \t Position Repeatability \t Position Accuracy" << endl;
  int cpt=0;
  for (int i = 2; i < argc; ++i)  {
    fileStream << argv[i] << "\t";
    for (int j=0; j < repeatabilityStat.size()/(argc-2); ++j) {
      fileStream << repeatabilityStat[cpt] << "\t";
      cpt++;
    }
    fileStream << endl;
  }

  DeleteElements(&featuresRef);
  fileStream.close();
}

bool detectFeatures(const eDetectorType DetectorType,
                    const Image & im,
                    libmv::vector<libmv::Feature *> * featuresOut)
{
  using namespace detector;
  switch (DetectorType)  {
    case FAST_DETECTOR:
    {
      scoped_ptr<Detector> detector(CreateFastDetector(9, 30));
      detector->Detect( im, featuresOut, NULL);
    }
    break;
    case SURF_DETECTOR:
    {
      scoped_ptr<Detector> detector(CreateSURFDetector());
      detector->Detect( im, featuresOut, NULL);
    }
    break;
    case STAR_DETECTOR:
    {
      scoped_ptr<Detector> detector(CreateStarDetector());
      detector->Detect( im, featuresOut, NULL);
    }
    break;
    default:
    {
      scoped_ptr<Detector> detector(CreateFastDetector(9, 30));
      detector->Detect( im, featuresOut, NULL);
    }
  }
  return (featuresOut->size() >= 1);
}

bool testRepeatability(const libmv::vector<libmv::Feature *> & featuresA,
                       const libmv::vector<libmv::Feature *> & featuresB,
                       const string & ImageNameB,
                       const Image & imageB,
                       libmv::vector<double> * exportedData,
                       ostringstream * stringStream)
{
  // Config Threshold for repeatability
  const double distThreshold = 1.5;

  //
  Mat3 transfoMatrix;
  if(ImageNameB.size() > 0 )  {
    // Read transformation matrix from data
    ifstream file( ImageNameB.c_str());
    if(file.is_open())  {
      for(int i=0; i<3*3; ++i)  {
        file>>transfoMatrix(i);
      }
      // Transpose cannot be used inplace.
      Mat3 temp = transfoMatrix.transpose();
      transfoMatrix = temp;
    }
    else  {
      LOG(ERROR) << "Invalid input transformation file.";
      if (stringStream) {
        (*stringStream) << "Invalid input transformation file.";
      }
      if(exportedData)  {
        exportedData->push_back(featuresA.size());
        exportedData->push_back(featuresB.size());
        exportedData->push_back(0);
        exportedData->push_back(0);
      }
      return 0;
    }
  }
 
  int nbRepeatable = 0;
  int nbRepeatablePossible = 0;
  double localisationError = 0;

  for (int iA=0; iA < featuresA.size(); ++iA) {
    const libmv::PointFeature * featureA = 
      dynamic_cast<libmv::PointFeature *>( featuresA[iA] );
    // Project A into the image coord system B.
    Vec3 pos; pos << featureA->x(), featureA->y(), 1.0;
    Vec3 transformed = transfoMatrix * pos;
    transformed/=transformed(2);

    //Search the nearest featureB
    double distanceSearch = std::numeric_limits<double>::max();
    int indiceFound = -1;

    // Check if imageB Contain the projected point.
    if ( imageB.AsArray3Du()->Contains(pos(0), pos(1)) )  {
      ++nbRepeatablePossible; //This feature could be detected in imageB also
      for (int iB=0; iB < featuresB.size(); ++iB) {
        const libmv::PointFeature * featureB =
          dynamic_cast<libmv::PointFeature *>( featuresB[iB] );
        Vec3 posB; posB << featureB->x(), featureB->y(), 1.0;
        //Test distance over the two points.
        double distance = DistanceL2(transformed,posB);

        //If small enough consider the point can be the same
        if ( distance <= distThreshold )  {
          distanceSearch = distance;
          indiceFound = iB;          
        }
      }
    }
    if ( indiceFound != -1 ) {
      //(test other parameter scale, orientation if any)
      ++nbRepeatable;
      const libmv::PointFeature * featureB =
        dynamic_cast<libmv::PointFeature *>( featuresB[indiceFound] );
      Vec3 posB; posB << featureB->x(), featureB->y(), 1.0;
      //Test distance over the two points.
      double distance = DistanceL2(transformed,posB);
      //cout << endl << distance;
      localisationError += distance;
    }
  }
  if( nbRepeatable >0 )  {
    localisationError/= nbRepeatable;
  }
  else  {
    localisationError = 0;
  }
  nbRepeatablePossible = min(nbRepeatablePossible, featuresB.size());

  ostringstream os;
  os<< endl << " Feature Repeatability " << ImageNameB << endl
    << " ---------------------- " << endl
    << " Image A get\t" << featuresA.size() << "\tfeatures" << endl
    << " Image B get\t" << featuresB.size() << "\tfeatures" << endl
    << " ---------------------- " << endl
    << " Position repeatability :\t" << 
      nbRepeatable / (double) nbRepeatablePossible * 100 << endl
    << " Position mean error    :\t" << localisationError << endl;
  cout << os.str();
  if (stringStream) {
    (*stringStream) << os.str();
  }
  if(exportedData)  {
    exportedData->push_back(featuresA.size());
    exportedData->push_back(featuresB.size());
    exportedData->push_back(nbRepeatable/(double) nbRepeatablePossible * 100);
    exportedData->push_back(localisationError);
  }
  return (nbRepeatable != 0);
}
