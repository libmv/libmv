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
// 

#ifndef LIBMV_MULTIVIEW_AUTOCALIBRATION_H_
#define LIBMV_MULTIVIEW_AUTOCALIBRATION_H_

#include <vector>
#include <algorithm>

#include <Eigen/QR>

#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

void K_From_AbsoluteConic(const Mat3 &W, Mat3 *K);

// Compute a metric reconstruction from a projective one by computing the
// dual absolute quadric using linear constraints.  We follow the linear
// approach proposed by Pollefeys in section 3.4 of [1]
//
// [1] M. Pollefeys, L. Van Gool, M. Vergauwen, F. Verbiest, K. Cornelis,
//     J. Tops, R. Koch, "Visual modeling with a hand-held camera",
//     International Journal of Computer Vision 59(3), 207-232, 2004.
class AutoCalibrationLinear {
 public:
  int AddProjection(const Mat34 &P, double width, double height) {
    Mat34 P_normalized;
    NormalizeProjection(P, width, height, &P_normalized);

    AddProjectionConstraints(P_normalized);

    // Store input
    projections_.push_back(P_normalized);
    widths_.push_back(width);
    heights_.push_back(height);
    
    return projections_.size() - 1;
  }
  
  Mat4 MetricTransformation() {
    // Compute the dual absolute quadric, Q.
    Mat A(constraints_.size(), 10);
    for (int i = 0; i < A.rows(); ++i) {
      A.row(i) = Vec(constraints_[i]);
    }
    Vec q;
    Nullspace(&A, &q);
    Mat4 Q;
    AbsoluteQuadricMatFromVec(q, &Q);
    // TODO(pau) force rank 3.

    // Compute a transformation to a metric frame by decomposing Q.
    Eigen::SelfAdjointEigenSolver<Mat4> eigen_solver(Q);
    
    // Eigen values should be possitive,
    Vec temp_values = eigen_solver.eigenvalues();
    if (temp_values.sum() < 0) {
      temp_values = -temp_values;
    }
     
    // and sorted, so that they are proportional to 1,1,1,0.
    Vec eigenvalues;
    Mat eigenvectors;
    SortEigenVectors(temp_values, eigen_solver.eigenvectors(),
                     &eigenvalues, &eigenvectors);
    
    LOG(INFO) << "Q\n" << Q << "\n";
    LOG(INFO) << "eigen values\n" << eigenvalues << "\n";
    LOG(INFO) << "eigen vectors\n" << eigenvectors << "\n";

    return eigenvectors / eigenvectors(3, 3);
  }

 private:
  // Add constraints on the absolute quadric based on assumptions on the
  // camera parameters.  See eq. (26) of [1].
  void AddProjectionConstraints(const Mat34 &P) {
    double nu = 1;
    
    // Non-extreme focal lenght.
    constraints_.push_back((w11(P) - w33(P)) / 9 / nu);
    constraints_.push_back((w22(P) - w33(P)) / 9 / nu);

    // Aspect ratio is near 1.
    constraints_.push_back((w11(P) - w22(P)) / 0.2 / nu);

    // No skew and principal point near 0,0.
    // Note that there is a typo in the Pollefeys' paper: the 0.01 is not at the
    // correct equation.
    constraints_.push_back(w12(P) / 0.01 / nu);
    constraints_.push_back(w13(P) / 0.1 / nu);
    constraints_.push_back(w23(P) / 0.1 / nu);
  }

  // Here is the maxima code for computing the dual image of the absolute conic:
  // P : matrix([p00, p01, p02, p03],
  //            [p10, p11, p12, p13],
  //            [p20, p21, p22, p23]);
  // Q : matrix([q0, q1, q2, q3],
  //            [q1, q4, q5, q6],
  //            [q2, q5, q7, q8],
  //            [q3, q6, q8, q9]);
  // w : facsum(P.Q.transpose(P), q0,q1,q2,q3,q4,q5,q6,q7,q8,q9);
  //
  // From the output, some vim kunfu has generated the following.
  static Vec10 w11(const Mat34 &P) {
    Vec10 w11;
    w11 << P(0,0) * P(0,0),
           2 * P(0,0) * P(0,1),
           2 * P(0,0) * P(0,2),
           2 * P(0,0) * P(0,3),
           P(0,1) * P(0,1),
           2 * P(0,1) * P(0,2),
           2 * P(0,1) * P(0,3),
           P(0,2) * P(0,2),
           2 * P(0,2) * P(0,3),
           P(0,3) * P(0,3);
    return w11;
  }

