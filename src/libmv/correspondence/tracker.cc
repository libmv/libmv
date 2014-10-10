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

#include "tracker.h"
#include "libmv/base/vector_utils.h"
#include "libmv/correspondence/feature.h"

using namespace libmv;
using namespace tracker;
 
bool Tracker::Track(const Image &image1,
                    const Image &image2, 
                    FeaturesGraph *new_features_graph,
                    bool keep_single_feature) {
  // we detect good features to track
  detector::DetectorData **data = NULL;
  vector<Feature *> features1;
  detector_->Detect(image1, &features1, data);
        
  vector<Feature *> features2;
  detector_->Detect(image2, &features2, data);

  // we compute the feature descriptors on every feature
  detector::DetectorData *detector_data = NULL;
  vector<descriptor::Descriptor *> descriptors1;
  
  describer_->Describe(features1, image1, detector_data, &descriptors1);
  vector<descriptor::Descriptor *> descriptors2;
  describer_->Describe(features2, image2, detector_data, &descriptors2);
  
  // Copy data form generic feature to Keypoints since the matcher is
  // a point matcher
  KeypointFeatureSet *feature_set1 = new_features_graph->CreateNewKeypointFeatureSet();
  feature_set1->features.resize(descriptors1.size());
  for (size_t i = 0; i < descriptors1.size(); i++) {
    KeypointFeature& feature = feature_set1->features[i];
    feature.descriptor = *(descriptor::VecfDescriptor*) descriptors1[i];
    *(PointFeature*)(&feature) = *(PointFeature*)features1[i];
  }
  
  KeypointFeatureSet *feature_set2 = new_features_graph->CreateNewKeypointFeatureSet();
  feature_set2->features.resize(descriptors2.size());
  for (size_t i = 0; i < descriptors2.size(); i++) {
    KeypointFeature& feature = feature_set2->features[i];
    feature.descriptor = *(descriptor::VecfDescriptor*) descriptors2[i];
    *(PointFeature*)(&feature) = *(PointFeature*)features2[i];
  }
  
  // we match them
  //TODO (jmichot) use the matcher_ to match and not the generic function
  //FindCorrespondences(*feature_set1, *feature_set2, correspondences);
  std::map<size_t, size_t> correspondences;
  std::map<size_t, size_t> reverse_correspondences;
  FindCorrespondences(*feature_set1, *feature_set2, &correspondences);  
  
  size_t max_num_track = 0;
  std::map<size_t, size_t>::iterator correspondences_iter =
   correspondences.begin();
  for (; correspondences_iter != correspondences.end();
   correspondences_iter++) {
    new_features_graph->matches_.Insert(0, 
                        max_num_track,
                        &feature_set1->features[correspondences_iter->first]);
    new_features_graph->matches_.Insert(1, 
                        max_num_track,
                        &feature_set2->features[correspondences_iter->second]);
    reverse_correspondences[correspondences_iter->second] =
     correspondences_iter->first;
    max_num_track++;
  }
  
  if (keep_single_feature) {
    for (size_t i = 0; i < feature_set1->features.size(); ++i)
    {
      correspondences_iter = correspondences.find(i);
      if (correspondences_iter == correspondences.end()) {
        new_features_graph->matches_.Insert(0, 
                                            max_num_track,
                                            &feature_set1->features[i]);
        max_num_track++;
      }
    }
    for (size_t i = 0; i < feature_set2->features.size(); ++i)
    {
      correspondences_iter = reverse_correspondences.find(i);
      if (correspondences_iter == reverse_correspondences.end()) {
        new_features_graph->matches_.Insert(1, 
                                            max_num_track,
                                            &feature_set2->features[i]);
        max_num_track++;
      }
    }
  }
  
  DeleteElements(&descriptors1);
  DeleteElements(&descriptors2);
  DeleteElements(&features1);
  DeleteElements(&features2);
  
  return true;
}

bool Tracker::Track(const Image &image, 
                    const FeaturesGraph &known_features_graph, 
                    FeaturesGraph *new_features_graph,
                    Matches::ImageID *image_id,
                    bool keep_single_feature) {
  // we detect good features to track
  detector::DetectorData **data = NULL;
  vector<Feature *> features;
  detector_->Detect(image, &features, data);
  
  // we compute the feature descriptors on every feature
  detector::DetectorData *detector_data = NULL;
  vector<descriptor::Descriptor *> descriptors;
  describer_->Describe(features, image, detector_data, &descriptors);
  
  // Copy data form generic feature to Keypoints since the matcher is
  // a point matcher
  KeypointFeatureSet *feature_set = new_features_graph->CreateNewKeypointFeatureSet();
  feature_set->features.resize(descriptors.size());
  for (size_t i = 0; i < descriptors.size(); i++) {
    KeypointFeature& feature = feature_set->features[i];
    feature.descriptor = *(descriptor::VecfDescriptor*) descriptors[i];
    *(PointFeature*)(&feature) = *(PointFeature*)features[i];
  }
  if (known_features_graph.matches_.NumImages() == 0)
    *image_id = 0;
  else
    *image_id = known_features_graph.matches_.GetMaxImageID() + 1;
    
  //TODO (jmichot) Use the matcher_ to match and not the generic function
  // FindCorrespondences(*feature_set1, *feature_set2, correspondences);
  //TODO (jmichot) For a rapid tracker, use only one feature 
  // (in the first image for instance) for every track
  std::map<size_t, size_t> reverse_correspondences_all;
  std::set<Matches::ImageID>::const_iterator iter_image =
   known_features_graph.matches_.get_images().begin();
  for (; iter_image != known_features_graph.matches_.get_images().end() 
      && *iter_image != *image_id; ++iter_image) {
    Matches::Features<KeypointFeature> known_features =
     known_features_graph.matches_.InImage<KeypointFeature>(*iter_image);
    KeypointFeatureSet feature_set_known;
    while(known_features) {
      feature_set_known.features.push_back(*known_features.feature());
      known_features.operator++();
    }
    
    std::map<size_t, size_t> correspondences;
    FindCorrespondences(feature_set_known, *feature_set, &correspondences);  
    
    // We insert known matches (as edges) to the graph
    std::map<size_t, size_t>::iterator correspondences_iter =
     correspondences.begin();
    for (; correspondences_iter != correspondences.end();
      correspondences_iter++) {
      known_features =
       known_features_graph.matches_.InImage<KeypointFeature>(*iter_image);
      size_t i = 0;
      while(known_features) {
        if ( i == correspondences_iter->first) {
          KeypointFeature * new_feature = &feature_set->features[
           correspondences_iter->second];
          new_features_graph->matches_.Insert(*image_id, 
                                              known_features.track(),
                                              new_feature);
          reverse_correspondences_all[correspondences_iter->second] =
           correspondences_iter->first;
        }
        known_features.operator++();
        i++;
      }                          
    }
  }
  size_t max_num_track = known_features_graph.matches_.GetMaxTrackID()+1;
  if (known_features_graph.matches_.NumTracks() == 0)
    max_num_track = 0;
  std::map<size_t, size_t>::iterator correspondences_iter;
  
  // We insert known matches (as edges) to the graph
  if (keep_single_feature) {
    for (size_t i = 0; i < feature_set->features.size(); ++i)
    {
      correspondences_iter = reverse_correspondences_all.find(i);
      if (correspondences_iter == reverse_correspondences_all.end()) {
        new_features_graph->matches_.Insert(*image_id, 
                                            max_num_track,
                                            &feature_set->features[i]);
        max_num_track++;
      }
    }
  }
  
  DeleteElements(&descriptors);
  DeleteElements(&features);  
  
  return true;
}
