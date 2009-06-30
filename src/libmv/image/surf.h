// Copyright (c) 2007, 2008, 2009 libmv authors.
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

#ifndef LIBMV_IMAGE_SURF_H
#define LIBMV_IMAGE_SURF_H

#include <cmath>
#include <vector>

#include "libmv/numeric/numeric.h"
#include "libmv/correspondence/feature.h"
#include "libmv/image/array_nd.h"
#include "libmv/image/blob_response.h"
#include "libmv/image/derivative.h"
#include "libmv/image/image_io.h"
#include "libmv/image/integral_image.h"
#include "libmv/image/non_maximal_suppression.h"
#include "libmv/image/surf_descriptor.h"
#include "libmv/logging/logging.h"

// TODO(keir): Move the detection routines into surf_detector.h.

namespace libmv {

struct SurfFeature : public PointFeature {
  libmv::Matrix<float, 64, 1> descriptor;
  // Match kdtree traits: with this, the SurfFeature can act as a kdtree point.
  float operator[](int i) const { return descriptor(i); }
};

template<typename TImage, typename TOctave>
void MakeSURFOctave(const TImage &integral_image, 
                    int num_intervals,
                    int lobe_start,
                    int lobe_increment,
                    int scale,
                    TOctave *octave) {
  assert(lobe_increment % 2 == 0);
  int rows = integral_image.rows();
  int cols = integral_image.cols();
  octave->Resize(num_intervals, rows / scale, cols / scale);
  octave->fill(0);
  for (int i = 0, lobe_size = lobe_start;
       i < num_intervals; ++i, lobe_size += lobe_increment) {
    VLOG(1) << "Filtering interval " << i
            << " with lobe size " << lobe_size;
    // Map a row-major eigen matrix into the array to avoid copying.
    // TODO(keir): Really, the right way to do this is to add some sort of
    // slicing semantics to the array class. Add slicing!
    Map<RMatf> blobiness(octave->Data() + octave->Offset(i, 0, 0),
                         rows / scale,
                         cols / scale);
    BlobResponse(integral_image, lobe_size, scale, &blobiness);
  }
}

// Do a single newton step toward the maximum.
template<typename TArray>
inline bool RefineMaxima3D(const TArray &f, int x, int y, int z, Vec3 *xp) {
  return Hessian3D(f, x, y, z).lu().solve(-Gradient3D(f, x, y, z), xp);
}

inline float GaussianScaleForInterval(float interval,
                               int lobe_start,
                               int lobe_increment) {
  // The magic number is from the paper; a gaussian filter with sigma = 1.2 is
  // roughly equivalent to the box filter approximation with kernel size 9x9
  // pixels.
  float lobe_size = lobe_start + interval*lobe_increment;
  float filter_width = 3*lobe_size;
  return filter_width * 1.2 / 9.0;
}

// Detect features. Each result colum stores x, y, s.
template<typename TImage, typename TPointFeature>
void DetectFeatures(const TImage integral_image,
                    int num_intervals,
                    int lobe_start,
                    int lobe_increment,
                    int scale,
                    std::vector<TPointFeature> *features) {

  Array3Df blob_responses;
  MakeSURFOctave(integral_image,
                 num_intervals,
                 lobe_start,
                 lobe_increment,
                 scale,
                 &blob_responses);

  std::vector<Vec3i> maxima;
  int parameter_maxima_region = 7;
  FindLocalMaxima3D(blob_responses, parameter_maxima_region, &maxima);

  // Refine the results.
  int rejected = 0;
  for (int i = 0; i < maxima.size(); ++i) {
    // Reject points on the boundary of scalespace.
    int x = maxima[i](0), y = maxima[i](1), z = maxima[i](2);
    if ( 0 == x || x == blob_responses.Shape(0)-1 ||
         0 == y || y == blob_responses.Shape(1)-1 ||
         0 == z || z == blob_responses.Shape(2)-1) {
      rejected++; continue;
    }
    // Reject anything that's not blobby enough.
    double parameter_maxima_threshold = 0.004;
    if (blob_responses(x, y, z) < parameter_maxima_threshold) {
      rejected++; continue;
    }
    // Reject saddle points.
    Vec3 delta;
    if (!RefineMaxima3D(blob_responses, x, y, z, &delta)) {
      rejected++; continue;
    }
    // Reject points which have refinements that move far away.
    double parameter_maxima_max_refinement_distance = 0.7;
    if (delta.norm() > parameter_maxima_max_refinement_distance) {
      rejected++; continue;
    }
    // 'src' is for (sigma, row, column).
    Vec3f src = maxima[i].cast<float>() + delta.cast<float>();
    TPointFeature detection;
    detection.coords << src(2), src(1);  // (x, y).
    detection.coords *= scale;
    detection.orientation = 0;
    detection.scale = GaussianScaleForInterval(src(0),
                                               lobe_start,
                                               lobe_increment);
    features->push_back(detection);
  }
  LOG(INFO) << "Found " << maxima.size() - rejected << " interest points.";
  LOG(INFO) << "Rejected " << rejected << " local maxima.";
}

// Detect features. Each result colum stores x, y, s.
template<typename TImage, typename TPointFeature>
void MultiscaleDetectFeatures(const TImage &integral_image,
                              int num_octaves,
                              int num_intervals,
                              std::vector<TPointFeature> *features) {
  int scale = 1;
  int lobe_start = 3;
  int lobe_increment = 2;
  for (int i = 0; i < num_octaves; ++i) {
    DetectFeatures(integral_image,
                   num_intervals,
                   lobe_start, lobe_increment, scale,
                   features);
    scale *= 2;
    lobe_start += lobe_increment;
    lobe_increment *= 2;
  }
  // TODO(keir): Right now, this can find nearly-identical features in scale
  // space if there are enough intervals and octaves. If there aren't many
  // features, doing an n^2 pruning based on distance in the 3D space of
  // (x,y,scale) is probably reasonable. Otherwise could do dual-tree kdtree
  // pruning, but there would have to be thousands of features for this to be
  // worthwhile.
  LOG(INFO) << "Multi scale fast hessian found " << features->size()
            << " features.";
}

// TODO(keir): Make the parameters for SURF extraction a class.
// TODO(keir): Add a unit test for this. Sadly it's not easy to do; perhaps
// detect a single blob in an image with faked gradients in the 4x4 bins?
template<typename TImage, typename TPointFeature>
void SurfFeatures(const TImage &image,
                  int num_octaves,
                  int num_intervals,
                  std::vector<TPointFeature> *detections) {
  Matf integral_image;
  IntegralImage(image, &integral_image);

  MultiscaleDetectFeatures(integral_image, num_octaves, num_intervals,
                           detections);
  for (int i = 0; i < detections->size(); ++i) {
    SurfFeature &f = (*detections)[i];
    USURFDescriptor<4, 5>(integral_image, f, &f.descriptor);
  }
}

}  // namespace libmv

#endif  // LIBMV_IMAGE_SURF_H
