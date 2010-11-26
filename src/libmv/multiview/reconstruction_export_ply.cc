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

#include <fstream>
#include <locale.h>

#include "libmv/multiview/reconstruction_export_ply.h"

namespace libmv {

void ExportToPLY(const Reconstruction &reconstruct, std::string out_file_name) {
  std::ofstream outfile;
  outfile.open(out_file_name.c_str(), std::ios_base::out);
  if (outfile.is_open()) {
    outfile << "ply" << std::endl;
    outfile << "format ascii 1.0" << std::endl;
    outfile << "comment Made by libmv authors" << std::endl;
    outfile << "comment 3D points structure:" << std::endl;
    outfile << "element vertex " << reconstruct.GetNumberStructures() 
      << std::endl;
    outfile << "property float x" << std::endl;
    outfile << "property float y" << std::endl;
    outfile << "property float z" << std::endl;
    outfile << "property uchar red" << std::endl;
    outfile << "property uchar green" << std::endl;
    outfile << "property uchar blue" << std::endl;
    outfile << "comment Cameras positions:" << std::endl;
    outfile << "element vertex " << reconstruct.GetNumberCameras() << std::endl;
    outfile << "property float x" << std::endl;
    outfile << "property float y" << std::endl;
    outfile << "property float z" << std::endl;
    outfile << "property uchar red" << std::endl;
    outfile << "property uchar green" << std::endl;
    outfile << "property uchar blue" << std::endl;
    outfile << "end_header" << std::endl;
    std::map<StructureID, Structure *>::const_iterator track_iter =
      reconstruct.structures().begin();
    for (; track_iter != reconstruct.structures().end(); ++track_iter) {
      PointStructure * point_s = 
        dynamic_cast<PointStructure*>(track_iter->second);
      if (point_s) {
        // Exports the point affine position
        outfile << point_s->coords_affine().transpose() << " ";
        // Exports the point color
        outfile << "255 255 255" << std::endl;
      }
    }
    std::map<CameraID, Camera *>::const_iterator camera_iter =  
      reconstruct.cameras().begin();
    for (; camera_iter != reconstruct.cameras().end(); ++camera_iter) {
      PinholeCamera * camera_pinhole =  
        dynamic_cast<PinholeCamera *>(camera_iter->second);
      if (camera_pinhole) {
        // Exports the camera position
        outfile << camera_pinhole->position().transpose() << " ";
        // Exports the camera color
        outfile << "255 0 0" << std::endl;
      }
    }
    outfile.close();
  }
}
} // namespace libmv
