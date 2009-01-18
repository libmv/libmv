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
#include "libmv/image/integral_image.h"
#include "libmv/image/non_maximal_suppression.h"
#include "third_party/glog/src/glog/logging.h"

namespace libmv {

template<typename TImage, typename TOctave>
void MakeSURFOctave(const TImage &integral_image, 
                    int intervals,
                    int lobe_start,
                    int lobe_increment,
                    int scale,
                    TOctave *octave) {
  assert(lobe_increment % 2 == 0);
  int rows = integral_image.rows();
  int cols = integral_image.cols();
  octave->Resize(intervals, rows / scale, cols / scale);
  for (int i = 0, lobe_size = lobe_start;
       i < intervals; ++i, lobe_size += lobe_increment) {
    VLOG(1) << "Filtering interval " << i
            << " with lobe size " << lobe_size;
    // Map a row-major eigen matrix into the array to avoid copying.
    // TODO(keir): Really, the right way to do this is to add some sort of
    // slicing semantics to the array class. Add slicing!
    Map<RMatf> blob_response(octave->Data() + octave->Offset(i, 0, 0),
                             rows, cols);
    BlobResponse(integral_image, lobe_size, scale, &blob_response);
  }
}

// Do a single newton step toward the maximum.
template<typename TArray>
inline Vec3 RefineMaxima3D(const TArray &f, int x, int y, int z) {
  Vec3 maxima;
  Hessian3D(f, x, y, z).lu().solve(-Gradient3D(f, x, y, z), &maxima);
  return maxima;
}

struct StoreResults {
 public:
  StoreResults(int max_x) : max_x(max_x), rejected(0) {}
  void operator() (int x, int y, int z, float value) {
    (void) value;
    // Only take maxima which are not at the top or bottom of scale space.
    if (x != 0 && x < max_x-1) {
      Vec3i position;
      //position << x << y << z;
      position(0) = x;
      position(1) = y;
      position(2) = z;
      points.push_back(position);
    } else {
      rejected++;
    }
  }
  int max_x;
  std::vector<Vec3i> points;
  int rejected;
};

// Detect features. Each result colum stores x, y, s.
template<typename TImage>
Mat3X DetectFeatures(const TImage image) {
  Matf integral_image;
  IntegralImage(image, &integral_image);

  // Only a single octave for now.
  int intervals = 4;
  Array3Df blob_responses;
  //MakeSURFOctave(integral_image, intervals, 3, 2, 1, &blob_responses);
  MakeSURFOctave(integral_image, intervals, 9, 8, 1, &blob_responses);

  StoreResults results(intervals);
  FindLocalMaxima3D(blob_responses, 5, &results);

  // Refine the results.
  int n = results.points.size();
  std::vector<Vec3f> intermediate_points;
  for (int i = 0; i < n; ++i) {
    Vec3 delta = RefineMaxima3D(blob_responses,
                                results.points[i](0),
                                results.points[i](1),
                                results.points[i](2));
    if (delta.norm() < 0.5) {
      // Only take points that are already close to the right spot.
      Vec3 updated = delta;
      updated(0) += double(results.points[i](0));
      updated(1) += double(results.points[i](1));
      updated(2) += double(results.points[i](2));
      //intermediate_points.push_back(results.points[i] + delta);
      intermediate_points.push_back(updated);
      // mult by scale.
    } else {
      results.rejected++;
    }
  }
  n = intermediate_points.size();
  Mat3X points(3, n);
  for (int i = 0; i < n; ++i) {
    points.col(i) = intermediate_points[i];
  }
  LOG(INFO) << "Found " << n << " interest points.";
  LOG(INFO) << "Rejected " << results.rejected << " local maxima.";
  return points;
}

}  // namespace libmv

#endif  // LIBMV_IMAGE_SURF_H
