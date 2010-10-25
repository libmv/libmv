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
#include "libmv/detector/detector_factory.h"
#include "libmv/descriptor/descriptor_factory.h"
#include "libmv/image/array_nd.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_converter.h"
#include "libmv/image/image_drawing.h"
#include "libmv/image/image_pyramid.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/bundle.h"
#include "libmv/multiview/fundamental.h"
#include "libmv/multiview/focal_from_fundamental.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/projection.h"
#include "libmv/multiview/reconstruction.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/triangulation.h"
#include "libmv/numeric/numeric.h"
#include "libmv/tools/tool.h"
#include <zconf.h>

using namespace libmv;

DEFINE_string(detector, "FAST", "select the detector (FAST,STAR,SURF,MSER)");
DEFINE_string(describer, "DAISY",
              "select the descriptor (SIMPLIEST,SURF,DIPOLE,DAISY)");
DEFINE_bool  (save_features, false,
              "save images with detected and matched features");
DEFINE_bool  (save_matches, false,
              "save images with matches");
DEFINE_bool  (robust_tracker, false,
              "perform a robust tracking (with epipolar filtering)");
DEFINE_double(robust_tracker_threshold, 1.0,
              "Epipolar filtering threshold (in pixels)");

DEFINE_bool  (pose_estimation, false,
              "perform a pose estimation");
DEFINE_double(focal, 50,
              "focale length for all the cameras");
DEFINE_double(principal_point_u, 0,
              "principal point u coordinate");
DEFINE_double(principal_point_v, 0,
              "principal point v coordinate");
DEFINE_string(patch, "", "only track this image/patch");

void DrawFeatures(ByteImage &imageArrayBytes,
                  Matches::Features<PointFeature> &features,
                  bool is_draw_orientation) {
  while(features) {
    Byte color = 255;
    float scale = features.feature()->scale;
    float angle = features.feature()->orientation;
    DrawCircle<ByteImage, Byte>(features.feature()->x(),
                                features.feature()->y(),
                                scale,
                                color,
                                &imageArrayBytes);
    if (is_draw_orientation) {
      DrawLine(features.feature()->x(),
               features.feature()->y(),
               features.feature()->x() + scale * cos(angle),
               features.feature()->y() + scale * sin(angle),
               color,
               &imageArrayBytes);
    }
    features.operator++();
  }
}

void DrawMatches(ByteImage &imageArrayBytes,
                 const Matches::ImageID id_image,
                 tracker::FeaturesGraph all_features_graph) {
  Matches::Features<KeypointFeature> features =
   all_features_graph.matches_.InImage<KeypointFeature>(id_image);
  Byte color = 255;
  size_t NumImages = all_features_graph.matches_.NumImages();
  while(features) {
    Matches::TrackID id_track = features.track();
    const Feature * ref = features.feature();
    for (size_t j = 0; j < NumImages; j++) {
      const Feature * f = all_features_graph.matches_.Get(j, id_track);
      if (f && ref && j != id_track) {
	      color = 255 * (NumImages-1 - j)/((float)NumImages);
        //Draw a line between the two points :
        KeypointFeature * pt0 = ((KeypointFeature*)ref);
        KeypointFeature * pt1 = ((KeypointFeature*)f);
        DrawLine(pt0->x(), pt0->y(),
                 pt1->x(), pt1->y(),
                 color,
                 &imageArrayBytes);
      }
    }
    features.operator++();
  }
}

void SaveImage(const ByteImage &imageArrayBytes,
               const std::string out_file_path,
               const std::string file_suffix) {
  std::string s = out_file_path;
  size_t index_dot = s.find_last_of(".");
  std::string ext = s.substr(index_dot);
  s.erase(index_dot,s.size());
  s.append(file_suffix);
  s.append(ext);
  WriteImage (imageArrayBytes, s.c_str());
}

