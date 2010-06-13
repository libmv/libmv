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
#include <iostream>
#include <list>
#include <string>

#include "libmv/base/scoped_ptr.h"
#include "libmv/base/vector.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/ArrayMatcher_Kdtree.h"
#include "libmv/correspondence/feature_matching.h"
#include "libmv/correspondence/feature_matching_FLANN.h"
#include "libmv/correspondence/tracker.h"
#include "libmv/detector/detector.h"
#include "libmv/detector/fast_detector.h"
#include "libmv/detector/star_detector.h"
#include "libmv/detector/surf_detector.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/daisy_descriptor.h"
#include "libmv/descriptor/dipole_descriptor.h"
#include "libmv/descriptor/simpliest_descriptor.h"
#include "libmv/descriptor/surf_descriptor.h"
#include "libmv/image/array_nd.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/bundle.h"
#include "libmv/numeric/numeric.h"
#include "libmv/tools/tool.h"

using namespace libmv;

DEFINE_string(detector, "FAST", "select the detector (FAST,STAR,SURF)");
DEFINE_string(describer, "SIMPLIEST", 
              "select the detector (SIMPLIEST,SURF,DIPOLE,DAISY)");
DEFINE_bool(save_features, false, 
            "save images with detected and matched features");

void WriteFeaturesImage(Array3Du &imageArrayBytes,
                        std::string out_file_path,
                        Matches::Features<PointFeature> &features)
{  
  while(features) {

    imageArrayBytes(features.feature()->y(),
                    features.feature()->x()) = 255;
    features.operator++();
  }
  
  std::string s = out_file_path;
  s.erase(s.end()-4,s.end());
  s.append("-features.pnm");
  WritePnm (imageArrayBytes, s.c_str());
}

void DisplayMatches(Matches::Matches &matches)
{ 
  std::cout << "Matches :"<<std::endl;
  std::cout << "    ";
  for (size_t j = 0; j < matches.NumImages(); j++) {
    if (j < 10) std::cout << " ";
    std::cout << j << " ";
  }
  std::cout << std::endl;
  
  for (size_t i = 0; i < matches.NumTracks(); i++) {
    if(i < 10)   std::cout << " ";
    if(i < 100)  std::cout << " ";
    if(i < 1000) std::cout << " ";
    std::cout << i <<" ";
    
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

bool IsArgImage(std::string & arg) {
  std::string arg_lower = arg;
  //std::transform(arg.begin(), arg.end(), arg_lower.begin(), std::tolower);
  if (arg_lower.find_last_of (".png") == arg_lower.size() - 1  ||
               arg_lower.find_last_of (".jpg") == arg_lower.size() - 1  ||
               arg_lower.find_last_of (".jpeg") == arg_lower.size() - 1 ||
               arg_lower.find_last_of (".pnm") == arg_lower.size() - 1 ) {
    return true;
  }
  return false;
}

int main (int argc, char *argv[]) {    
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  std::list<std::string> image_list;
    
  for (int i = 1;i < argc;++i) {
    std::string arg (argv[i]);
    if (IsArgImage(arg)) {
      LOG(INFO) << "New image." << std::endl;
      image_list.push_back(arg);
    }
  }
  
  // Create the tracker
  detector::Detector * detector                 = NULL;
  descriptor::Describer *describer              =  NULL;
  correspondence::ArrayMatcher<float> *matcher  = NULL;
  
  if (FLAGS_detector == "FAST") {
    detector = detector::CreateFastDetector(9, 10, true);
  } else if (FLAGS_detector == "SURF") {
    detector = detector::CreateSURFDetector();
  } else if (FLAGS_detector == "STAR") {
    detector = detector::CreateStarDetector(true);
  } else {
    LOG(FATAL) << "ERROR : undefined Detector !";
  }
  if (FLAGS_describer == "SIMPLIEST") {
    describer = descriptor::CreateSimpliestDescriber();
  } else if (FLAGS_describer == "SURF") {
    describer = descriptor::CreateSurfDescriber();
  } else if (FLAGS_describer == "DIPOLE") {
    describer = descriptor::CreateDipoleDescriber();
  } else if (FLAGS_describer == "DAISY") {
    describer = descriptor::CreateDaisyDescriber();
  } else {
    LOG(FATAL) << "ERROR : undefined Describer !";
  }
  
  matcher = new correspondence::ArrayMatcher_Kdtree<float>();
    
  tracker::Tracker points_tracker(detector,describer,matcher);
  libmv::Matches all_matches;
  
  // Track the sequence of images  
  size_t image_index = 1;
  std::list<std::string>::iterator image_list_iterator = image_list.begin();
  std::string first_image_path = *image_list_iterator; 
  image_list_iterator++;
  for (; image_list_iterator !=
         image_list.end(); ++image_list_iterator) {
    std::string image_path = (*image_list_iterator);
    if (image_index == 1) {
      Array3Du imageArrayBytes1;
      ReadImage (first_image_path.c_str(), &imageArrayBytes1);
      Image image1 (new Array3Du (imageArrayBytes1));
      
      Array3Du imageArrayBytes2;
      ReadImage (image_path.c_str(), &imageArrayBytes2);
      Image image2 (new Array3Du (imageArrayBytes2));

      points_tracker.Track<Image>(image1, image2, &all_matches);
      
      LOG(INFO) << "NumTracks "<< all_matches.NumTracks() << std::endl;
      LOG(INFO) << "NumImages "<< all_matches.NumImages() << std::endl;
              
      Matches::Features<PointFeature> features_set =
        all_matches.All<PointFeature>();
      
      if (FLAGS_save_features) {
        WriteFeaturesImage(imageArrayBytes1, 
                          first_image_path,
                          features_set);
        WriteFeaturesImage(imageArrayBytes2, 
                          image_path,
                          features_set);
      }
    } else { 
      Array3Du imageArrayBytes;
      ReadImage (image_path.c_str(), &imageArrayBytes);
      Image image (new Array3Du (imageArrayBytes));
          
      libmv::Matches new_matches;  
      libmv::Matches::ImageID new_image_id;
      points_tracker.Track<Image>(image, 
                                  all_matches, 
                                  &new_matches,
                                  &new_image_id);
      
      LOG(INFO) << "NumTracks "<< new_matches.NumTracks() << std::endl;
      LOG(INFO) << "NumImages "<< new_matches.NumImages() << std::endl;
              
      Matches::Features<PointFeature> features_set =
        new_matches.All<PointFeature>();
      
      if (FLAGS_save_features) {
        WriteFeaturesImage(imageArrayBytes, 
                          image_path,
                          features_set);
      }
      all_matches.Merge(new_matches);
    }
    image_index++;
  }

  DisplayMatches(all_matches);
  return 0;
}
