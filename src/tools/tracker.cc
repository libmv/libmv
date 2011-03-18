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
#include "libmv/correspondence/export_matches_txt.h"
#include "libmv/correspondence/import_matches_txt.h"
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
#include "libmv/reconstruction/reconstruction.h"
#include "libmv/reconstruction/euclidean_reconstruction.h"
#include "libmv/reconstruction/export_ply.h"
#include "libmv/reconstruction/export_blender.h"
#include "libmv/reconstruction/mapping.h"
#include "libmv/reconstruction/optimization.h"
#include "libmv/multiview/robust_fundamental.h"
#include "libmv/multiview/triangulation.h"
#include "libmv/numeric/numeric.h"
#include "libmv/tools/revision.h"
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
DEFINE_bool  (non_robust_tracker, false,
              "perform a non robust tracking (without epipolar filtering)");
DEFINE_double(robust_tracker_threshold, 1.0,
              "Epipolar filtering threshold (in pixels)");

DEFINE_bool  (track_all_known_features, false,
              "track all known features in all images");

DEFINE_bool  (pose_estimation, false,
              "perform a pose estimation");
DEFINE_double(focal, 50,
              "focale length for all the cameras");
DEFINE_double(principal_point_u, 0,
              "principal point u coordinate");
DEFINE_double(principal_point_v, 0,
              "principal point v coordinate");

DEFINE_string(o, "matches.txt", "Matches output file");

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
  std::set<Matches::ImageID>::const_iterator iter_i;
  std::set<Matches::TrackID>::const_iterator iter_t;
  std::cout << "Matches : \t\t"<<std::endl << "\t";
  iter_i = matches.get_images().begin();
  for (; iter_i != matches.get_images().end();++iter_i) {
    std::cout << *iter_i << " ";
  }
  std::cout << std::endl;
  iter_t = matches.get_tracks().begin();
  for (; iter_t != matches.get_tracks().end();++iter_t) {
    std::cout << *iter_t <<"\t";
    iter_i = matches.get_images().begin();
    for (; iter_i != matches.get_images().end();++iter_i) {
      const Feature * f = matches.Get(*iter_i, *iter_t);
      if (f)
        std::cout << "X ";
      else
        std::cout << "  ";
    }
    std::cout << std::endl;
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
                           Vec2 &image_size) {
  VLOG(1) << "Euclidean Reconstruction From Video..." << std::endl;
  std::list<Reconstruction *> reconstructions;
  EuclideanReconstructionFromVideo(matches, 
                                   image_size(0), 
                                   image_size(1),
                                   FLAGS_focal,
                                   &reconstructions);
  VLOG(1) << "Euclidean Reconstruction From Video...[DONE]" << std::endl;
  int i = 0;
  std::list<Reconstruction *>::iterator iter = reconstructions.begin();
  for (; iter != reconstructions.end(); ++iter) {
    std::stringstream s;
    if (reconstructions.size() > 1)
      s << "./out" << "-" << i << ".py";
    else
      s << "./out.py";
    ExportToBlenderScript(**iter, s.str());
  }
  // Cleaning
  VLOG(2) << "Cleaning." << std::endl;
  iter = reconstructions.begin();
  for (; iter != reconstructions.end(); ++iter) {
    (*iter)->ClearCamerasMap();
    (*iter)->ClearStructuresMap(); 
    delete *iter;
  }
  reconstructions.clear();
}

