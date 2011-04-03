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

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>


#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/base/scoped_ptr.h"
#include "libmv/correspondence/export_matches_txt.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/nRobustViewMatching.h"
#include "libmv/detector/detector_factory.h"
#include "libmv/descriptor/descriptor_factory.h"
#include "libmv/image/image.h"
#include "libmv/image/image_converter.h"
#include "libmv/image/image_drawing.h"
#include "libmv/image/image_io.h"
#include "libmv/tools/tool.h"

using namespace libmv;
using namespace std;

DEFINE_string(detector, "FAST", "select the detector (FAST,STAR,SURF,MSER)");
DEFINE_string(describer, "DIPOLE",
              "select the descriptor (SIMPLIEST,SURF,DIPOLE,DAISY)");
DEFINE_bool(save_matches_results, true,
            "save images with detected and matched features");
DEFINE_bool(save_matches_file, false,
            "save the matches in a file");
DEFINE_string(matches_out, "matches.txt", "Matches output file");
DEFINE_bool(save_hugin, true,
            "save Hugin point matches (ready to minimize Hugin project)");
//TODO(pmoulon) this parameter must set the homography as geometric constraint
DEFINE_bool(save_reconstruction, true,
            "save perspective reconstruction from largest track (.ply)");

// TODO(pmoulon) move this function in a more general file
template <typename Image>
void DrawFeatures( Image & im, const libmv::vector<libmv::Feature *> & feat)
{
  std::cout << feat.size() << " Detected points " <<std::endl;
  for (int i = 0; i < feat.size(); ++i)
  {
    const libmv::PointFeature * feature =
      dynamic_cast<libmv::PointFeature *>( feat[i] );
    const int x = feature->x();
    const int y = feature->y();
    const float scale = feature->scale;

    DrawCircle(x, y, scale, (unsigned char)255, &im);
    const float angle = feature->orientation;
    DrawLine(x, y, x + scale * cos(angle), y + scale *sin(angle),
             (unsigned char) 255, &im);
  }
}

std::string ExtractFilename( const std::string& path )
{
  return path.substr( path.find_last_of( "\\/" ) +1 );
}

bool IsArgImage(const std::string & arg) {
  string copy = arg;
  // Convert to lower case to avoid JPG/jpg confusion
  std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);
  return (arg.size()>3 &&
         (copy.find_last_of (".png") == copy.size() - 1 ||
          copy.find_last_of (".jpg") == copy.size() - 1 ||
          copy.find_last_of (".jpeg") == copy.size()- 1 ||
          copy.find_last_of (".pnm") == copy.size() - 1 ));
}

//TODO(pmoulon) Move this function in matches. (use by this file and tracker)
void DisplayMatches(const Matches &matches)
{
  std::cout << "Matches : \t\t"<<std::endl << "\t";
  for (size_t j = 0; j < matches.NumImages(); j++) {
    std::cout << j << " ";
  }
  std::cout << std::endl;

  for (size_t i = 0; i < matches.NumTracks(); i++) {
    std::cout << i <<"\t";

    for (size_t j = 0; j < matches.NumImages(); j++) {
      const Feature * f = matches.Get(j,i);
      if (f)
        std::cout << "X ";
      else
        std::cout << "  ";
    }
    std::cout <<std::endl;
  }
}


