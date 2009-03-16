// Copyright (c) 2007, 2008 libmv authors.
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


#ifndef UI_TVR_TVR_DATA_H_
#define UI_TVR_TVR_DATA_H_

#include <vector>

#include "ui/tvr/features.h"

struct TvrDocument {
  QImage images[2];
  SurfFeatureSet feature_sets[2];
  libmv::Correspondences correspondences;
  libmv::Mat3 F;
  double focal_distance[2];
  libmv::Mat3 K[2];
  libmv::Mat3 R[2];
  libmv::Vec3 t[2];
  std::vector<libmv::Vec3> X;


  void SaveAsBlender(const char *filename) {
    FILE* fid = fopen(filename, "wb");
    assert(fid);

    fprintf(fid, "# libmv blender camera track export; do not edit\n");
    fprintf(fid, "import Blender\n");
    fprintf(fid, "from Blender import Camera, Object, Scene, NMesh\n");
    fprintf(fid, "from Blender import Mathutils\n");
    fprintf(fid, "from Blender.Mathutils import *\n");
    fprintf(fid, "cur = Scene.GetCurrent()\n");

    fprintf(fid, "# Cameras\n");
    for (int i = 0; i < 2; ++i) {
      // Set up the camera
      fprintf(fid, "c%04d = Camera.New('persp')\n", i);

      // TODO(pau) we may want to take K(0,0)/K(1,1) and push that into the
      // render aspect ratio settings.
      double f = K[i](0,0);
      double width = images[0].width();
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
          fprintf(fid, "%g,", R[i](j,k)); // transposed!
        }
        fprintf(fid, "0.0],");
      }
      libmv::Vec3 optical_center = -R[i].transpose() * t[i];
      fprintf(fid, "[");
      for (int j = 0; j < 3; ++j)
        fprintf(fid, "%g,", optical_center(j));
      fprintf(fid, "1.0]))\n");

      // K matrix; not used by blender
      fprintf(fid, "k%04d = [", i);
      for (int j=0; j<3; j++) {
        fprintf(fid, "[");
        for (int k=0; k<3; k++) {
          fprintf(fid, "%g,", K[i](j,k)); 
        }
        fprintf(fid, "],");
      }
      fprintf(fid, "]\n");

      // Link the scene and the camera together
      fprintf(fid, "o%04d.link(c%04d)\n\n", i, i);
      fprintf(fid, "cur.link(o%04d)\n\n", i);
    }

    // IPO curves
    fprintf(fid, "# Animate the camera with IpoCurves\n");
    fprintf(fid, "crender = Camera.New('persp')\n");
    fprintf(fid, "crender.lens = 35.0 # this lens value doesn't matter\n");
    fprintf(fid, "crender.setDrawSize(0.05)\n");
    fprintf(fid, "orender = Object.New('Camera')\n");
    fprintf(fid, "orender.name = \"libmv_render_cam\"\n");
    fprintf(fid, "orender.link(crender)\n");
    fprintf(fid, "cur.link(orender)\n");
    fprintf(fid, "cur.setCurrentCamera(orender)\n");
    fprintf(fid, "ipo = Blender.Ipo.New('Object','render_cam_objipo')\n");
    fprintf(fid, "orender.setIpo(ipo)\n");
    fprintf(fid, "locx = ipo.addCurve('LocX')\n");
    fprintf(fid, "locx.setInterpolation('Linear')\n");
    fprintf(fid, "locy = ipo.addCurve('LocY')\n");
    fprintf(fid, "locy.setInterpolation('Linear')\n");
    fprintf(fid, "locz = ipo.addCurve('LocZ')\n");
    fprintf(fid, "locz.setInterpolation('Linear')\n");
    fprintf(fid, "rotx = ipo.addCurve('RotX')\n");
    fprintf(fid, "rotx.setInterpolation('Linear')\n");
    fprintf(fid, "roty = ipo.addCurve('RotY')\n");
    fprintf(fid, "roty.setInterpolation('Linear')\n");
    fprintf(fid, "rotz = ipo.addCurve('RotZ')\n");
    fprintf(fid, "rotz.setInterpolation('Linear')\n");
    fprintf(fid, "camipo = Blender.Ipo.New('Camera','render_cam_camipo')\n");
    fprintf(fid, "crender.setIpo(camipo)\n");
    fprintf(fid, "lenscurve = camipo.addCurve('Lens')\n");
    fprintf(fid, "lenscurve.setInterpolation('Linear')\n");
    fprintf(fid, "\n");
    for (int i = 0; i < 2; ++i) {
      fprintf(fid, "locx.addBezier((%d,o%04d.LocX))\n", i, i);
      fprintf(fid, "locy.addBezier((%d,o%04d.LocY))\n", i, i);
      fprintf(fid, "locz.addBezier((%d,o%04d.LocZ))\n", i, i);
      fprintf(fid, "rotx.addBezier((%d,o%04d.RotX*18/3.141593))\n", i, i);
      fprintf(fid, "roty.addBezier((%d,o%04d.RotY*18/3.141593))\n", i, i);
      fprintf(fid, "rotz.addBezier((%d,o%04d.RotZ*18/3.141593))\n", i, i);
      fprintf(fid, "lenscurve.addBezier((%d,c%04d.lens))\n", i, i);
      fprintf(fid, "\n");
    }
    fprintf(fid, "\n");

    fprintf(fid, "allob=Object.Get()\n");
    fprintf(fid, "for o in allob:\n");
    fprintf(fid, "   print o.name[:10]\n");
    fprintf(fid, "   if o.name.startswith('libmv_cam'):\n");
    fprintf(fid, "	 cur.unlink(o)\n");
    fprintf(fid, "\n");

    fprintf(fid, "# Point cloud\n");
    fprintf(fid, "ob=Object.New('Mesh','libmv_point_cloud')\n");
    fprintf(fid, "ob.setLocation(0.0,0.0,0.0)\n");
    fprintf(fid, "mesh=ob.getData()\n");
    fprintf(fid, "cur.link(ob)\n");
    for (int i = 0; i < X.size(); ++i) {
      fprintf(fid, "v = NMesh.Vert(%g,%g,%g)\n", X[i](0), X[i](1), X[i](2));
      fprintf(fid, "mesh.verts.append(v)\n");
    }
    fprintf(fid, "mesh.update()\n");
    fprintf(fid, "cur.update()\n\n");

    fprintf(fid, "# Add a helper object to help manipulating joined camera and"
                 "points\n");
    fprintf(fid, "scene_dummy = Object.New('Empty','libmv_scene')\n");
    fprintf(fid, "scene_dummy.setLocation(0.0,0.0,0.0)\n");
    fprintf(fid, "cur.link(scene_dummy)\n");
    // FIXME orender is from IPO... do IPO curves!
    //	fprintf(fid, "scene_dummy.makeParent([orender,ob])\n");
    fprintf(fid, "scene_dummy.makeParent([ob])\n");
    for (int i = 0; i < 2; ++i) {
      fprintf(fid, "scene_dummy.makeParent([o%04d])\n", i);
    }
    fprintf(fid, "\n");
    //	fprintf(fid, "scene_dummy.setEuler((-3.141593/2, 0.0, 0.0))\n");
    fprintf(fid, "scene_dummy.SizeX=1.0\n");
    fprintf(fid, "scene_dummy.SizeY=1.0\n");
    fprintf(fid, "scene_dummy.SizeZ=1.0\n");

    fclose(fid);
  }
};

#endif //UI_TVR_TVR_DATA_H_
