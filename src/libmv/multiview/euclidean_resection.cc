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

#include <cmath>
#include <Eigen/Geometry>

#include "libmv/logging/logging.h"
#include "libmv/base/vector.h"
#include "libmv/multiview/euclidean_resection.h"
#include "libmv/multiview/projection.h"


namespace libmv {
namespace resection {

void EuclideanResection(const Mat2X &x_camera, 
                        const Mat3X &X_world,
                        Mat3 *R, Vec3 *t,
                        eLibmvResectionMethod eResectionMethod) {
  switch (eResectionMethod)
  {
  case eRESECTION_ANSARDANIILIDIS:
    {
      EuclideanResectionAnsarDaniilidis(x_camera, X_world, R, t);
    }
    break;
  case eRESECTION_EPNP:
    {
      EuclideanResectionEPnP(x_camera, X_world, R, t);      
    }
    break;
  default:
    {
      LOG(FATAL) << "Unknown resection method." << std::endl; 
    }
  }
}

void EuclideanResection(const Mat &x_image, 
                        const Mat3X &X_world,
                        const Mat3 &K, Mat3 *R, Vec3 *t,
                        eLibmvResectionMethod eResectionMethod)
{
  CHECK(x_image.rows() == 2 || x_image.rows() == 3);
  Mat x_camera;
  Mat3X x_camera_h;
  if (x_image.rows() == 2) {
    Mat x_image_h;
    EuclideanToHomogeneous(x_image, &x_image_h);
    x_camera_h = K.inverse() * x_image_h;
  } else if (x_image.rows() == 3) {
    x_camera_h = K.inverse() * x_image;
  }
  HomogeneousToEuclidean(x_camera_h, &x_camera);
  EuclideanResection(x_camera, X_world, R, t, eResectionMethod);
}

void AbsoluteOrientation(const Mat3X &X,
                         const Mat3X &Xp,
                         Mat3 *R,
                         Vec3 *t) {
  int nPoints = X.cols();
  Vec3 C  = X.rowwise().sum() / nPoints; // Centroid of X.
  Vec3 Cp = Xp.rowwise().sum() / nPoints; // Centroid of Xp.

  // Normalize the two point sets.
  Mat3X Xn(3,nPoints), Xpn(3, nPoints);
  for( int i = 0; i < nPoints; ++i ){
    Xn.col(i)  = X.col(i) - C;
    Xpn.col(i) = Xp.col(i) - Cp;
  }
  
  // Constrcuting the N matrix (pg. 635)
  double Sxx = Xn.row(0).dot(Xpn.row(0));
  double Syy = Xn.row(1).dot(Xpn.row(1));
  double Szz = Xn.row(2).dot(Xpn.row(2));
  double Sxy = Xn.row(0).dot(Xpn.row(1));
  double Syx = Xn.row(1).dot(Xpn.row(0));
  double Sxz = Xn.row(0).dot(Xpn.row(2));
  double Szx = Xn.row(2).dot(Xpn.row(0));
  double Syz = Xn.row(1).dot(Xpn.row(2));
  double Szy = Xn.row(2).dot(Xpn.row(1));

  Mat4 N;
  N << Sxx + Syy + Szz, Syz - Szy,       Szx - Sxz,        Sxy - Syx,
       Syz - Szy,       Sxx - Syy - Szz, Sxy + Syx,        Szx + Sxz,
       Szx - Sxz,       Sxy + Syx,       -Sxx + Syy - Szz, Syz + Szy,
       Sxy - Syx,       Szx + Sxz,       Syz + Szy,        -Sxx - Syy + Szz;
           
  // Find the nit quaternion q that maximizes qNq. It is the eigenvector
  // corresponding to the lagest eigenvalue.
  Vec4 q = N.svd().matrixU().col(0);
  // Retrieve the 3x3 rotation matrix.
  Vec4 qq = q.cwise() * q;
  double q0q1 = q(0) * q(1);
  double q0q2 = q(0) * q(2);
  double q0q3 = q(0) * q(3);
  double q1q2 = q(1) * q(2);
  double q1q3 = q(1) * q(3);
  double q2q3 = q(2) * q(3);

  (*R) << qq(0) + qq(1) - qq(2) - qq(3),
          2 * (q1q2 - q0q3),
          2 * (q1q3 + q0q2),
          2 * (q1q2+ q0q3),
          qq(0) - qq(1) + qq(2) - qq(3),
          2 * (q2q3 - q0q1),
          2 * (q1q3 - q0q2),
          2 * (q2q3 + q0q1),
          qq(0) - qq(1) - qq(2) + qq(3);
  // Check the sign of the R matrix.
  if (R->determinant() < 0)
      R->row(2) = -R->row(2);
  // Compute the final translation.
  *t = Cp - *R * C;
}

// Convert i and j indexes of the original variables into their quadratic
// permutation single index. It follows the rules that
// t_ij = t_ji.
inline int IJ2PointIdex(int i, int j, int num_points) {
  // Always make sure that j is bigger than i. This handles t_ij = t_ji.
  if (j < i) {
    std::swap(i, j);
  }
  int idx;
  int num_permutation_rows = num_points * (num_points - 1) / 2;
  // All t_ii's are located at the end of the t variables vector after
  // all t_ij's.
  if (j == i) {
    idx = num_permutation_rows + i;
  } else {
    int offset = (num_points - i - 1) * (num_points - i) / 2;
    idx = (num_permutation_rows - offset + j - i - 1);
  }
  return idx;
};

// Convert i and j indexes of the solution for lambda to their linear indexes.
inline int IJ2LIndex(int i, int j, int num_lambda) {
  if (j < i) {
    std::swap(i, j);
  }
  int A = num_lambda * (num_lambda + 1) / 2;
  int B = num_lambda - i;
  int C = B * (B + 1) / 2;
  int idx = A - C + j - i;
  return idx;
};

inline int Sign(double value) {
  return (value < 0) ? -1 : 1;
};

// Organizes a square matrix into a single row constraint on the elements of
// Lambda to create the constraints in equation (5)
// in "Linear Pose Estimation from Points or Lines", by Ansar, A.
// and Daniilidis, PAMI 2003. vol. 25, no. 5.
inline Vec Matrix2Constraint(const Mat &A,
                             int num_k_columns,
                             int num_lambda) {
  Vec C(num_k_columns);
  C.setZero();
  int idx = 0;
  for (int i = 0; i < num_lambda; ++i) {
    for( int j = i; j < num_lambda; ++j) {
      C(idx) = A(i, j);
      if (i != j){
        C(idx) += A(j, i);
      }
      ++ idx;
    }
  }
  return C;
}

// Normalizes the vectors contained in the columns of the Vecs matrix.
inline void NormalizeColumnVectors(Mat3X *vectors) {
  int noVect = vectors->cols();
  for (int i = 0; i < noVect; ++i){
    vectors->col(i).normalize();
  }
}

void EuclideanResectionAnsarDaniilidis(const Mat2X &x_camera, 
                                       const Mat3X &X_world,               
                                       Mat3 *R, 
                                       Vec3 *t) {
  CHECK(x_camera.cols() == X_world.cols());
  CHECK(x_camera.cols() > 3);

  int num_points = x_camera.cols();
  // Copy the normalized camera coords into 3 vectors and normalize them so
  // that they are unit vectors from the camera center.
  Mat3X x_camera_unit(3, num_points);
  x_camera_unit.block(0, 0, 2, num_points) = x_camera;
  x_camera_unit.row(2).setOnes();
  NormalizeColumnVectors(&x_camera_unit);
  
  int num_m_rows = num_points * (num_points - 1) / 2;
  int num_tt_variables = num_points * (num_points + 1) / 2;
  int num_m_columns = num_tt_variables + 1;
  Mat M(num_m_columns, num_m_columns);
  M.setZero();
  Matu ij_index(num_tt_variables, 2);

  // Create the constrain equations for the t_ij variables (7) and arrange them
  // into the M matrix (8). Also store the initial i.j indexes.
  int row=0;
  for (int i = 0; i < num_points; ++i) {
    for (int j = i+1; j < num_points; ++j) {
      M(row, row) = -2 * x_camera_unit.col(i).dot(x_camera_unit.col(j));
      M(row, num_m_rows + i) = x_camera_unit.col(i).dot(x_camera_unit.col(i));
      M(row, num_m_rows + j) = x_camera_unit.col(j).dot(x_camera_unit.col(j));
      Vec3 Xdiff = X_world.col(i) - X_world.col(j);
      double center2point_dostance = Xdiff.norm();
      M(row, num_m_columns - 1) =
          - center2point_dostance * center2point_dostance;
      ij_index(row, 0) = i;
      ij_index(row, 1) = j;
      ++row;
    }
    ij_index(i + num_m_rows, 0) = i;
    ij_index(i + num_m_rows, 1) = i;
  }

  int num_lambda = num_points + 1; // Dimension of the null space of M.
  Mat V = M.svd().matrixV().block(0, num_m_rows, num_m_columns, num_lambda);

  // TODO(Vess): The number of constrain equations in K (nKRows) must be
  // (nPoints + 1) * (nPoints + 2)/2. This creates a performance issue for
  // more than 4 points. It is fine for 4 points at the moment with 18
  // instead of 15 equations.
  int num_k_rows = num_m_rows + num_points *
                   (num_points*(num_points-1)/2 - num_points+1);
  int num_k_columns = num_lambda * (num_lambda + 1) / 2;
  Mat K(num_k_rows, num_k_columns);
  K.setZero();

  int counter_k_row = 0;
  // Constructing the first part of the K matrix corresponding to (t_ii, t_jk)
  // for i != j.
  for (int idx1 = num_m_rows; idx1 < num_tt_variables; ++idx1) {
    for (int idx2 = 0; idx2 < num_m_rows; ++idx2) {

      unsigned int i = ij_index(idx1, 0);
      unsigned int j = ij_index(idx2, 0);
      unsigned int k = ij_index(idx2, 1);

      if( i != j && i != k ){
        int idx3 = IJ2PointIdex(i, j, num_points);
        int idx4 = IJ2PointIdex(i, k, num_points);

        K.row(counter_k_row) =
            Matrix2Constraint(V.row(idx1).transpose() * V.row(idx2)-
                              V.row(idx3).transpose() * V.row(idx4),
                              num_k_columns,
                              num_lambda);
        ++counter_k_row;
      }
    }
  }

  // Constructing the second part of the K matrix corresponding to (t_ii,t_jk)
  // for j==k.
  for (int idx1 = num_m_rows; idx1 < num_tt_variables; ++idx1) {
    for (int idx2 = idx1 + 1; idx2 < num_tt_variables; ++idx2) {
      unsigned int i = ij_index(idx1, 0);
      unsigned int j = ij_index(idx2, 0);
      unsigned int k = ij_index(idx2, 1);

      int idx3 = IJ2PointIdex(i, j, num_points);
      int idx4 = IJ2PointIdex(i, k, num_points);

      K.row(counter_k_row) =
                       Matrix2Constraint(V.row(idx1).transpose() * V.row(idx2)-
                                         V.row(idx3).transpose() * V.row(idx4),
                                         num_k_columns,
                                         num_lambda);
      ++counter_k_row;
    }
  }
  Vec L_sq = K.svd().matrixV().col(num_k_columns - 1);

  // Pivot on the largest element, for numerical stability. Afterwards
  // recover the sign of the lambda solution.
  double max_L_sq_value = fabs(L_sq(IJ2LIndex(0, 0, num_lambda)));
  int max_L_sq_index = 1;
  for (int i = 1; i < num_lambda; ++i) {
    double abs_sq_value = fabs(L_sq(IJ2LIndex(i, i, num_lambda)));
    if (max_L_sq_value < abs_sq_value) {
      max_L_sq_value = abs_sq_value;
      max_L_sq_index = i;
    }
  }
  // Ensure positiveness of the largest value corresponding to lambda_ii.
  L_sq = L_sq * Sign(L_sq(IJ2LIndex(max_L_sq_index,
                                    max_L_sq_index,
                                    num_lambda)));
  
  
  Vec L(num_lambda);
  L(max_L_sq_index) = sqrt(L_sq(IJ2LIndex(max_L_sq_index,
                                          max_L_sq_index,
                                          num_lambda)));
  
  for (int i = 0; i < num_lambda; ++i) {
    if (i != max_L_sq_index) {
      L(i) = L_sq(IJ2LIndex(max_L_sq_index, i, num_lambda)) / L(max_L_sq_index);
    }
  }

  // Correct the scale using the fact that the last constraint is equal to 1.
  L = L / (V.row(num_m_columns - 1).dot(L));
  Vec X = V * L;
  
  // Recovering the distances from the camera center to the 3D points Q.
  Vec d(num_points);
  d.setZero();
  for (int c_point = num_m_rows; c_point < num_tt_variables; ++c_point) {
    d(c_point-num_m_rows) = sqrt(X(c_point));
  }

  // Creating the 3D points in the camera system.
  Mat X_cam(3, num_points);
  for (int c_point = 0; c_point < num_points; ++c_point ) {
    X_cam.col(c_point) = d(c_point) * x_camera_unit.col(c_point);
  }
  // Recovering the camera traslation and rotation.
  AbsoluteOrientation(X_world, X_cam, R, t);
}

// TODO(julien): make this generic andmove it to projection.h ?
// Estimates the root mean square error (2D)
double RMSE(const Mat2X &x_camera, 
            const Mat3X &X_world,               
            const Mat34 &P) {
  size_t num_points = x_camera.cols();
  Mat2X dx = Project(P, X_world) - x_camera;
  return dx.norm() / num_points;
}

// TODO(julien): make this generic andmove it to projection.h ?
// Estimates the root mean square error (2D)
double RMSE(const Mat2X &x_camera, 
            const Mat3X &X_world,               
            const Mat3 &K, 
            const Mat3 &R, 
            const Vec3 &t) {
  Mat34 P;
  P_From_KRt(K, R, t, &P);
  size_t num_points = x_camera.cols();
  Mat2X dx = Project(P, X_world) - x_camera;
  return dx.norm() / num_points;
}

// Selects 4 (virtuals) control points (mean and PCA)
void SelectControlPoints(const Mat3X &X_world, 
                         Mat *X_centered, 
                         Mat34 *X_control_points) {
  size_t num_points = X_world.cols();
  // The first one (C0) is the centroid
  Vec mean, variance;
  MeanAndVarianceAlongRows(X_world, &mean, &variance);
  X_control_points->col(0) = mean;
  // Computes PCA
  *X_centered = X_world;
  for (size_t c = 0; c < num_points; c++)
    X_centered->col(c) -= mean;
  Mat3 X_centered_sq = (*X_centered) * X_centered->transpose();
  Eigen::SVD<Mat3> X_centered_sq_svd = X_centered_sq.svd();
  Vec3 w = X_centered_sq_svd.singularValues();
  Mat3 u = X_centered_sq_svd.matrixU();
  for (size_t c = 0; c < 3; c++) {
    double k = sqrt(w(c) / num_points);    
    X_control_points->col(c+1) = mean + k * u.col(c);
  }
}

// Computes the barycentric coordinates for all real points
void ComputeBarycentricCoordinates(const Mat3X &X_world_centered, 
                                   const Mat34 &X_control_points,
                                   Mat4X *alphas) {
  size_t num_points = X_world_centered.cols();
  Mat3 C2 ;
  for (size_t c = 1; c < 4; c++)
    C2.col(c-1) = X_control_points.col(c) - X_control_points.col(0);
  Mat3 C2inv = C2.inverse();
  alphas->resize(4, num_points);
  alphas->setZero();
  Mat3X a = C2inv * X_world_centered;
  alphas->block(1, 0, 3, num_points) = a;
  for (size_t c = 0; c < num_points; c++) {
    (*alphas)(0, c) = 1.0 - alphas->col(c).sum();
  }
}

// Estimates the coordinates of all real points in the camera coordinate frame
void ComputePointsCoordinatesInCameraFrame(const Mat4X &alphas, 
    const Vec4 &betas,
    const Eigen::Matrix<double, 12, 12> &U,
    Mat3X *X_camera) {
  size_t num_points = alphas.cols();
  // Estimates the control points in the camera reference frame
  Mat34 C2b; C2b.setZero();
  for (size_t cu = 0; cu < 4; cu++) {
    for (size_t c = 0; c < 4; c++) {
      C2b.col(c) += betas(cu) * U.block(11 - cu, c * 3, 1, 3).transpose();
    }
  }
  // Estimates the 3D points in the camera reference frame
  X_camera->resize(3, num_points);
  for (size_t c = 0; c < num_points; c++) {
    X_camera->col(c) << C2b * alphas.col(c);
  }
  // Check the sign of the z coordinate of the first point
  if ((*X_camera)(2,0) < 0) {
    C2b = -C2b;
    *X_camera = -(*X_camera);
  }    
}

void EuclideanResectionEPnP(const Mat2X &x_camera, const Mat3X &X_world, 
                            Mat3 *R, Vec3 *t) {
  CHECK(x_camera.cols() == X_world.cols());
  CHECK(x_camera.cols() > 3);
  size_t num_points = X_world.cols();
 
  // Selects control points
  Mat34 X_control_points;
  Mat X_centered;
  SelectControlPoints(X_world, &X_centered, &X_control_points);

  // Computes the barycentric coordinates
  Mat4X alphas(4, num_points);
  ComputeBarycentricCoordinates(X_centered, X_control_points, &alphas);
  
  // Estimates the M matrix with the barycentric coordinates
  double a0, a1, a2, a3, ui, vi;
  Mat M(2*num_points, 12);
  Eigen::Matrix<double, 2, 12> sub_M;
  for (size_t c = 0; c < num_points; c++) {
    a0 = alphas(0, c);
    a1 = alphas(1, c);
    a2 = alphas(2, c);
    a3 = alphas(3, c);
    ui = x_camera(0, c);
    vi = x_camera(1, c);
    M.block(2*c, 0, 2, 12) << a0, 0, 
                  a0*(-ui), a1, 0,
                  a1*(-ui), a2, 0, 
                  a2*(-ui), a3, 0,
                  a3*(-ui), 0, 
                  a0, a0*(-vi), 0,
                  a1, a1*(-vi), 0,
                  a2, a2*(-vi), 0,
                  a3, a3*(-vi);
  }
  
  Eigen::SVD<Mat> MtMsvd = (M.transpose()*M).svd();
  // TODO(julien) avoid to transpose: rewrite the u2.block() calls
  Eigen::Matrix<double, 12, 12> u2 = MtMsvd.matrixU().transpose();
  // Estimates the L matrix
  Eigen::Matrix<double, 6, 3> dv1;
  Eigen::Matrix<double, 6, 3> dv2;
  Eigen::Matrix<double, 6, 3> dv3;
  Eigen::Matrix<double, 6, 3> dv4;
  size_t id_row = 11;
  dv1.row(0) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 3, 1, 3);
  dv1.row(1) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv1.row(2) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv1.row(3) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv1.row(4) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv1.row(5) = u2.block(id_row, 6, 1, 3) - u2.block(id_row, 9, 1, 3);
  id_row = 10;
  dv2.row(0) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 3, 1, 3);
  dv2.row(1) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv2.row(2) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv2.row(3) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv2.row(4) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv2.row(5) = u2.block(id_row, 6, 1, 3) - u2.block(id_row, 9, 1, 3);
  id_row = 9;
  dv3.row(0) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 3, 1, 3);
  dv3.row(1) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv3.row(2) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv3.row(3) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv3.row(4) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv3.row(5) = u2.block(id_row, 6, 1, 3) - u2.block(id_row, 9, 1, 3);
  id_row = 8;
  dv4.row(0) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 3, 1, 3);
  dv4.row(1) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv4.row(2) = u2.block(id_row, 0, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv4.row(3) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 6, 1, 3);
  dv4.row(4) = u2.block(id_row, 3, 1, 3) - u2.block(id_row, 9, 1, 3);
  dv4.row(5) = u2.block(id_row, 6, 1, 3) - u2.block(id_row, 9, 1, 3);
  Eigen::Matrix<double, 6, 10> L;
  for (size_t r = 0; r < 6; r++) {
    L.row(r) << dv1.row(r).dot(dv1.row(r)),
          2.0 * dv1.row(r).dot(dv2.row(r)),
                dv2.row(r).dot(dv2.row(r)),
          2.0 * dv1.row(r).dot(dv3.row(r)),
          2.0 * dv2.row(r).dot(dv3.row(r)),
                dv3.row(r).dot(dv3.row(r)),
          2.0 * dv1.row(r).dot(dv4.row(r)),
          2.0 * dv2.row(r).dot(dv4.row(r)),
          2.0 * dv3.row(r).dot(dv4.row(r)),
                dv4.row(r).dot(dv4.row(r));
  }  
  Vec6 rho;
  rho << (X_control_points.col(0) - X_control_points.col(1)).squaredNorm(),
         (X_control_points.col(0) - X_control_points.col(2)).squaredNorm(),
         (X_control_points.col(0) - X_control_points.col(3)).squaredNorm(),
         (X_control_points.col(1) - X_control_points.col(2)).squaredNorm(),
         (X_control_points.col(1) - X_control_points.col(3)).squaredNorm(),
         (X_control_points.col(2) - X_control_points.col(3)).squaredNorm();
 
  // Estimates 3 solutions based on 3 approximations of L (betas)
  Mat3X X_camera;
  Mat3 K; K.setIdentity();
  vector<Mat3> Rs(3);
  vector<Vec3> ts(3);
  Vec rmse(3);
  // Estimates the betas, first approximation
  // Betas          = [b00 b01 b11 b02 b12 b22 b03 b13 b23 b33]
  // Betas_approx_1 = [b00 b01     b02         b03]
  Vec4 betas; betas.setZero();
  Eigen::Matrix<double, 6, 4> l_6x4;
  Vec4 b4;
  for (size_t r = 0; r < 6; r++) {
    l_6x4.row(r) << L(r, 0), L(r, 1), L(r, 3), L(r, 6); 
  }
  Eigen::SVD<Mat> svdOfL4(l_6x4);
  if (svdOfL4.solve(rho, &b4)) {
    if (b4(0) < 0) {
      b4 = -b4;
    } 
    b4(0) =  std::sqrt(b4(0));
    betas <<  b4(0), b4(1)/b4(0), b4(2)/b4(0), b4(3)/b4(0);
    ComputePointsCoordinatesInCameraFrame(alphas, betas, u2, &X_camera);
    AbsoluteOrientation(X_world, X_camera, &Rs[0], &ts[0]);
    rmse(0) = RMSE(x_camera, X_world, K, Rs[0], ts[0]);
  } else {
   LOG(INFO) << " Beta first approximation not good enough." << std::endl;
   ts[0].setZero();
   rmse(0) = 1e10;
  }
 
  // Estimates the betas, second approximation
  // Betas          = [b00 b01 b11 b02 b12 b22 b03 b13 b23 b33]
  // Betas_approx_2 = [b00 b01 b11]
  betas.setZero();
  Eigen::Matrix<double, 6, 3> l_6x3;
  Vec3 b3;
  l_6x3 = L.block(0, 0, 6, 3);
  Eigen::SVD<Mat> svdOfL3(l_6x3);
  if (svdOfL3.solve(rho, &b3)) {
    if (b3(0) < 0) {
      betas(0) = std::sqrt(-b3(0));
      if (b3(2) < 0)
        betas(1) = std::sqrt(-b3(2));
    } else {
      betas(0) = std::sqrt(b3(0));
      if (b3(2) > 0)
        betas(1) = std::sqrt(b3(2));
    }
    if (b3(1) < 0)
      betas(0) = -betas(0);
    ComputePointsCoordinatesInCameraFrame(alphas, betas, u2, &X_camera);
    AbsoluteOrientation(X_world, X_camera, &Rs[1], &ts[1]);
    rmse(1) = RMSE(x_camera, X_world, K, Rs[1], ts[1]);
  } else {
   LOG(INFO) << " Beta second approximation not good enough." << std::endl;
   ts[1].setZero();
   rmse(1) = 1e10;
  }
  
  // Estimates the betas, third approximation
  // Betas          = [b00 b01 b11 b02 b12 b22 b03 b13 b23 b33]
  // Betas_approx_3 = [b00 b01 b11 b02 b12]
  betas.setZero();
  Eigen::Matrix<double, 6, 5> l_6x5;
  Vec5 b5;
  l_6x5 = L.block(0, 0, 6, 5);
  Eigen::SVD<Mat> svdOfL5(l_6x5);
  if (svdOfL5.solve(rho, &b5)) {
    if (b5(0) < 0) {
      betas(0) = std::sqrt(-b5(0));
      if (b5(2) < 0)
        betas(1) = std::sqrt(-b5(2));
    } else {
      betas(0) = std::sqrt(b5(0));
      if (b5(2) > 0)
        betas(1) = std::sqrt(b5(2));
    }
    if (b5(1) < 0)
      betas(0) = -betas(0);
    betas(2) = b5(3) / betas(0);
    ComputePointsCoordinatesInCameraFrame(alphas, betas, u2, &X_camera);
    AbsoluteOrientation(X_world, X_camera, &Rs[2], &ts[2]);
    rmse(2) = RMSE(x_camera, X_world, K, Rs[2], ts[2]);
  } else {
   LOG(INFO) << " Beta third approximation not good enough." << std::endl;
   ts[2].setZero();
   rmse(2) = 1e10;
  }
  
  // TODO(julien) do non-linear refinement (Gauss-Newton) for the 3 solutions
  // maybe in a separate function.
  
  // Selects the solution (R,t) which has the RMSE minimum
  size_t n = 0;
  if (rmse(1) < rmse(0)) n = 1;
  if (rmse(2) < rmse(n)) n = 2;
  *R = Rs[n];
  *t = ts[n];
}
} // namespace resection
} // namespace libmv
