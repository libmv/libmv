// Copyright (c) 2009 libmv authors.
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

#ifndef LIBMV_MULTIVIEW_EUCLIDEAN_RESECTION_KERNEL_H
#define LIBMV_MULTIVIEW_EUCLIDEAN_RESECTION_KERNEL_H

#include "libmv/base/vector.h"
#include "libmv/logging/logging.h"
#include "libmv/multiview/euclidean_resection.h"
#include "libmv/numeric/numeric.h"

namespace libmv {
namespace euclidean_resection {
namespace kernel {

class Kernel {
 public:
  typedef Mat34 Model;
  enum { MINIMUM_SAMPLES = 5 };
  // TODO(julien) avoid the copy of x_camera: create a new Kernel ?
  Kernel(const Mat2X &x_camera, const Mat3X &X) : x_camera_(x_camera), X_(X) {
    CHECK(x_camera.cols() == X.cols());
  }
  Kernel(const Mat2X &x_image, const Mat3X &X, const Mat3 &K) : X_(X) {
    CHECK(x_image.cols() == X.cols());
    // Conversion from image coordinates to normalized camera coordinates 
    EuclideanToNormalizedCamera(x_image, K, &x_camera_);
  }
  void Fit(const vector<int> &samples, vector<Model> *models) const {
    Mat2X x = ExtractColumns(x_camera_, samples);
    Mat3X X = ExtractColumns(X_, samples);
    Mat34 P;
    Mat3 K; K.setIdentity();
    Mat3 R;
    Vec3 t;
    EuclideanResection(x, X, &R, &t, RESECTION_EPNP);
    P_From_KRt(K, R, t, &P);
    models->push_back(P);
  }
  double Error(int sample, const Model &model) const {
    Mat3X X = X_.col(sample);
    Mat2X error = Project(model, X) - x_camera_.col(sample);
    return error.col(0).norm();
  }
  int NumSamples() const {
    return x_camera_.cols();
  }
 private:
  // x_camera_ contains the normalized camera coordinates 
        Mat2X  x_camera_;
  const Mat3X &X_;
};

}  // namespace kernel
}  // namespace resection
}  // namespace libmv

#endif  // LIBMV_MULTIVIEW_EUCLIDEAN_RESECTION_KERNEL_H
