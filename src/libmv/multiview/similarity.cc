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

#include "libmv/multiview/similarity.h"

namespace libmv {

// Parametrization
// s*cos -s*sin  tx
// s*sin  s*cos  ty
// 0      0      1

// It gives the following system A x = B :
// |-Y1  Y1 1 0 | | s*sin |   | X2 |
// | X1  Y1 0 1 | | s*cos |   | Y2 |
//                | tx    | =
//                | ty    |
// 
bool Similarity2DFromCorrespondencesLinear(const Mat &x1, const Mat &x2,
                                           Mat3 *M,
                                           double expected_precision) {
  assert(2 == x1.rows());
  assert(2 >= x1.cols());
  assert(x1.rows() == x2.rows());
  assert(x1.cols() == x2.cols());

  const int n = x1.cols();
  Mat A = Mat::Zero(2*n, 4);
  Mat b = Mat::Zero(2*n, 1);
  for (int i = 0; i < n; ++i) {
    const int j= i * 2;
    A(j,0) = -x1(1,i);
    A(j,1) =  x1(0,i);
    A(j,2) =  1.0;
    //A(j,3) =  0.0;

    A(j+1,0) = x1(0,i);
    A(j+1,1) = x1(1,i);
    //A(j+1,2) = 0.0;
    A(j+1,3) = 1.0;

    b(j,0)   = x2(0,i);
    b(j+1,0) = x2(1,i);
  }
  // Solve Ax=B
  Vec x = A.fullPivLu().solve(b);
  if ((A * x).isApprox(b, expected_precision))  {
    (*M)<<x(1),-x(0), x(2), // s*cos -s*sin tx
          x(0), x(1), x(3), // s*sin s*cos ty
          0.0,  0.0,  1.0;
    // Ensures that R is orthogonal (using SDV decomposition)
    Eigen::JacobiSVD<Mat> svd(M->block<2,2>(0, 0), Eigen::ComputeThinU | 
                                                   Eigen::ComputeThinV);
    double scale = std::sqrt(std::abs(M->block<2,2>(0, 0).determinant()));
    Mat2 sI2 = scale * Mat2::Identity();
    M->block<2,2>(0, 0) = svd.matrixU() * sI2 * svd.matrixV().transpose();  
    if (M->block<2,2>(0, 0).determinant() < 0)
      M->block<2,2>(0, 0) = -M->block<2,2>(0, 0);  
    
    // TODO(julien) Implement this paper:
    // Polar decomposition algorithm proposed by [Higham 86]
    // SIAM J. Sci. Stat. Comput. Vol. 7, Num. 4, October 1986.
    // "Computing the Polar Decomposition - with Applications"
    // by Nicholas Higham.
    
    return true;
  } else {
    return false;
  }
}

bool ExtractSimilarity2DCoefficients(const Mat3 &M,
                                     Vec2   *tr,
                                     double *angle,
                                     double *scale) {
  //assert(M(1, 0) == -M(0, 1));
  //assert(M(0, 0) == M(1, 1));
  assert(M(2, 2) == 1);
  *scale = std::sqrt(std::pow(M(0, 0),2) + std::pow(M(0, 1),2));
  *scale = (*scale + std::sqrt(std::pow(M(0, 0),2) + std::pow(M(0, 1),2))) / 2.0;
  if (*scale == 0)
    return false;
  
  *angle = std::asin(M(1, 0)/(*scale));
  if (*angle > 0) {
    if (M(0, 0) < 0)
      *angle += M_PI / 2.0;
  } else {
    if (M(0, 0) < 0)
      *angle -= M_PI / 2.0;
  }
  *tr << M(0, 2), M(1, 2);
  return true;
}
} // namespace libmv
