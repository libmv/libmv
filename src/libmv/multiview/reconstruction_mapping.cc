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

#include "libmv/multiview/conditioning.h"
#include "libmv/multiview/nviewtriangulation.h"
#include "libmv/multiview/reconstruction_mapping.h"
#include "libmv/multiview/reconstruction_tools.h"

namespace libmv {

uint PointStructureTriangulation(const Matches &matches, 
                                 CameraID image_id, 
                                 size_t minimum_num_views, 
                                 Reconstruction *reconstruction,
                                 vector<StructureID> *new_structures_ids) {
  // Checks that the camera is in reconstruction
  if (!reconstruction->ImageHasCamera(image_id)) {
      VLOG(1)   << "Error: the image " << image_id 
                << " has no camera." << std::endl;
    return 0;
  }
  vector<StructureID> structures_ids;
  Mat2X x_image;
  vector<Mat34> Ps; 
  vector<Vec2> xs; 
  Vec2 x;
  // Selects only the unreconstructed tracks observed in the image
  SelectUnexistingPointStructures(matches, image_id, *reconstruction,
                                  &structures_ids, &x_image);
  VLOG(1)   << "Points structure selected:" << x_image.cols() << std::endl;
  // Computes an isotropic normalization
  Mat3 precond;
  IsotropicPreconditionerFromPoints(x_image, &precond);
  // Selects the point structures that are observed at least in
  // minimum_num_views images (images that have an already localized camera) 
  Mat41 X_world;
  uint number_new_structure = 0;
  if (new_structures_ids)
    new_structures_ids->reserve(structures_ids.size());
  PinholeCamera *camera = NULL;
  for (size_t t = 0; t < structures_ids.size(); ++t) {
    Matches::Features<PointFeature> fp =
      matches.InTrack<PointFeature>(structures_ids[t]);
    Ps.clear();
    xs.clear();
    while (fp) {
      camera = dynamic_cast<PinholeCamera *>(
        reconstruction->GetCamera(fp.image()));
      if (camera) {
        Ps.push_back(precond * camera->projection_matrix()); 
        x << fp.feature()->x(), fp.feature()->y();
        xs.push_back(x);
      }
      fp.operator++();
    }
    if (Ps.size() >= minimum_num_views) {
      Mat2X x(2, xs.size());
      VectorToMatrix<Vec2, Mat2X>(xs, &x);
      Mat xn;
      ApplyTransformationToPoints(x, precond, &xn);
      // TODO(julien) avoid this copy
      x = xn;
      NViewTriangulateAlgebraic<double>(x, Ps, &X_world);
      bool is_inlier = true;
      
      if (isnan(X_world.sum()))
        is_inlier = false;
      
      if (X_world(3,0) == 0)
        is_inlier = false;      
           
      // TODO(julien) detect and remove outliers
      /*
      Vec2 q2; double err = 0;
      HomogeneousToEuclidean((precond.inverse() * Ps[0] * X_world).col(0), &q2);
      Vec3 x3;
      x3 << x.col(0), 1;
      x3 = precond.inverse() * x3;
      err = (x3.block<2,1>(0, 0) - q2).norm();
      VLOG(1)   << "Prj:" << err << "px";
      //if (err > 40)
      //  is_inlier = false;
      HomogeneousToEuclidean((precond.inverse() * Ps[1] * X_world).col(0), &q2);
      x3 << x.col(1), 1;
      x3 = precond.inverse() * x3;
      err = (x3.block<2,1>(0, 0) - q2).norm();
      VLOG(1)   << " - " << err << "px" << std::endl;
      //if (err > 40)
      //  is_inlier = false;*/
      
      if (is_inlier) {
        // Creates an add the point structure to the reconstruction
        PointStructure * p = new PointStructure();
        p->set_coords(X_world.col(0));
        reconstruction->InsertTrack(structures_ids[t], p);
        if (new_structures_ids)
          new_structures_ids->push_back(structures_ids[t]);
        number_new_structure++;
        VLOG(1)   << "Add Point Structure ["
                  << structures_ids[t] <<"] "
                  << p->coords().transpose() << " ("
                  << p->coords().transpose() / p->coords()[3] << ")"
                  << std::endl;
      }
    }
  }
  return number_new_structure;
}

uint PointStructureRetriangulation(const Matches &matches, 
                                   CameraID image_id,  
                                   Reconstruction *reconstruction) {
  // Checks that the camera is in reconstruction
  // Checks that the camera is in reconstruction
  if (!reconstruction->ImageHasCamera(image_id)) {
      VLOG(1)   << "Error: the image " << image_id 
                << " has no camera." << std::endl;
    return 0;
  }
  vector<StructureID> structures_ids;
  Mat2X x_image;
  vector<Mat34> Ps; 
  vector<Vec2> xs; 
  Vec2 x;
  // Selects only the reconstructed structures observed in the image
  SelectExistingPointStructures(matches, image_id, *reconstruction,
                                  &structures_ids, &x_image);
  // Computes an isotropic normalization
  Mat3 precond;
  IsotropicPreconditionerFromPoints(x_image, &precond);
  Mat41 X_world;
  uint number_updated_structure = 0;
  PinholeCamera *camera = NULL;
  PointStructure *pstructure = NULL;
  for (size_t t = 0; t < structures_ids.size(); ++t) {
    Matches::Features<PointFeature> fp =
      matches.InTrack<PointFeature>(structures_ids[t]);
    Ps.clear();
    xs.clear();
    while (fp) {
      camera = dynamic_cast<PinholeCamera *>(
        reconstruction->GetCamera(fp.image()));
      if (camera) {
        Ps.push_back(camera->projection_matrix()); 
        x << fp.feature()->x(), fp.feature()->y();
        xs.push_back(x);
      }
      fp.operator++();
    }
    Mat2X x(2, xs.size());
    VectorToMatrix<Vec2, Mat2X>(xs, &x);
    Mat xn;
    ApplyTransformationToPoints(x, precond, &xn);
    // TODO(julien) avoid this copy
    x = xn;
    NViewTriangulateAlgebraic<double>(x, Ps, &X_world);
    bool is_inlier = true;
    if (isnan(X_world.sum()))
      is_inlier = false;
    if (X_world(3,0) == 0)
      is_inlier = false;
    // TODO(julien) detect and remove outliers
    // Creates an add the point structure to the reconstruction
    pstructure = dynamic_cast<PointStructure *>(
      reconstruction->GetStructure(structures_ids[t]));
    if (is_inlier && pstructure) {
      pstructure->set_coords(X_world.col(0));
      number_updated_structure++;
      VLOG(1)   << "Point structure updated ["
                << structures_ids[t] <<"] "
                << pstructure->coords().transpose() << " ("
                << pstructure->coords().transpose() / pstructure->coords()[3] 
                << ")" << std::endl;
    }
  }
  return number_updated_structure;
}
} // namespace libmv
