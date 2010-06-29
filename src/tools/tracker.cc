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
#include "libmv/correspondence/robust_tracker.h"
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
#include "libmv/image/image_converter.h"
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
DEFINE_bool(robust_tracker, true, 
            "perform a robust tracking (with epipolar filtering)");
DEFINE_double(focal, 50, 
            " focale length for all the cameras");
DEFINE_bool(pose_estimation, false, 
            "perform a pose estimation");

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
  s.append("-features.png");
  std::cout << "Writing file ["<<s<<"]"<<std::endl;
  WriteImage (imageArrayBytes, s.c_str());
}

void DisplayMatches(Matches::Matches &matches)
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

bool IsArgImage(const std::string & arg) {
  return (arg.find_last_of (".png") == arg.size() - 1 ||
          arg.find_last_of (".jpg") == arg.size() - 1 ||
          arg.find_last_of (".jpeg") == arg.size()- 1 ||
          arg.find_last_of (".pnm") == arg.size() - 1 );
}

int main (int argc, char *argv[]) {    
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);
  
  std::list<std::string> image_list;
  std::vector<std::pair<size_t, size_t> > image_sizes;
    
  for (int i = 1;i < argc;++i) {
    std::string arg (argv[i]);
    if (IsArgImage(arg)) {
      image_list.push_back(arg);
    }
  }
  
  size_t number_of_images = image_list.size();
  
  // Create the tracker
  detector::Detector * detector                 = NULL;
  descriptor::Describer *describer              = NULL;
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
  
  libmv::tracker::FeaturesGraph all_features_graph;
   
  tracker::Tracker *points_tracker = NULL;
  if (!FLAGS_robust_tracker)
    points_tracker = new tracker::Tracker(detector,describer,matcher);
  else
    points_tracker = new tracker::RobustTracker(detector,describer,matcher);
  
  // Track the sequence of images  
  size_t image_index = 1;
  std::list<std::string>::iterator image_list_iterator = image_list.begin();
  std::string first_image_path = *image_list_iterator; 
  image_list_iterator++;
  for (; image_list_iterator != image_list.end(); ++image_list_iterator) {
    std::string image_path = (*image_list_iterator);
    if (image_index == 1) {
      LOG(INFO) << "Tracking image '"<< first_image_path << "'" << std::endl;
      Array3Du imageArrayBytes1;
      ReadImage (first_image_path.c_str(), &imageArrayBytes1);
      
      // Grayscale image convertion
      Array3Du *arrayGrayBytes = NULL;
      if (imageArrayBytes1.Depth() == 3) {
        arrayGrayBytes = new Array3Du ();
        Rgb2Gray<Array3Du, Array3Du>(imageArrayBytes1, arrayGrayBytes);
      } else {
        arrayGrayBytes = new Array3Du (imageArrayBytes1);
      }
      Image image1 (arrayGrayBytes);
      
      image_sizes.push_back(std::pair<size_t,size_t>(
       arrayGrayBytes->Height(), arrayGrayBytes->Width()));
      
      LOG(INFO) << "Tracking image '"<< image_path << "'" << std::endl;
      Array3Du imageArrayBytes2;
      ReadImage (image_path.c_str(), &imageArrayBytes2);
      
      // Grayscale image convertion
      arrayGrayBytes = NULL;
      if (imageArrayBytes2.Depth() == 3) {
        arrayGrayBytes = new Array3Du ();
        Rgb2Gray<Array3Du, Array3Du>(imageArrayBytes2, arrayGrayBytes);
      } else {
        arrayGrayBytes = new Array3Du (imageArrayBytes2);
      }
      Image image2(arrayGrayBytes);
      
      image_sizes.push_back(std::pair<size_t,size_t>(
       arrayGrayBytes->Height(), arrayGrayBytes->Width()));
       
      points_tracker->Track(image1, image2, &all_features_graph);
      
      LOG(INFO) << "#Tracks "<< all_features_graph.matches_.NumTracks()
       << std::endl;
      LOG(INFO) << "#Images "<< all_features_graph.matches_.NumImages() 
       << std::endl;
             
      if (FLAGS_save_features) {
        Matches::Features<PointFeature> features_set =
         all_features_graph.matches_.InImage<PointFeature>(0);
        WriteFeaturesImage(imageArrayBytes1, 
                          first_image_path,
                          features_set);
        features_set = all_features_graph.matches_.InImage<PointFeature>(1);
        WriteFeaturesImage(imageArrayBytes2, 
                          image_path,
                          features_set);
      }
    } else { 
      LOG(INFO) << "Tracking image '"<< image_path << "'" << std::endl;
      Array3Du imageArrayBytes;
      ReadImage (image_path.c_str(), &imageArrayBytes);
      Array3Du *arrayGrayBytes = NULL;
      // Grayscale image convertion
      if (imageArrayBytes.Depth() == 3) {
        arrayGrayBytes = new Array3Du ();
        Rgb2Gray<Array3Du, Array3Du>(imageArrayBytes, arrayGrayBytes);
      } else {
        arrayGrayBytes = new Array3Du (imageArrayBytes);
      }
      Image image(arrayGrayBytes);
      
      image_sizes.push_back(std::pair<size_t,size_t>(
       arrayGrayBytes->Height(), arrayGrayBytes->Width()));
          
      libmv::tracker::FeaturesGraph new_features_graph;  
      libmv::Matches::ImageID new_image_id;
      points_tracker->Track(image, 
                            all_features_graph, 
                            &new_features_graph,
                            &new_image_id);
      
      LOG(INFO) << "#NewTracks "<< new_features_graph.matches_.NumTracks()
       << std::endl;
              
      if (FLAGS_save_features) {
        Matches::Features<PointFeature> features_set =
         new_features_graph.matches_.InImage<PointFeature>(new_image_id);
        WriteFeaturesImage(imageArrayBytes, 
                           image_path,
                           features_set);
      }
      all_features_graph.Merge(new_features_graph);
      LOG(INFO) << "#Tracks "<< all_features_graph.matches_.NumTracks() 
       << std::endl;
      LOG(INFO) << "#Images "<< all_features_graph.matches_.NumImages() 
       << std::endl;
    }
    image_index++;
  }

  DisplayMatches(all_features_graph.matches_);
  
  //TODO (julien) Move this part into another tool (here it's a 2D tracker only)
  // Pose estimation
  if (FLAGS_pose_estimation)  {
    
    vector<Mat3> Ks(number_of_images);
    
    vector<Mat3> Rs(number_of_images);
    vector<Vec3> ts(number_of_images);
    
    vector<Mat3> Fs(number_of_images);
    vector<Mat3> Es(number_of_images);
    
    size_t index_image = 0;
    std::set<Matches::ImageID>::iterator image_iter =
      all_features_graph.matches_.get_images().begin();
      
    Matches::ImageID previous_image_id = *image_iter;
                  
    Ks[0] << FLAGS_focal,  0, image_sizes[index_image].second/2.0,
    0, FLAGS_focal, image_sizes[index_image].first/2.0,
    0,  0,     1;
    Rs[0].setIdentity();
    ts[0]<< 0, 0, 0;
    
    image_iter++;
    index_image++;
    for (; image_iter != all_features_graph.matches_.get_images().end();
     ++image_iter, ++index_image) {
                    
      Ks[index_image] << FLAGS_focal,  0, image_sizes[index_image].second/2.0,
      0, FLAGS_focal, image_sizes[index_image].first/2.0,
      0,  0,     1;
      
      vector<Mat> xs(2);
      TwoViewPointMatchMatrices(all_features_graph.matches_, 
                                previous_image_id, 
                                *image_iter, 
                                &xs);
      Mat &x0 = xs[0];
      Mat &x1 = xs[1];
      // Compute fundamental matrix 
      FundamentalFromCorrespondences7PointRobust(x0, 
                                                 x1, 
                                                 1, 
                                                 &Fs[index_image],
                                                 NULL);
                                         
      // Compute essential matrix
      EssentialFromFundamental(Fs[index_image], 
                               Ks[index_image-1], 
                               Ks[index_image], 
                               &Es[index_image]);
            
      // Recover variation dR, dt from E and K
      MotionFromEssentialAndCorrespondence(Es[index_image], 
                                           Ks[index_image-1], x0.col(0), 
                                           Ks[index_image], x1.col(0), 
                                           &Rs[index_image], 
                                           &ts[index_image]);
      
      // Recover the real R = Rprev * dR, t = Rprev * dt + tprev
      Rs[index_image] = Rs[index_image-1] * Rs[index_image];
      ts[index_image] = Rs[index_image-1] * ts[index_image] + ts[index_image-1];
      
      LOG(INFO) << " T =  "<< ts[index_image] << std::endl;
      
      //TODO (julien) Triangulation (features viewed in min N=3 views ?)
      //TODO (julien) Bundle Adjustment (projective/Euclidean BA ?)
    }
  }
  
  // Delete the tracker
  if (points_tracker) {
    delete points_tracker;
    points_tracker = NULL;
  }
  // Delete the features graph
  all_features_graph.Clear();
  
  //TODO(jmichot) Clean the variables detector, describer, matcher
  return 0;
}
