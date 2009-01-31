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
#include "libmv/image/array_nd.h"
#include "libmv/image/blob_response.h"
#include "libmv/image/derivative.h"
#include "libmv/image/image_io.h"
#include "libmv/image/integral_image.h"
#include "libmv/image/non_maximal_suppression.h"
#include "third_party/glog/src/glog/logging.h"

namespace libmv {

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
  for (int i = 0, lobe_size = lobe_start;
       i < num_intervals; ++i, lobe_size += lobe_increment) {
    VLOG(1) << "Filtering interval " << i
            << " with lobe size " << lobe_size;
    // Map a row-major eigen matrix into the array to avoid copying.
    // TODO(keir): Really, the right way to do this is to add some sort of
    // slicing semantics to the array class. Add slicing!
    Map<RMatf> blobiness(octave->Data() + octave->Offset(i, 0, 0),
        rows / scale, cols / scale);
    BlobResponse(integral_image, lobe_size, scale, &blobiness);
  }
}

// Do a single newton step toward the maximum.
template<typename TArray>
inline Vec3 RefineMaxima3D(const TArray &f, int x, int y, int z) {
  Vec3 maxima;
  if (!Hessian3D(f, x, y, z).lu().solve(-Gradient3D(f, x, y, z), &maxima)) {
    maxima << 100, 100, 100; // Force abandoning ill-conditioned extremum.
  }
  return maxima;

}

float GaussianScaleForInterval(float interval,
                               int lobe_start,
                               int lobe_increment) {
  // XXX fixme; should this have a *9 or something?
  // The magic number is from the paper; a gaussian filter with sigma = 1.2 is
  // roughly equivalent to the box filter approximation with kernel size 9x9
  // pixels.
  return (lobe_start + interval*lobe_increment) * 1.2 / 9.0;
}

// Detect features. Each result colum stores x, y, s.
template<typename TImage>
void DetectFeatures(const TImage integral_image,
                    int num_intervals,
                    int lobe_start,
                    int lobe_increment,
                    int scale,
                    std::vector<Vec3f> *features) {

  Array3Df blob_responses;
  MakeSURFOctave(integral_image,
                 num_intervals,
                 lobe_start,
                 lobe_increment,
                 scale,
                 &blob_responses);

  std::vector<Vec3i> maxima;
  FindLocalMaxima3D(blob_responses, 3, &maxima);

  // Refine the results.
  int rejected = 0;
  for (int i = 0; i < maxima.size(); ++i) {
    // Reject points on the boundary of scalespace.
    int x = maxima[i](0), y = maxima[i](1), z = maxima[i](2);
    if ( 0 == x || x == blob_responses.Shape(0)-1 ||
         0 == y || y == blob_responses.Shape(1)-1 ||
         0 == z || z == blob_responses.Shape(2)-1) {
      rejected++;
      continue;
    }
    Vec3 delta = RefineMaxima3D(blob_responses, x, y, z);
    // Reject points that are not close to the right spot.
    if (delta.norm() > 0.5) {
      rejected++;
      continue;
    }
    Vec3f updated = delta + maxima[i].cast<float>();
    updated(0) = GaussianScaleForInterval(updated(0),
                                          lobe_start,
                                          lobe_increment);
    updated(1) *= scale;
    updated(2) *= scale;
    features->push_back(updated);
  }
  LOG(INFO) << "Found " << features->size() << " interest points.";
  LOG(INFO) << "Rejected " << rejected << " local maxima.";
}

// Detect features. Each result colum stores x, y, s.
template<typename TImage>
void MultiscaleDetectFeatures(const TImage image,
                              int num_octaves,
                              int num_intervals,
                              std::vector<Vec3f> *features) {
  Matf integral_image;
  IntegralImage(image, &integral_image);
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
}

}  // namespace libmv

#endif  // LIBMV_IMAGE_SURF_H
