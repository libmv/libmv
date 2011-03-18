// Copyright (c) 2011 libmv authors.
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
#include <list>
#include <string>

#include "libmv/correspondence/import_matches_txt.h"
#include "libmv/correspondence/tracker.h"
#include "libmv/logging/logging.h"
#include "libmv/reconstruction/euclidean_reconstruction.h"
#include "libmv/reconstruction/export_blender.h"
#include "libmv/reconstruction/export_ply.h"

using namespace libmv;

DEFINE_string(i, "matches.txt", "Matches input file");
DEFINE_string(o, "reconstruction.py", "Reconstruction output file");

DEFINE_int32(w, 0, "Image width (px)");
DEFINE_int32(h, 0, "Image height (px)");

DEFINE_double(f, 50,
              "Focale length for all the cameras (px)");

DEFINE_double(u0, 0,
              "Principal point u coordinate (px)");
DEFINE_double(v0, 0,
             "Principal point v coordinate (px)");

void GetFilePathExtention(const std::string &file, 
                          std::string *path_name, 
                          std::string *ext) {
  size_t dot_pos =  file.rfind (".");
  if (dot_pos != std::string::npos) {
    *path_name = file.substr(0, dot_pos);
    *ext = file.substr(dot_pos + 1, file.length() - dot_pos - 1);    
  } else {
    *path_name = file;
    *ext = "";
  }
}


int main (int argc, char *argv[]) {
  std::string usage ="Estimate the camera trajectory using matches.\n";
  usage += "Usage: " + std::string(argv[0]) + " -i INFILE.txt -o OUTFILE.ply.";
  
  //usage += argv[0] + "<detector>";
  google::SetUsageMessage(usage);
  google::ParseCommandLineFlags(&argc, &argv, true);

  // Imports matches
  tracker::FeaturesGraph fg;
  FeatureSet *fs = fg.CreateNewFeatureSet();
  
  VLOG(0) << "Loading Matches file..." << std::endl;
  ImportMatchesFromTxt(FLAGS_i, &fg.matches_, fs);
  VLOG(0) << "Loading Matches file...[DONE]." << std::endl;
  
  // Estimates the camera trajectory and 3D structure of the scene
  int w = FLAGS_w, h = FLAGS_h;
  // HACK to have the good principal point
  if (FLAGS_u0 > 0)
    w = 2 * (FLAGS_u0 + 0.5);
  if (FLAGS_v0 > 0)
    h = 2 * (FLAGS_v0 + 0.5);
  // TODO(julien) put u and v as arguments of EuclideanReconstructionFromVideo
  VLOG(0) << "Euclidean Reconstruction From Video..." << std::endl;
  std::list<Reconstruction *> reconstructions;
  EuclideanReconstructionFromVideo(fg.matches_, 
                                   w, h,
                                   FLAGS_f,
                                   &reconstructions);
  VLOG(0) << "Euclidean Reconstruction From Video...[DONE]" << std::endl;
  
  // Exports the reconstructions
  VLOG(0) << "Exporting Reconstructions..." << std::endl;
  std::string file_path_name, file_ext;
  GetFilePathExtention(FLAGS_o, &file_path_name, &file_ext);
  std::transform(file_ext.begin(), file_ext.end(), file_ext.begin(), ::tolower);
  
  int i = 0;
  std::list<Reconstruction *>::iterator iter = reconstructions.begin();
  if (file_ext == "ply") {
    for (; iter != reconstructions.end(); ++iter) {
      std::stringstream s;
      if (reconstructions.size() > 1)
        s << file_path_name << "-" << i << ".ply";
      else
        s << FLAGS_o;
      ExportToPLY(**iter, s.str());
    }
  } else  if (file_ext == "py") {    
    for (; iter != reconstructions.end(); ++iter) {
      std::stringstream s;
      if (reconstructions.size() > 1)
        s << file_path_name << "-" << i << ".py";
      else
        s << FLAGS_o;
      ExportToBlenderScript(**iter, s.str());
    }
  }
  VLOG(0) << "Exporting Reconstructions...[DONE]" << std::endl;
  
  // Cleaning
  VLOG(0) << "Cleaning." << std::endl;
  iter = reconstructions.begin();
  for (; iter != reconstructions.end(); ++iter) {
    (*iter)->ClearCamerasMap();
    (*iter)->ClearStructuresMap(); 
    delete *iter;
  }
  reconstructions.clear();
  // Delete the features graph
  fg.DeleteAndClear();
  return 0;
}
