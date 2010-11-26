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

#include <stdio.h>
#include <locale.h>

#include "libmv/multiview/reconstruction_export_blender.h"

namespace libmv {

void ExportToBlenderScript(const Reconstruction &reconstruct, 
                           std::string out_file_name) {
  // Avoid to have comas instead of dots (foreign lang.) 
  setlocale(LC_ALL, "C");

  FILE* fid = fopen(out_file_name.c_str(), "wb");
  assert(fid);

  fprintf(fid, "# libmv blender camera track export; do not edit\n");
  fprintf(fid, "import Blender\n");
  fprintf(fid, "from Blender import Camera, Object, Scene, NMesh\n");
  fprintf(fid, "from Blender import Mathutils\n");
  fprintf(fid, "from Blender.Mathutils import *\n");

  fprintf(fid, "cur = Scene.GetCurrent()\n");

  //////////////////////////////////////////////////////////////////////////
  // Cameras.
  fprintf(fid, "# Cameras\n");
  PinholeCamera *pcamera = NULL;
  uint i = 0;
  Mat3 K, R; Vec3 t;
  std::map<CameraID, Camera *>::const_iterator camera_iter =  
    reconstruct.cameras().begin();
  for (; camera_iter != reconstruct.cameras().end(); ++camera_iter) {
    pcamera = dynamic_cast<PinholeCamera *>(camera_iter->second);
    // TODO(julien) how to export generic cameras ? 
    if (pcamera) {
      K = pcamera->intrinsic_matrix();
      R = pcamera->orientation_matrix();
      t = pcamera->position();
      // Set up the camera
      fprintf(fid, "c%04d = Camera.New('persp')\n", i);
      // TODO(pau) we may want to take K(0,0)/K(1,1) and push that into the
      // render aspect ratio settings.
      double f = K(0,0);
      double width = pcamera->image_width();
      // WARNING: MAGIC CONSTANT from blender source
      // Look for 32.0 in 'source/blender/render/intern/source/initrender.c'
      double lens = f / width * 32.0;
      fprintf(fid, "c%04d.lens = %g\n", i, lens);
      fprintf(fid, "c%04d.setDrawSize(0.05)\n", i);
      fprintf(fid, "o%04d = Object.New('Camera')\n", i);
      fprintf(fid, "o%04d.name = 'libmv_cam%04d'\n", i, i);

      // Camera world matrix, which is the inverse transpose of the typical
      // 'projection' matrix as generally thought of by vision researchers.
      // Usually it is x = K[R|t]X; but here it is the inverse of R|t stacked
      // on [0,0,0,1], but then in fortran-order. So that is where the
      // weirdness comes from.
      fprintf(fid, "o%04d.setMatrix(Mathutils.Matrix(",i);
      for (int j = 0; j < 3; ++j) {
        fprintf(fid, "[");
        for (int k = 0; k < 3; ++k) {
          // Opengl's camera faces down the NEGATIVE z axis so we have to
          // do a 180 degree X axis rotation. The math works out so that
          // the following conditional nicely implements that.
          if (j == 2 || j == 1)
            fprintf(fid, "%g,", -R(j,k)); // transposed + opposite!
          else
            fprintf(fid, "%g,", R(j,k)); // transposed!
        }
        fprintf(fid, "0.0],");
      }
      libmv::Vec3 optical_center = -R.transpose() * t;
      fprintf(fid, "[");
      for (int j = 0; j < 3; ++j)
        fprintf(fid, "%g,", optical_center(j));
      fprintf(fid, "1.0]))\n");

      // Link the scene and the camera together
      fprintf(fid, "o%04d.link(c%04d)\n\n", i, i);
      fprintf(fid, "cur.link(o%04d)\n\n", i);
      i++;
    }
  }
  uint num_cam_exported = i;

  //////////////////////////////////////////////////////////////////////////
  // Point Cloud.
  fprintf(fid, "# Point cloud\n");
  fprintf(fid, "ob=Object.New('Mesh','libmv_point_cloud')\n");
  fprintf(fid, "ob.setLocation(0.0,0.0,0.0)\n");
  fprintf(fid, "mesh=ob.getData()\n");
  fprintf(fid, "cur.link(ob)\n");
  std::map<StructureID, Structure *>::const_iterator track_iter =
    reconstruct.structures().begin();
  for (; track_iter != reconstruct.structures().end(); ++track_iter) {
    PointStructure * point_s = 
      dynamic_cast<PointStructure*>(track_iter->second);
    if (point_s) {
      fprintf(fid, "v = NMesh.Vert(%g,%g,%g)\n", point_s->coords_affine()(0),
                                                 point_s->coords_affine()(1),
                                                 point_s->coords_affine()(2));
      fprintf(fid, "mesh.verts.append(v)\n");
    }
  }
  fprintf(fid, "mesh.update()\n");
  fprintf(fid, "cur.update()\n\n");

  //////////////////////////////////////////////////////////////////////////
  // Scene node including cameras and points.
  fprintf(fid, "# Add a helper object to help manipulating joined camera and"
                "points\n");
  fprintf(fid, "scene_dummy = Object.New('Empty','libmv_scene')\n");
  fprintf(fid, "scene_dummy.setLocation(0.0,0.0,0.0)\n");
  fprintf(fid, "cur.link(scene_dummy)\n");

  fprintf(fid, "scene_dummy.makeParent([ob])\n");
  for (int i = 0; i < num_cam_exported; ++i) {
    fprintf(fid, "scene_dummy.makeParent([o%04d])\n", i);
  }
  fprintf(fid, "\n");
  //  fprintf(fid, "scene_dummy.setEuler((-3.141593/2, 0.0, 0.0))\n");
  fprintf(fid, "scene_dummy.SizeX=1.0\n");
  fprintf(fid, "scene_dummy.SizeY=1.0\n");
  fprintf(fid, "scene_dummy.SizeZ=1.0\n");

  fclose(fid);
}
} // namespace libmv