  static Vec10 w12(const Mat34 &P) {
    Vec10 w12;
    w12 << P(0,0) * P(1,0),
           (P(0,0) * P(1,1) + P(0,1) * P(1,0)),
           (P(0,0) * P(1,2) + P(0,2) * P(1,0)),
           (P(0,0) * P(1,3) + P(0,3) * P(1,0)),
           P(0,1) * P(1,1),
           (P(0,1) * P(1,2) + P(0,2) * P(1,1)),
           (P(0,1) * P(1,3) + P(0,3) * P(1,1)),
           P(0,2) * P(1,2),
           (P(0,2) * P(1,3) + P(0,3) * P(1,2)),
           P(0,3) * P(1,3);
    return w12;
  }

  static Vec10 w13(const Mat34 &P) {
    Vec10 w13;
    w13 << P(0,0) * P(2,0),
           (P(0,0) * P(2,1) + P(0,1) * P(2,0)),
           (P(0,0) * P(2,2) + P(0,2) * P(2,0)),
           (P(0,0) * P(2,3) + P(0,3) * P(2,0)),
           P(0,1) * P(2,1),
           (P(0,1) * P(2,2) + P(0,2) * P(2,1)),
           (P(0,1) * P(2,3) + P(0,3) * P(2,1)),
           P(0,2) * P(2,2),
           (P(0,2) * P(2,3) + P(0,3) * P(2,2)),
           P(0,3) * P(2,3);
    return w13;
  }

  static Vec10 w22(const Mat34 &P) {
    Vec10 w22;
    w22 << P(1,0) * P(1,0),
           2 * P(1,0) * P(1,1),
           2 * P(1,0) * P(1,2),
           2 * P(1,0) * P(1,3),
           P(1,1) * P(1,1),
           2 * P(1,1) * P(1,2),
           2 * P(1,1) * P(1,3),
           P(1,2) * P(1,2),
           2 * P(1,2) * P(1,3),
           P(1,3) * P(1,3);
    return w22;
  }

  static Vec10 w23(const Mat34 &P) {
    Vec10 w23;
    w23 << P(1,0) * P(2,0),
           (P(1,0) * P(2,1) + P(1,1) * P(2,0)),
           (P(1,0) * P(2,2) + P(1,2) * P(2,0)),
           (P(1,0) * P(2,3) + P(1,3) * P(2,0)),
           P(1,1) * P(2,1),
           (P(1,1) * P(2,2) + P(1,2) * P(2,1)),
           (P(1,1) * P(2,3) + P(1,3) * P(2,1)),
           P(1,2) * P(2,2),
           (P(1,2) * P(2,3) + P(1,3) * P(2,2)),
           P(1,3) * P(2,3);
    return w23;
  }

  static Vec10 w33(const Mat34 &P) {
    Vec10 w33;
    w33 << P(2,0) * P(2,0),
           2 * P(2,0) * P(2,1),
           2 * P(2,0) * P(2,2),
           2 * P(2,0) * P(2,3),
           P(2,1) * P(2,1),
           2 * P(2,1) * P(2,2),
           2 * P(2,1) * P(2,3),
           P(2,2) * P(2,2),
           2 * P(2,2) * P(2,3),
           P(2,3) * P(2,3);
    return w33;
  }

  static void AbsoluteQuadricMatFromVec(const Vec &q, Mat4 *Q) {
    *Q << q(0), q(1), q(2), q(3),
          q(1), q(4), q(5), q(6),
          q(2), q(5), q(7), q(8),
          q(3), q(6), q(8), q(9);
  }

  static void NormalizeProjection(const Mat34 &P,
                                  double width,
                                  double height,
                                  Mat34 *P_new) {
    Mat3 T;
    T << width + height,              0,  (width - 1) / 2,
                      0, width + height, (height - 1) / 2,
                      0,              0,                1;
    *P_new = T.inverse() * P;
  }
  
  static void DenormalizeProjection(const Mat34 &P,
                                    double width,
                                    double height,
                                    Mat34 *P_new) {
    Mat3 T;
    T << width + height,              0,  (width - 1) / 2,
                      0, width + height, (height - 1) / 2,
                      0,              0,                1;
    *P_new = T * P;
  }

  // TODO(pau): make this generic and move it to numeric.h
  static void SortEigenVectors(const Vec &values, const Mat &vectors,
                               Vec *sorted_values, Mat *sorted_vectors) {
    // Compute eigenvalues order.
    std::pair<double, int> order[4];
    for (int i = 0; i < 4; ++i) {
      order[i].first = -values(i);
      order[i].second = i;
    }
    std::sort(order, order + 4);

    sorted_values->resize(4);
    sorted_vectors->resize(4,4);
    for (int i = 0; i < 4; ++i) {
      (*sorted_values)(i) = values[order[i].second];
      sorted_vectors->col(i) = vectors.col(order[i].second);
    }
  }
 
 private:
  std::vector<Mat34> projections_; // The *normalized* projection matrices.
  std::vector<double> widths_;
  std::vector<double> heights_;
  std::vector<Vec10> constraints_;  // Linear constraints on q.
};

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_AUTOCALIBRATION_H_
