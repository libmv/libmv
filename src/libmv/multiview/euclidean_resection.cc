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
#include "libmv/multiview/euclidean_resection.h"
#include "libmv/logging/logging.h"
#include <Eigen/Geometry>

namespace libmv {
namespace resection {


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

void EuclideanResection(const Mat2X &x_camera, const Mat3X &X_world,
                        Mat3 *R, Vec3 *t) {
  CHECK(x_camera.cols() == X_world.cols());

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
  int max_L_sq_value = abs(L_sq(IJ2LIndex(0, 0, num_lambda)));
  int max_L_sq_index = 1;
  for (int i = 1; i < num_lambda; ++i) {
    double abs_sq_value = abs(L_sq(IJ2LIndex(i, i, num_lambda)));
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
  // Recoverng the camera traslation and rotation.
  AbsoluteOrientation(X_world, X_cam, R, t);
}

} // namespace resection
} // namespace libmv
