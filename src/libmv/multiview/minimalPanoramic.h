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
//

#ifndef LIBMV_MULTIVIEW_MINIMAL_PANORAMIC_H_
#define LIBMV_MULTIVIEW_MINIMAL_PANORAMIC_H_

#include "libmv/numeric/numeric.h"
#include "libmv/numeric/poly.h"
#include "libmv/base/vector.h"

namespace libmv {

// Minimal solutions for panoramic stitching.
// http://www.cs.ubc.ca/~mbrown/minimal/minimal.html
// 2-Point algorithm: rotation of the camera with a single focal length
// (4 degrees of freedom)
//-- An error have been found in the detail of the calculus ( formulae (17) ).

//  InProceedings BHN07
//  author =   M. Brown and R. Hartley and D. Nister,
//  title =    Minimal Solutions for Panoramic Stitching,
//  booktitle= Proceedings of the International Conference on Computer Vision
//              and Pattern Recognition (CVPR07)

// Compute from 1 to 3 possible focal lenght for 2 point correspondences.
// Suppose that the camera share the same camera center (and the same focal).
// Image 1 => H*x = x'  =>  Image 2
// x (u1j)                  x' (u2j)
// a (u11)                  a' (u21)
// b (u12)                  b' (u22)
// Return : 1 to three focal lenght :
//     [f 0 0]
// K = [0 f 0]
//     [0 0 1]
static void F_FromCorrespondance_2points(const Vec2 &u11, const Vec2 &u12,
  const Vec2 &u21, const Vec2 &u22, vector<double> * v_focals) {
    assert(v_focals != NULL);
    // Setup the variable of the input problem :
    double a12 = (u11.transpose() * u12)(0);
    double a1  = (u11.norm() * u11.norm());
    double a2  = (u12.norm() * u12.norm());
    double b12 = (u21.transpose() * u22)(0);
    double b1  = (u21.norm() * u21.norm());
    double b2  = (u22.norm() * u22.norm());

    // Build the 3rd degre polynomial in F^2.

    // f^6 * p + f^4 * q + f^2* r + s = 0;

    double p=  ( 2*a12 - 2*b12 + b1 +b2 - a1 -a2 );

    double q = ( a12*a12 - b12*b12 + 2*a12*(b1 + b2) - 2*b12*(a1 + a2) -
      a1*a2 + b1*b2 );

    double r = ( a12*a12*(b1+ b2) -  b12*b12*(a1 + a2) + 2*a12*b1*b2 -
      2*b12*a1*a2 );

    double s = a12*a12*b1*b2 - b12*b12*a1*a2;

    //- Solve it by using F = f^2 and a Cubic polynomial solver
    // F^3 * p + F^2 * q + F^1 * r + s = 0

    double roots[3]={0.0f,0.0f,0.0f};
    int nbRoots = SolveCubicPolynomial( q/p, r/p, s/p,
      &roots[0], &roots[1], &roots[2]);

    for(int i=0; i < nbRoots; ++i)  {
      if(roots[i]>0)  {
        (*v_focals).push_back( sqrt( roots[i] ) );
      }
    }
}

// Compute the (3*3) Rotation matrix that fit at least square sense
// the two 3D point cloud.
// Method :
// K. Arun,T. Huand and D. Blostein. Least-squares fitting of 2 3-D point sets.
// IEEE Transactions on Pattern Analysis and Machine Intelligence,
// 9:698-700, 1987.

// Given the calibration matrices K1,K2
// solve for the rotation from corresponding image rays.
// R = min || X2 - R * x1 ||.
// 
// In case of panography : (camera that share the same camera center ) 
// H = K2 * R * K1.inverse();
// 
// Full explanation in paragraph 8 :
// Solving for Rotation from 'Minimal Solutions for Panoramic Stitching'.

// Parameters :
// x1 : Point cloud A (3D coords)
// x2 : Point cloud B (3D coords)
//      [f 0 0]
// K1 = [0 f 0]
//      [0 0 1]
// K2 (the same form as K2]
// R the rotation matrix that best fit the problem :
// R = min || X2 - R * x1 ||
static void GetR_FixedCameraCenter(const Mat &x1, const Mat &x2,
  const Mat3 & K1, const Mat3 & K2, Mat3 * R)  {

  assert(3 == x1.rows());
  assert(2 <= x1.cols());
  assert(x1.rows() == x2.rows());
  assert(x1.cols() == x2.cols());

  Mat3 C = Mat3::Zero();
  // Build constraint matrix
  for(int i=0; i < x1.cols(); ++i) {
    Mat r1i = K1.inverse() * x1.col(i);
    r1i /= r1i.norm();

    Mat r2i = K2.inverse() * x2.col(i);
    r2i /= r2i.norm();

    C = C + ( r2i * r1i.transpose() );
  }

  // Solve for rotation angle :
  Eigen::SVD<Mat> svd(C);
  Mat3 ScaleMatrix = Mat3::Identity();
  ScaleMatrix(2,2) = ((svd.matrixU() *
    (svd.matrixV()).transpose()).determinant() > 0) ? 1.0 : -1.0;

  (*R) = svd.matrixU() * ScaleMatrix * svd.matrixV().transpose();
}

}// namespace libmv
#endif //LIBMV_MULTIVIEW_MINIMAL_PANORAMIC_H_