void BlendImages(const ByteImage &imageArrayBytesA,
                 const ByteImage &imageArrayBytesB,
                 ByteImage &imageArrayBytesOut,
                 float alpha = 0.2) {
  unsigned int h = std::max(imageArrayBytesA.Height(),
                            imageArrayBytesB.Height());
  unsigned int w = std::max(imageArrayBytesA.Width(),
                            imageArrayBytesB.Width());
  unsigned int d = std::max(imageArrayBytesA.Depth(),
                            imageArrayBytesB.Depth());
  imageArrayBytesOut.Resize(h, w, d);
  imageArrayBytesOut.Fill(0);

  size_t dA=0,dB=0;
  if (imageArrayBytesA.Depth() == 3) dA = 1;
  if (imageArrayBytesB.Depth() == 3) dB = 1;

  for(size_t j=0; j < h; ++j)
    for(size_t i=0; i < w; ++i) {
      imageArrayBytesOut(j,i,0) = (1 - alpha) * imageArrayBytesA(j,i,0)
       + alpha * imageArrayBytesB(j,i,0);
      if (d == 3) {
        imageArrayBytesOut(j,i,1) = (1 - alpha) * imageArrayBytesA(j,i,dA)
         + alpha * imageArrayBytesB(j,i,dB);

        imageArrayBytesOut(j,i,2) = (1 - alpha) * imageArrayBytesA(j,i,2*dA)
         + alpha * imageArrayBytesB(j,i,2*dB);
      }
    }
}

