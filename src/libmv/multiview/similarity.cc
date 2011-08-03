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

#include <Eigen/Geometry> 

#include "libmv/multiview/affine.h"
#include "libmv/multiview/similarity.h"
#include "libmv/multiview/similarity_parameterization.h"

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
  assert(2 <= x1.cols());
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
    Similarity2DSCParameterization<double>::To(x, M);    
    return true;
  } else {
    return false;
  }
}

bool Similarity3DFromCorrespondencesLinear(const Mat &x1,
                                          const Mat &x2,
                                          Mat4 *H,
                                          double expected_precision) {
   // TODO(julien) Compare to *H = umeyama (x1, x2, true);
   // and keep the best one (quality&speed)   
  if (Affine3DFromCorrespondencesLinear(x1, x2, H, expected_precision)) {
    // Ensures that R is orthogonal (using SDV decomposition)
    Eigen::JacobiSVD<Mat> svd(H->block<3,3>(0, 0), Eigen::ComputeThinU | 
                                                   Eigen::ComputeThinV);
    double scale = svd.singularValues()(0);
    Mat3 sI3 = scale * Mat3::Identity();
    H->block<3,3>(0, 0) = svd.matrixU() * sI3 * svd.matrixV().transpose();
    return true;
  }
  return false;
}

bool ExtractSimilarity2DCoefficients(const Mat3 &M,
                                     Vec2   *tr,
                                     double *angle,
                                     double *scale) {
  Vec4 p;
  Similarity2DSAParameterization<double>::From(M, &p);  
  *scale = p(0);
  *angle = p(1);  
  *tr << p(2), p(3);
  return true;
}
} // namespace libmv
