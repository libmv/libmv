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

#include "libmv/reconstruction/euclidean_reconstruction.h"
#include "libmv/reconstruction/export_blender.h"
#include "libmv/reconstruction/export_ply.h"
#include "libmv/reconstruction/image_order_selection.h"
#include "libmv/reconstruction/mapping.h"
#include "libmv/reconstruction/optimization.h"
#include "libmv/reconstruction/reconstruction.h"

namespace libmv {

bool EuclideanReconstructionFromVideo(
      const Matches &matches, 
      int image_width, 
      int image_height,
      double focal,
      std::list<Reconstruction *> *reconstructions) {
  PinholeCamera * camera = NULL;
  
  Vec2u image_size;
  image_size << image_width, image_height;
  
  double cu = image_width/2 - 0.5, 
         cv = image_height/2 - 0.5;
  Mat3 K;
  K << focal, 0, cu,
       0, focal, cv,
       0,   0,   1;
  
  std::list<libmv::vector<Matches::ImageID> > connected_graph_list;
  SelectKeyframes(matches,  &connected_graph_list);
  
  VLOG(2) << " List order: ";
  for (size_t i = 0; i < connected_graph_list.begin()->size(); ++i) {
    VLOG(2) << (*connected_graph_list.begin())[i] << " ";
  }
  VLOG(2) << std::endl;
  
  Matches matches_inliers;
  size_t image_id = 0;
  size_t index_image_graph = 0;
  Reconstruction *recons = NULL;
  std::list<libmv::vector<Matches::ImageID> >::iterator graph_iter =
    connected_graph_list.begin();
  for (; graph_iter != connected_graph_list.end(); ++graph_iter) {
    if (graph_iter->size() >= 2) {
      recons = new Reconstruction();
      reconstructions->push_back(recons);
      VLOG(2) << " -- Initial Motion Estimation --  " << std::endl;
      ReconstructFromTwoCalibratedViews(matches, 
                                        (*graph_iter)[0], 
                                        (*graph_iter)[1], 
                                        K, K,
                                        &matches_inliers, 
                                        recons);
      index_image_graph = 0;
      image_id = (*graph_iter)[index_image_graph];
      camera = dynamic_cast<PinholeCamera*>(
        recons->GetCamera(image_id));
      if (camera) {
        camera->set_image_size(image_size); 
      }
      index_image_graph = 1;
      image_id = (*graph_iter)[index_image_graph];
      camera = dynamic_cast<PinholeCamera*>(
        recons->GetCamera(image_id));
      if (camera) {
        camera->set_image_size(image_size); 
      }
      
      VLOG(2) << " -- Initial Intersection --  " << std::endl;
      size_t minimum_num_views = 2;
      PointStructureTriangulation(matches_inliers, 
                                  image_id,
                                  minimum_num_views, 
                                  recons);
      
      // Performs projective bundle adjustment
      VLOG(2) << " -- Bundle adjustment --  " << std::endl;
      MetricBundleAdjust(matches_inliers, recons);
      
      /*
      VLOG(2) << " -- Bundle adjustment (second pass) --  " << std::endl;
      RemoveOutliers(image_id, &matches_inliers, recons, 1.0);
      MetricBundleAdjust(matches_inliers, recons);*/
      
      //std::string s = "out-1.py";
      //ExportToBlenderScript(*recons, s);
      
      // Estimation of the pose of other images by resection
      minimum_num_views = 3;
      for (index_image_graph = 2; index_image_graph < graph_iter->size();
          ++index_image_graph) {
        image_id = (*graph_iter)[index_image_graph];
        VLOG(2) << " -- Incremental Resection --  " << std::endl;
        CalibratedCameraResection(matches, image_id, K,
                                  &matches_inliers, recons);    
        // TODO(julien) optimize camera
        
        camera = dynamic_cast<PinholeCamera*>(
          recons->GetCamera(image_id));
        if (camera) {
          camera->set_image_size(image_size); 
        }
        
        VLOG(2) << " -- Incremental Intersection --  " << std::endl;
        PointStructureTriangulation(matches, 
                                    image_id,
                                    minimum_num_views, 
                                    recons);    
        // TODO(julien) optimize only points
        
        // Performs bundle adjustment
        // TODO(julien) maybe BA can be called not for every images..
        VLOG(2) << " -- Bundle adjustment --  " << std::endl;
        MetricBundleAdjust(matches, recons);
        /*
        VLOG(2) << " -- RemoveOutliers --  " << std::endl;
        //RemoveOutliers(image_id, &matches, recons, 2.0);
        VLOG(2) << " -- Bundle adjustment --  " << std::endl;
        MetricBundleAdjust(matches, recons);*/

        //std::stringstream s;
        //s << "out-" << index_image_graph << ".py";
        //ExportToBlenderScript(*recons, s.str());
      }
    }
  }
  return true;
}
  
bool EuclideanReconstructionFromImageSet(
      const Matches &matches, 
      const vector<std::pair<size_t, size_t> > &image_sizes,
      std::list<Reconstruction *> *reconstructions) {
  VLOG(0) << "not yet implemented!" << std::endl;
  assert(0);
  /*
   * 
  SelectEfficientImageOrder(matches,  &connected_graph_list);
  
  Mat3 K1, K2;
  size_t index_image = 0;
  Matches matches_inliers;
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
                                    &matches_inliers, &reconstruction);
  
  LOG(INFO) << " -- Initial Intersection --  " << std::endl;
  size_t minimum_num_views = 2;
  PointStructureTriangulation(matches_inliers, *image_iter, minimum_num_views, 
                              &reconstruction);

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
                             &matches_inliers, &reconstruction);     

    LOG(INFO) << " -- Incremental Intersection --  " << std::endl;
    size_t minimum_num_views = 3;
    PointStructureTriangulation(matches_inliers, 
                                *image_iter,
                                minimum_num_views, 
                                &reconstruction);
    
    LOG(INFO) << " -- Bundle Adjustment --  " << std::endl;
    MetricBundleAdjust(matches_inliers, &reconstruction);
  }
  
  */
  return true;
}
  
} // namespace libmv