void DisplayMatches(Matches &matches) {
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

ByteImage * ConvertToGrayscale(const ByteImage &imageArrayBytes) {
  ByteImage *arrayGrayBytes = NULL;
  // Grayscale image convertion
  if (imageArrayBytes.Depth() == 3) {
    arrayGrayBytes = new ByteImage ();
    Rgb2Gray<ByteImage, ByteImage>(imageArrayBytes, arrayGrayBytes);
  } else {
    //TODO(julien) Useless: don't copy an already grayscale image
    arrayGrayBytes = new ByteImage (imageArrayBytes);
  }
  return arrayGrayBytes;
}

bool IsArgImage(const std::string & arg) {
  return  (arg.rfind (".png") != std::string::npos ||
           arg.rfind (".jpg") != std::string::npos ||
           arg.rfind (".jpeg") != std::string::npos ||
           arg.rfind (".pnm") != std::string::npos );
}

void ProceedReconstruction(Matches &matches, 
                           vector<std::pair<size_t, size_t> > &image_sizes) {
  Reconstruction reconstruct;
  Mat3 K1, K2;
  size_t index_image = 0;
  std::set<Matches::ImageID>::const_iterator image_iter =
    matches.get_images().begin();
  Matches::ImageID previous_image_id = *image_iter;
    
  double u = image_sizes[index_image].second/2.0;
  if (FLAGS_principal_point_u > 0)
    u = FLAGS_principal_point_u;
  double v = image_sizes[index_image].first/2.0;
  if (FLAGS_principal_point_v > 0)
    v = FLAGS_principal_point_v;
  // The first image is fixed
  K1 << FLAGS_focal,  0, u,
        0, FLAGS_focal,  v, 
        0, 0,            1;
      
  // Estimation of the second image
  image_iter++;
  index_image++;
  
  u = image_sizes[index_image].second/2.0;
  if (FLAGS_principal_point_u > 0)
    u = FLAGS_principal_point_u;
  v = image_sizes[index_image].first/2.0;
  if (FLAGS_principal_point_v > 0)
    v = FLAGS_principal_point_v;
  K2 << FLAGS_focal,  0, u,
        0, FLAGS_focal,  v,
        0,  0,           1;

  LOG(INFO) << " -- Initial Motion Estimation --  " << std::endl;
  ReconstructFromTwoCalibratedViews(matches, previous_image_id, *image_iter,
                                    K1, K2,
                                    &matches, &reconstruct);
  
  LOG(INFO) << " -- Initial Intersection --  " << std::endl;
  size_t minimum_num_views = 2;
  PointStructureTriangulation(matches, *image_iter, minimum_num_views, 
                              &reconstruct);

  //LOG(INFO) << " -- Bundle Adjustment --  " << std::endl;
  //TODO (julien) Perfom Bundle Adjustment (Euclidean BA)
  
  // Estimation of the pose of other images by resection
  image_iter++;
  index_image++;
  for (; image_iter != matches.get_images().end();
       ++image_iter, ++index_image) {
    u = image_sizes[index_image].second/2.0;
    if (FLAGS_principal_point_u > 0)
      u = FLAGS_principal_point_u;
    v = image_sizes[index_image].first/2.0;
    if (FLAGS_principal_point_v > 0)
      v = FLAGS_principal_point_v;
    K1 << FLAGS_focal,  0, u,
          0, FLAGS_focal,  v,
          0,  0,           1;
                       
    LOG(INFO) << " -- Incremental Resection --  " << std::endl;
    CalibratedCameraResection(matches, *image_iter, K1,
                             &matches, &reconstruct);     

    LOG(INFO) << " -- Incremental Intersection --  " << std::endl;
    size_t minimum_num_views = 3;
    PointStructureTriangulation(matches, *image_iter, minimum_num_views, 
                                &reconstruct);
    
    //LOG(INFO) << " -- Bundle Adjustment --  " << std::endl;
    //TODO (julien) Perfom Bundle Adjustment (Euclidean BA)
  }
  
  ExportToPLY(reconstruct, "./out.ply");
  reconstruct.ClearCamerasMap();
  reconstruct.ClearStructuresMap();
}

int main (int argc, char *argv[]) {
  google::SetUsageMessage("Track a sequence.");
  google::ParseCommandLineFlags(&argc, &argv, true);

  std::list<std::string> image_list;
  vector<std::pair<size_t, size_t> > image_sizes;

  for (int i = 1;i < argc;++i) {
    std::string arg (argv[i]);
    if (IsArgImage(arg)) {
      image_list.push_back(arg);
    }
  }

  bool is_keep_new_detected_features = true;
  bool is_patch_tracking_mode = false;

  //track patch mode
  if (!FLAGS_patch.empty()) {
    is_keep_new_detected_features = false;
    is_patch_tracking_mode = true;
    LOG(INFO) << "Patch traking mode activated. "<<std::endl;
  }

  // Create the tracker
  correspondence::ArrayMatcher<float> *matcher  = NULL;
  // Set the detector
  detector::eDetector edetector = detector::FAST_DETECTOR;
  std::map<std::string, detector::eDetector> detectorMap;
  detectorMap["FAST"] = detector::FAST_DETECTOR;
  detectorMap["SURF"] = detector::SURF_DETECTOR;
  detectorMap["STAR"] = detector::STAR_DETECTOR;
  detectorMap["MSER"] = detector::MSER_DETECTOR;
  if (detectorMap.find(FLAGS_detector) != detectorMap.end()) {
    edetector = detectorMap[FLAGS_detector];
  } else {
    LOG(FATAL) << "ERROR : undefined Detector !";
  }
  detector::Detector * pDetector = detectorFactory(edetector);
  
  // Set the descriptor
  descriptor::eDescriber edescriber = descriptor::DAISY_DESCRIBER;
  std::map<std::string, descriptor::eDescriber> descriptorMap;
  descriptorMap["SIMPLIEST"] = descriptor::SIMPLEST_DESCRIBER;
  descriptorMap["SURF"]      = descriptor::SURF_DESCRIBER;
  descriptorMap["DIPOLE"]    = descriptor::DIPOLE_DESCRIBER;
  descriptorMap["DAISY"]     = descriptor::DAISY_DESCRIBER;
  if (descriptorMap.find(FLAGS_describer) != descriptorMap.end()) {
    edescriber = descriptorMap[FLAGS_describer];
  } else {
    LOG(FATAL) << "ERROR : undefined Describer !";
  }
  descriptor::Describer * pDescriber = describerFactory(edescriber);

  // Set the matcher
  matcher = new correspondence::ArrayMatcher_Kdtree<float>();

  libmv::tracker::FeaturesGraph all_features_graph;
  tracker::Tracker *points_tracker = NULL;
  if (!FLAGS_robust_tracker) {
    points_tracker = new tracker::Tracker(pDetector, pDescriber, matcher);
  } else {
    tracker::RobustTracker * r_tracker =
     new tracker::RobustTracker(pDetector, pDescriber, matcher);
    r_tracker->set_rms_threshold_inlier(FLAGS_robust_tracker_threshold);
    points_tracker = r_tracker;
  }

  // Track the sequence of images

  if (is_patch_tracking_mode) {
    std::string image_path = FLAGS_patch;
    LOG(INFO) << "Tracking patch '"<< image_path << "'" << std::endl;
    ByteImage imageArrayBytes;
    ReadImage (image_path.c_str(), &imageArrayBytes);
    ByteImage *arrayGrayBytes = ConvertToGrayscale(imageArrayBytes);
    Image image(arrayGrayBytes);

    image_sizes.push_back(std::pair<size_t,size_t>(
     arrayGrayBytes->Height(), arrayGrayBytes->Width()));

    libmv::tracker::FeaturesGraph new_features_graph;
    libmv::Matches::ImageID new_image_id = 0;
    points_tracker->Track(image,
                          all_features_graph,
                          &new_features_graph,
                          &new_image_id,
                          true);
    LOG(INFO) << "#Patch Tracks "<< new_features_graph.matches_.NumTracks()
      << std::endl;

    all_features_graph.Merge(new_features_graph);
  }
  // Track the sequence of images
  size_t image_index = 0;
  std::list<std::string>::iterator image_list_iterator = image_list.begin();
  for (; image_list_iterator != image_list.end(); ++image_list_iterator) {
    std::string image_path = (*image_list_iterator);

    LOG(INFO) << "Tracking image '"<< image_path << "'" << std::endl;
    ByteImage imageArrayBytes;
    ReadImage (image_path.c_str(), &imageArrayBytes);
    ByteImage *arrayGrayBytes = ConvertToGrayscale(imageArrayBytes);

    Image image(arrayGrayBytes);

    image_sizes.push_back(std::pair<size_t,size_t>(
      arrayGrayBytes->Height(), arrayGrayBytes->Width()));

    libmv::tracker::FeaturesGraph new_features_graph;
    libmv::Matches::ImageID new_image_id = 0;
    points_tracker->Track(image,
                          all_features_graph,
                          &new_features_graph,
                          &new_image_id,
                          is_keep_new_detected_features);
    LOG(INFO) << "#New Tracks "<< new_features_graph.matches_.NumTracks()
      << std::endl;

    if (!is_patch_tracking_mode)
      all_features_graph.Merge(new_features_graph);

    if (FLAGS_save_features || FLAGS_save_matches) {
      Matches::Features<PointFeature> features_set =
        new_features_graph.matches_.InImage<PointFeature>(new_image_id);

      if (FLAGS_save_features)
        DrawFeatures(imageArrayBytes, features_set, false);
      if (FLAGS_save_matches)
        DrawMatches(imageArrayBytes, new_image_id, all_features_graph);

      SaveImage(imageArrayBytes, image_path, "-features");
    }

    if (!is_keep_new_detected_features) {
      new_features_graph.Clear();
    }

    LOG(INFO) << "#All Tracks "<< all_features_graph.matches_.NumTracks()
      << std::endl;
    LOG(INFO) << "#All Images "<< all_features_graph.matches_.NumImages()
      << std::endl;
    image_index++;
  }

  DisplayMatches(all_features_graph.matches_);
  // Estimates the camera trajectory and 3D structure of the scene
  if (FLAGS_pose_estimation)  {
    ProceedReconstruction(all_features_graph.matches_, image_sizes);     
  }
  
  // Delete the tracker
  if (points_tracker) {
    delete points_tracker;
    points_tracker = NULL;
  }
  // Delete the features graph
  all_features_graph.Clear();

  //TODO(julien) Clean the variables detector, describer, matcher
  return 0;
}