int main(int argc, char **argv) {

  google::SetUsageMessage("NViewMatching Demo.");
  google::ParseCommandLineFlags(&argc, &argv, true);

  libmv::vector<string> image_vector;

  for (int i = 1;i < argc;++i) {
    std::string arg (argv[i]);
    if (IsArgImage(arg)) {
      image_vector.push_back(arg);
    }
  }

  detector::eDetector edetector = detector::FAST_DETECTOR;
  if (FLAGS_detector == "FAST") {
    edetector = detector::FAST_DETECTOR;
  } else if (FLAGS_detector == "SURF") {
    edetector = detector::SURF_DETECTOR;
  } else if (FLAGS_detector == "STAR") {
    edetector = detector::STAR_DETECTOR;
  } else if (FLAGS_detector == "MSER") {
    edetector = detector::MSER_DETECTOR;
  } else {
    LOG(FATAL) << "ERROR : undefined Detector !";
  }
  scoped_ptr<detector::Detector> spDetector = detectorFactory(edetector);

  descriptor::eDescriber edescriber = descriptor::DIPOLE_DESCRIBER;
  if (FLAGS_describer == "SIMPLIEST") {
    edescriber = descriptor::SIMPLEST_DESCRIBER;
  } else if (FLAGS_describer == "SURF") {
    edescriber = descriptor::SURF_DESCRIBER;
  } else if (FLAGS_describer == "DIPOLE") {
    edescriber = descriptor::DIPOLE_DESCRIBER;
  } else if (FLAGS_describer == "DAISY") {
    edescriber = descriptor::DAISY_DESCRIBER;
  } else {
    LOG(FATAL) << "ERROR : undefined Describer !";
  }
  scoped_ptr<descriptor::Describer> spDescriber  = describerFactory(edescriber);

  libmv::correspondence::nRobustViewMatching nViewMatcher(spDetector.get(),
                                                          spDescriber.get());

  nViewMatcher.computeCrossMatch(image_vector);

  // Show Cross Matches
  DisplayMatches(nViewMatcher.getMatches());
  
  //-- Export the matches
  if (FLAGS_save_matches_file) {
    ExportMatchesToTxt(nViewMatcher.getMatches(), FLAGS_matches_out);
  }

  //-- Export and visualize data (show matches between the images)
  if (FLAGS_save_matches_results) {
    for (size_t i=0; i< nViewMatcher.getMatches().NumImages(); ++i) {
      Array3Du imageA;
      ReadImage( image_vector[i].c_str() , &imageA);
      Array3Du imageTemp;
      Rgb2Gray( imageA, &imageTemp);
      Image im( new Array3Du(imageTemp) );

      for (size_t j=0; j< i/*nViewMatcher.m_tracks.NumImages()*/; ++j)  {
        Matches::Features<KeypointFeature> features =
          nViewMatcher.getMatches().InImage<KeypointFeature>(i);
        int cpt = 0;
        Array3Du bigIma;
        if (i!=j && features) {
          Array3Du imageB;
          ReadImage(image_vector[j].c_str() , &imageB);
          Array3Du imageTemp2;
          Rgb2Gray( imageB, &imageTemp2);
          Image im2( new Array3Du(imageTemp2) );

          //-- Concat the two images (TODO move this function in IMAGE module)
          // With horizontal, vertical option
          int width = max(imageB.Width(),
                      imageA.Width()) + min(imageB.Width(), imageA.Width());
          int height = max(imageB.Height(), imageA.Height());
          //-- Paste data
          bigIma = Array3Du(height,width);
          for (int jj=0;jj<imageA.Height();++jj)
          for (int ii=0;ii<imageA.Width();++ii) {
            bigIma(jj,ii)= (*im.AsArray3Du())(jj,ii);
          }
          for (int jj=0;jj<imageB.Height();++jj)
          for (int ii=0;ii<imageB.Width();++ii) {
            bigIma(jj,ii+min(imageB.Width(), imageA.Width()))=
              (*im2.AsArray3Du())(jj,ii);
          }


          while (features) {
            Matches::TrackID id_track = features.track();
            const Feature * ref = features.feature();
            const Feature * f = nViewMatcher.getMatches().Get(j, id_track);
            if (f && ref)  {
              KeypointFeature * pt0 = ((KeypointFeature*)ref);
              KeypointFeature * pt1 = ((KeypointFeature*)f);
              DrawCircle(pt0->x(), pt0->y(),
                          pt0->scale, (unsigned char)255, &bigIma);
              DrawCircle(pt1->x()+min(imageA.Width(), imageB.Width()),
                          pt1->y(), pt1->scale, (unsigned char)255, &bigIma);
              DrawLine( pt0->x(),
                        pt0->y(),
                        pt1->x()+min(imageB.Width(), imageA.Width()),
                        pt1->y(),
                        255,
                        &bigIma);
              ++cpt;
            }
            features.operator++();
          }
        }
        //-- If many point have been added
        if (cpt> 4*2) {
          ostringstream os;
          os<< "TOTO_" << ExtractFilename(image_vector[i].c_str()) << "_"
            << ExtractFilename(image_vector[j].c_str()) << "___"
            << cpt << "__matches" << ".jpeg";
          WriteJpg(bigIma, os.str().c_str(), 100);
        }
      }
    }
  }

  //-- Export to Hugin oto format
  if (FLAGS_save_hugin) {
    ofstream myFile("huginGenerated.txt");

    myFile << "# oto project file generated by Libmv"<< endl << endl;

    myFile << "input images :"<< endl;

    for (int i = 0; i < image_vector.size(); ++i) {
      //-- A. Export image info
      ByteImage byteImage;
      if ( 0 == ReadImage( image_vector[i].c_str(), &byteImage) )  {
        LOG(ERROR) << "Invalid inputImage.";
        continue;
      }
      int width = byteImage.Width();
      int height = byteImage.Height();
      myFile << "#-imgfile " << width << " " << height
        << " " << ExtractFilename(image_vector[i]) << endl;
      // No specific camera parameter
      myFile << "o f0 y+0.000000 r+0.000000 p+0.000000 u20 d0 e0 v50 a0 b0 c0" << endl;
    }

    //-- B. Export matches :

    myFile << endl << "# Control points:"<<endl;

    for (size_t i=0; i< nViewMatcher.getMatches().NumImages(); ++i) {
      for (size_t j=0; j<i; ++j)  {
        Matches::Features<KeypointFeature> features =
          nViewMatcher.getMatches().InImage<KeypointFeature>(i);

        while (features) {

          Matches::TrackID id_track = features.track();
          const Feature * ref = features.feature();
          const Feature * f = nViewMatcher.getMatches().Get(j, id_track);
          if (f && ref)  {
            KeypointFeature * pt0 = ((KeypointFeature*)ref);
            KeypointFeature * pt1 = ((KeypointFeature*)f);
            myFile << endl << "c "
               << "n" << i << " "
               << "N" << j << " "
               << "x" << pt0->x() << " "
               << "y" << pt0->y() << " "
               << "X" << pt1->x() << " "
               << "Y" << pt1->y();
          }

        features.operator++();
        }
      }
    }
  }

  if (FLAGS_save_reconstruction)
  {
    //-- Do something with the Matches :
    Matches matches = nViewMatcher.getMatches();

    //-- Count the occurences of the tracks lenght
    libmv::vector<int> vec_trackLenghtHisto(matches.NumImages()+1);
    fill(vec_trackLenghtHisto.begin(), vec_trackLenghtHisto.end(),0);
    for (size_t i = 0; i < matches.NumTracks(); i++) {
      int iCount = 0;
      for (size_t j = 0; j < matches.NumImages(); j++) {
        const Feature * f = matches.Get(j,i);
        iCount+= (f)? 1 : 0;
      }
      vec_trackLenghtHisto[iCount]++;
    }

    //-- Display the occurence of each track lenght :
    copy(vec_trackLenghtHisto.begin(), vec_trackLenghtHisto.end(),
      std::ostream_iterator<int>(std::cout, " "));

    //-- Do we have sufficient point to perform a nView reconstruction :
    //TODO(pmoulon) : perform the perspective reconstruction from NviewTensor
  }

  return 0;
}

