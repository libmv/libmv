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

#define V3DLIB_ENABLE_SUITESPARSE 1

#include <map>

#include "libmv/multiview/bundle.h"
#include "libmv/numeric/numeric.h"
#include "third_party/ssba/Math/v3d_linear.h"
#include "third_party/ssba/Math/v3d_linear_utils.h"
#include "third_party/ssba/Geometry/v3d_metricbundle.h"

namespace libmv {

static void
showErrorStatistics(double const f0,
                    V3D::StdDistortionFunction const& distortion,
                    std::vector<V3D::CameraMatrix> const& cams,
                    std::vector<V3D::Vector3d> const& Xs,
                    std::vector<V3D::Vector2d> const& measurements,
                    std::vector<int> const& correspondingView,
                    std::vector<int> const& correspondingPoint) {
  using namespace V3D;
  using namespace std;
  int const K = measurements.size();

  double meanReprojectionError = 0.0;
  for (int k = 0; k < K; ++k) {
    int const i = correspondingView[k];
    int const j = correspondingPoint[k];
    Vector2d p = cams[i].projectPoint(distortion, Xs[j]);

    double reprojectionError = norm_L2(f0 * (p - measurements[k]));
    meanReprojectionError += reprojectionError;
  }
  cout << "mean reprojection error (in pixels): " << meanReprojectionError/K << endl;
}



void EuclideanBAFull(const std::vector<Mat2X> &x,
                     std::vector<Mat3> *Ks,
                     std::vector<Mat3> *Rs,
                     std::vector<Vec3> *ts,
                     Mat3X *X) {
  using namespace V3D;
  using namespace std;

  int mode = V3D::FULL_BUNDLE_FOCAL_LENGTH;

  //////////////////////////////////////////////////
  // Prepare data structures.
  int num_camsN = Rs->size();
  int num_pointsM = X->cols();
  int num_obsK = num_camsN * num_pointsM;
  
  assert(x.size() == num_camsN);
  for (int i = 0; i < num_camsN; ++i) {
    assert(x[i].cols() == num_pointsM);
  }

  Matrix3x3d KMat;
  StdDistortionFunction distortion;

  makeIdentityMatrix(KMat);
  KMat[0][0] = (*Ks)[0](0,0);
  KMat[0][1] = (*Ks)[0](0,1);
  KMat[0][2] = (*Ks)[0](0,2);
  KMat[1][1] = (*Ks)[0](1,1);
  KMat[1][2] = (*Ks)[0](1,2);
  distortion.k1 = 0;
  distortion.k2 = 0;
  distortion.p1 = 0;
  distortion.p2 = 0;

  double const f0 = KMat[0][0];
  cout << "intrinsic before bundle = "; displayMatrix(KMat);
  Matrix3x3d Knorm = KMat;
  // Normalize the intrinsic to have unit focal length.
  scaleMatrixIP(1.0/f0, Knorm);
  Knorm[2][2] = 1.0;

  std::vector<Vector3d> Xs(num_pointsM);
  for (int j = 0; j < num_pointsM; ++j) {
    Xs[j][0] = (*X)(0, j);
    Xs[j][1] = (*X)(1, j);
    Xs[j][2] = (*X)(2, j);
  }

  std::vector<CameraMatrix> cams(num_camsN);
  for (int i = 0; i < num_camsN; ++i) {
    Matrix3x3d R;
    Vector3d T;

    R[0][0] = (*Rs)[i](0,0);  R[0][1] = (*Rs)[i](0,1);  R[0][2] = (*Rs)[i](0,2);
    R[1][0] = (*Rs)[i](1,0);  R[1][1] = (*Rs)[i](1,1);  R[1][2] = (*Rs)[i](1,2);
    R[2][0] = (*Rs)[i](2,0);  R[2][1] = (*Rs)[i](2,1);  R[2][2] = (*Rs)[i](2,2);
    T[0] = (*ts)[i](0);
    T[1] = (*ts)[i](1);
    T[2] = (*ts)[i](2);

    cams[i].setIntrinsic(Knorm);
    cams[i].setRotation(R);
    cams[i].setTranslation(T);
  }

  std::vector<Vector2d> measurements;
  std::vector<int> correspondingView;
  std::vector<int> correspondingPoint;

  measurements.reserve(num_obsK);
  correspondingView.reserve(num_obsK);
  correspondingPoint.reserve(num_obsK);

  for (int i = 0; i < num_camsN; ++i) {
    for (int j = 0; j < num_pointsM; ++j) {
      Vector2d p;
      p[0] = x[i](0,j);
      p[1] = x[i](1,j);

      // Normalize the measurements to match the unit focal length.
      scaleVectorIP(1.0/f0, p);
      measurements.push_back(p);
      correspondingView.push_back(i);
      correspondingPoint.push_back(j);
    }
  }

  assert(num_obsK == measurements.size());


  showErrorStatistics(f0, distortion, cams, Xs, measurements, correspondingView,
                      correspondingPoint);

  V3D::optimizerVerbosenessLevel = 1;
  double const inlierThreshold = 2.0 / f0;

  Matrix3x3d K0 = cams[0].getIntrinsic();
  cout << "K0 = "; displayMatrix(K0);

  CommonInternalsMetricBundleOptimizer opt(mode, inlierThreshold,
                                           K0, distortion,
                                           cams, Xs, measurements,
                                           correspondingView,
                                           correspondingPoint);
  opt.maxIterations = 50;
  opt.minimize();
  cout << "optimizer status = " << opt.status << endl;
  cout << "refined K = "; displayMatrix(K0);
  cout << "distortion = " << distortion.k1 << " " << distortion.k2 << " "
    << distortion.p1 << " " << distortion.p2 << endl;

  for (int i = 0; i < num_camsN; ++i) cams[i].setIntrinsic(K0);

  Matrix3x3d Knew = K0;
  scaleMatrixIP(f0, Knew);
  Knew[2][2] = 1.0;
  cout << "Knew = "; displayMatrix(Knew);

  showErrorStatistics(f0, distortion, cams, Xs, measurements, correspondingView,
                      correspondingPoint);

  //////////////////////////////////////////////////
  // Get the results.
  for (int i = 0; i < num_camsN; ++i) {
    (*Ks)[i](0,0) = Knew[0][0];
    (*Ks)[i](0,1) = Knew[0][1];
    (*Ks)[i](0,2) = Knew[0][2];
    (*Ks)[i](1,0) = Knew[1][0];
    (*Ks)[i](1,1) = Knew[1][1];
    (*Ks)[i](1,2) = Knew[1][2];
    (*Ks)[i](2,0) = Knew[2][0];
    (*Ks)[i](2,1) = Knew[2][1];
    (*Ks)[i](2,2) = Knew[2][2];
    Matrix3x4d const RT = cams[i].getOrientation();
    (*Rs)[i](0,0) = RT[0][0];
    (*Rs)[i](0,1) = RT[0][1];
    (*Rs)[i](0,2) = RT[0][2];
    (*Rs)[i](1,0) = RT[1][0];
    (*Rs)[i](1,1) = RT[1][1];
    (*Rs)[i](1,2) = RT[1][2];
    (*Rs)[i](2,0) = RT[2][0];
    (*Rs)[i](2,1) = RT[2][1];
    (*Rs)[i](2,2) = RT[2][2];
    (*ts)[i](0) = RT[0][3];
    (*ts)[i](1) = RT[1][3];
    (*ts)[i](2) = RT[2][3];
  }

  for (int j = 0; j < num_pointsM; ++j) {
    (*X)(0, j) = Xs[j][0];
    (*X)(1, j) = Xs[j][1];
    (*X)(2, j) = Xs[j][2];
  }
}


}  // namespace libmv

