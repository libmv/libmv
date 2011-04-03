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
//
// Undistort is a tool for undistorting images using known lens distortion 
// coefficients. It supports radial (up to 5 coef.) and tangential distortions
// (up to 2 coef).
// Undistorted images are saved in the same location as input images, with a
// suffix "_undist" placed at the end of the file name.
// Note: All images must have the same size.
// 
// We use the Brown's distortion model
// Variables:
//   (x,y): 2D point in the image (pixel)
//   (u,v): the undistorted 2D point (pixel)
//   radial_distortion (k1, k2, k3, ...): vector containing the
//   radial distortion
//   tangential_distortion_ (p1, p2): vector containing the
//                                    tangential distortion
//   (cx,cy): camera principal point
//
// Equation:
//  u = x + (x - cx) * (k1 * r^2 + k2 * r^4 +...)
//    + (p1(r^2 + 2(x-cx)^2) + 2p2(x-cx)(y-cy))(1 + p3*r^2 +...)
//  v = y + (y - cy) * (k1 * r^2 + k2 * r^4 +...)
//   + (p2(r^2 + 2(y-cy)^2) + 2p1(x-cx)(y-cy))(1 + p3*r^2 +...)

#include <algorithm>
#include <string>
#include <vector>

#include "libmv/base/scoped_ptr.h"
#include "libmv/camera/lens_distortion.h"
#include "libmv/camera/pinhole_camera.h"
#include "libmv/correspondence/feature.h"
#include "libmv/correspondence/import_matches_txt.h"
#include "libmv/correspondence/matches.h"
#include "libmv/correspondence/tracker.h"
#include "libmv/image/cached_image_sequence.h"
#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_sequence_io.h"
#include "libmv/image/sample.h"
#include "libmv/logging/logging.h"

DEFINE_double(k1, 0,  "Radial distortion coefficient k1 (Brown's model)");
DEFINE_double(k2, 0,  "Radial distortion coefficient k2 (Brown's model)");
DEFINE_double(k3, 0,  "Radial distortion coefficient k3 (Brown's model)");
DEFINE_double(k4, 0,  "Radial distortion coefficient k4 (Brown's model)");
DEFINE_double(k5, 0,  "Radial distortion coefficient k5 (Brown's model)");

DEFINE_double(p1, 0,  "Tangential distortion coefficient p1 (Brown's model)");
DEFINE_double(p2, 0,  "Tangential distortion coefficient p2 (Brown's model)");

DEFINE_double(fx, 0,  "Focal x (in px)");
DEFINE_double(fy, 0,  "Focal y (in px, default: fx)");
DEFINE_double(u0, 0,  "Principal Point u0 (in px, default: width/2)");
DEFINE_double(v0, 0,  "Principal Point v0 (in px, default: height/2)");
DEFINE_double(sk, 0,  "Skew factor");

DEFINE_string(of, "",         "Output folder.");
DEFINE_string(os, "_undist",  "Output file suffix.");

using namespace libmv;

/// TODO(julien) Put this somewhere else...
std::string ReplaceFolder(const std::string &s,
                          const std::string &new_folder) {
  std::string so = s;
  std::string nf = new_folder;
  if (new_folder == "")
    return so;
  
#ifdef WIN32
  size_t n = so.rfind("\\");
  if (n == std::string::npos)
    n = so.rfind("/");
  if (nf.rfind("\\") != nf.size()-1)
    nf.append("\\");
#else
  size_t n = so.rfind("/");
  if (nf.rfind("/") != nf.size()-1)
    nf.append("/");
#endif
    
  if (n != std::string::npos) {
    so.replace(0, n+1, nf);
  } else {
    so = nf; 
    so.append(s);
  }
  return so;
}