int main (int argc, char *argv[]) {
  std::string usage ="Track salient 2D points in a sequence of images.\n";
  usage += "It uses an incremental approach (an image is matched only with \
  its 2 neighbors.\n";
  usage += "Usage: " + std::string(argv[0]) + " *.png -o {MATCHESFILE}.txt.\n";
  usage += "Using libmv v" + std::string(LIBMV_VERSION) +"\n";
  //usage += argv[0] + "<detector>";
  google::SetUsageMessage(usage);
  google::ParseCommandLineFlags(&argc, &argv, true);

  std::list<std::string> image_list;
  vector<std::pair<size_t, size_t> > image_sizes;

  std::string arg_lw;
  for (int i = 1;i < argc;++i) {
    std::string arg(argv[i]);
    arg_lw.resize(arg.length());
    std::transform(arg.begin(), arg.end(), arg_lw.begin(), ::tolower);
    if (IsArgImage(arg_lw)) {
      image_list.push_back(arg);
    }
  }

  bool is_keep_new_detected_features = true;

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

  tracker::Tracker *points_tracker = NULL;
  if (FLAGS_non_robust_tracker) {
    points_tracker = new tracker::Tracker(pDetector, pDescriber, matcher);
  } else {
    tracker::RobustTracker * r_tracker =
     new tracker::RobustTracker(pDetector, pDescriber, matcher);
    r_tracker->set_rms_threshold_inlier(FLAGS_robust_tracker_threshold);
    points_tracker = r_tracker;
  }
 
  // Track the sequence of images
  libmv::tracker::FeaturesGraph all_features_graph;
  libmv::tracker::FeaturesGraph current_previous_fg[2];
  int i_prev = 0, i_new = 1;
  size_t image_index = 0;
  std::list<std::string>::iterator image_list_iterator = image_list.begin();
  for (; image_list_iterator != image_list.end(); ++image_list_iterator) {
    std::string image_path = (*image_list_iterator);

    VLOG(1) << "Tracking image '"<< image_path << "'" << std::endl;
    ByteImage imageArrayBytes;
    ReadImage (image_path.c_str(), &imageArrayBytes);
    ByteImage *arrayGrayBytes = ConvertToGrayscale(imageArrayBytes);
    Image image(arrayGrayBytes);
    image_sizes.push_back(std::pair<size_t,size_t>(
      arrayGrayBytes->Height(), arrayGrayBytes->Width()));

    libmv::Matches::ImageID new_image_id = 0;
    if (FLAGS_track_all_known_features) {
      // TODO(julien) this case is not a simple tracker and should be moved
      // to another tool?
      VLOG(1) << "#Tracks Searched "<<
        all_features_graph.matches_.NumTracks() << std::endl;
      points_tracker->Track(image,
                            all_features_graph,
                            &current_previous_fg[i_new],
                            &new_image_id,
                            is_keep_new_detected_features);
    } else {
      VLOG(1) << "#Tracks Searched "<<
        current_previous_fg[i_prev].matches_.NumTracks() << std::endl;
      points_tracker->Track(image,
                            current_previous_fg[i_prev],
                            &current_previous_fg[i_new],
                            &new_image_id,
                            is_keep_new_detected_features);
      
    }
    current_previous_fg[i_prev].Clear();
    all_features_graph.Merge(current_previous_fg[i_new]);
    VLOG(1) << " New Image ID "<< new_image_id << std::endl;
    VLOG(1) << "#New Tracks "<< current_previous_fg[i_new].matches_.NumTracks()
      << std::endl;

    if (FLAGS_save_features || FLAGS_save_matches) {
      Matches::Features<PointFeature> features_set =
        current_previous_fg[i_new].matches_.InImage<PointFeature>(new_image_id);

      if (FLAGS_save_features)
        DrawFeatures(imageArrayBytes, features_set, false);
      if (FLAGS_save_matches)
        DrawMatches(imageArrayBytes, new_image_id, all_features_graph);

      SaveImage(imageArrayBytes, image_path, "-features");
    }

    VLOG(1) << "#All Tracks "<< all_features_graph.matches_.NumTracks()
      << std::endl;
    VLOG(1) << "#All Images "<< all_features_graph.matches_.NumImages()
      << std::endl;
    image_index++;
    i_prev = i_prev ^ 1;
    i_new  = i_prev ^ 1;
  }

  // Exports all matches
  ExportMatchesToTxt(all_features_graph.matches_, FLAGS_o);
  DisplayMatches(all_features_graph.matches_);
  
  // Estimates the camera trajectory and 3D structure of the scene
  if (FLAGS_pose_estimation)  {
    Vec2 image_size;
    image_size << image_sizes[0].first, image_sizes[0].second;
    // TODO(julien) all images are supposed to have the same size 
    //              for this tracker
    ProceedReconstruction(all_features_graph.matches_, image_size);     
  }
  
  // Delete the tracker
  if (points_tracker) {
    delete points_tracker;
    points_tracker = NULL;
  }
  // Delete the features graph
  all_features_graph.DeleteAndClear();

  // TODO(julien) Clean the variables detector, describer, matcher
  return 0;
}