int main(int argc, char **argv) {
  std::string usage ="Undistort images using know distortion parameters.\n";
  usage += "Usage: " + std::string(argv[0]) + " IMAGE1 [IMAGE2 ... IMAGEN] ";
  usage += "[-k1 DOUBLE [-k2 DOUBLE] ... [-p1 DOUBLE [-p2 DOUBLE]]] ";
  usage += "-fx DOUBLE -fy DOUBLE [-u0 DOUBLE -v0 DOUBLE] [-sk DOUBLE]]\n";
  usage += "\t * IMAGEX is an image {PNG, PNM, JPEG}\n";
  google::SetUsageMessage(usage);
  google::ParseCommandLineFlags(&argc, &argv, true);

  // This is not the place for this. I am experimenting with what sort of API
  // will be convenient for the tracking base classes.
  std::vector<std::string> files;
  for (int i = 1; i < argc; ++i) {
    files.push_back(argv[i]);
  }
  
  LensDistortion lens_distortion;
  if (FLAGS_k5 == 0) {
    if (FLAGS_k4 == 0) {
      if (FLAGS_k3 == 0) {
        if (FLAGS_k2 == 0) {
          if (FLAGS_k1 != 0) {
            Vec radial_k(1);
            radial_k << FLAGS_k1;
            lens_distortion.set_radial_distortion(radial_k);
          }
        } else {
          Vec radial_k(2);
          radial_k << FLAGS_k1, FLAGS_k2;
          lens_distortion.set_radial_distortion(radial_k);
        }
      } else {
        Vec radial_k(3);
        radial_k << FLAGS_k1, FLAGS_k2, FLAGS_k3;
        lens_distortion.set_radial_distortion(radial_k);
      }
    } else {
      Vec radial_k(4);
      radial_k << FLAGS_k1, FLAGS_k2, FLAGS_k3, FLAGS_k4;
      lens_distortion.set_radial_distortion(radial_k);
    }
  } else {
    Vec radial_k(5);
    radial_k << FLAGS_k1, FLAGS_k2, FLAGS_k3, FLAGS_k4, FLAGS_k5;
    lens_distortion.set_radial_distortion(radial_k);
  }
  VLOG(0) << "Radial coefficients: " 
          << lens_distortion.radial_distortion().transpose() << std::endl;
  
  if (FLAGS_p1 != 0 && FLAGS_p2 != 0) {
    Vec tangential_p(2);
    tangential_p(0) = FLAGS_p1;
    tangential_p(1) = FLAGS_p2;
    lens_distortion.set_tangential_distortion(tangential_p);
  }
  VLOG(0) << "Tangential coefficients: " 
          << lens_distortion.tangential_distortion().transpose() << std::endl;
  
  PinholeCameraDistortion camera(&lens_distortion);
  FloatImage *image = NULL;
  FloatImage *image_out = NULL;
  ImageCache cache;
  Vec2u size_image;
  Vec2 q;
  Mat qs_x(0, 0);
  Mat qs_y(0, 0);
  
  scoped_ptr<ImageSequence> source(ImageSequenceFromFiles(files, &cache));
  for (size_t i = 0; i < files.size(); ++i) {
    image = source->GetFloatImage(i);
    if (image) {
      if (qs_x.cols() == 0) {
        VLOG(0) << "Estimating undistortion map..." << std::endl;
        size_image << image->Width(), image->Height();
        image_out = new FloatImage(image->Height(),
                                   image->Width(),
                                   image->Depth());
        image_out->fill(0);
        if (!image_out)
          return 1;
        camera.set_image_size(size_image);
        Mat3 K;
        if (FLAGS_u0 == 0)
          FLAGS_u0 = size_image(0) / 2 - 0.5;
        if (FLAGS_v0 == 0)
          FLAGS_v0 = size_image(1) / 2 - 0.5;
        if (FLAGS_fy == 0)
          FLAGS_fy = FLAGS_fx;
        K << FLAGS_fx, FLAGS_sk, FLAGS_u0,
                    0, FLAGS_fy, FLAGS_v0,
                    0,        0,        1;
        camera.set_intrinsic_matrix(K);
        qs_x.resize(image->Height(), image->Width());
        qs_y.resize(image->Height(), image->Width());
        qs_x.fill(-1);
        qs_y.fill(-1);
        // TODO(julien) Put this map building in lens_distortion
        for (size_t x = 0; x < image->Width(); ++x) 
          for (size_t y = 0; y < image->Height(); ++y) {
            q << x, y;
            camera.ComputeUndistortedCoordinates(q, &q);
            if (image->Contains(q(1), q(0))) {
              qs_x(y, x) = q(0);
              qs_y(y, x) = q(1);
              for (int d = 0; d < image_out->Depth(); ++d) 
              (*image_out)(y, x, d) = SampleLinear(*image, q(1), q(0), d);
            }
          }
        VLOG(0) << "Estimating undistortion map...[DONE]." << std::endl;
        VLOG(1) << "qs_x : \n" << qs_x << std::endl;
        VLOG(1) << "qs_y : \n" << qs_y << std::endl;
      } else {
        // Tests if the size of the image is the same as the previous one
        if (size_image(0) != image->Width() || size_image(1) != image->Height())
          return 2;
        // TODO(julien) Put this image undistorting in camera?
        VLOG(0) << "Undistorting image " << i << "..." << std::endl;
        for (size_t x = 0; x < image->Width(); ++x) 
        for (size_t y = 0; y < image->Height(); ++y) {
          if (image->Contains(qs_y(y, x), qs_x(y, x))) {
          for (int d = 0; d < image_out->Depth(); ++d) 
            (*image_out)(y, x, d) = SampleLinear(*image,
                                                 qs_y(y, x), 
                                                 qs_x(y, x), d);
          }
        }
        VLOG(0) << "Undistorting image " << i << "...[DONE]." << std::endl;
      }
    }
    source->Unpin(i);
    // Write the output image
    VLOG(0) << "Saving undistorted image." << std::endl;
    std::stringstream s;
    s << ReplaceFolder(files[i].substr(0, files[i].rfind(".")), FLAGS_of);
    s << FLAGS_os;
    s << files[i].substr(files[i].rfind("."), files[i].size());
    WriteImage(*image_out, s.str().c_str());
  }
  if (image_out)
    delete image_out;
  return 0;
}
