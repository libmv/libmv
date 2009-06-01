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


#include "libmv/multiview/projection.h"
#include "libmv/multiview/robust_estimation.h"
#include "libmv/multiview/homography.h" 

namespace libmv {
  

void HomographyFromCorrespondencesLinear(const Mat &x,
                                         const Mat &y,
                                         Mat3 *H) {
  assert(2 == x.rows());
  assert(4 <= x.cols());
  assert(x.rows() == y.rows());
  assert(x.cols() == y.cols());

  int n = x.cols();
  
  Mat L = Mat::Zero(n * 2, 9);
  for (int i = 0; i < n; i++){    
    int j = 2 * i;
    L(j, 0) = x(0, i);
    L(j, 1) = x(1, i);
    L(j, 2) = 1.0; 
    L(j, 6) = -y(0, i) * x(0, i);
    L(j, 7) = -y(0, i) * x(1, i);
    L(j, 8) = -y(0, i);  
      
    j = j+1;
    L(j, 3) = x(0, i);
    L(j, 4) = x(1, i);
    L(j, 5) = 1.0; 
    L(j, 6) = -y(1, i) * x(0, i);
    L(j, 7) = -y(1, i) * x(1, i);
    L(j, 8) = -y(1, i);       
  }

  Vec h;    
  Nullspace(&L, &h);
  
  int k = 0;
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      (*H)(i, j) = h(k);
      k = k + 1;
    }
  }
}


// ROBUST ESTIMATION ///////////////////////////////////////////////////////////
struct HomographyModel {
 public:
  HomographyModel() {}
  HomographyModel(Mat3 _H) : H(_H) {}

  template<typename TVec>
  double Error(TVec x1x2) {
    double homography_error = HomographyError(H, x1x2.start(2), x1x2.end(2));
    VLOG(4) << "Homography error^2 = " << homography_error;
    return homography_error;
  }
  
  Mat3 H;
};

class HomographyFitter {
 public:
  void Fit(Mat4X &x1x2, std::vector<HomographyModel> *models) {
    Mat3 H;
    int n = x1x2.cols();
    Mat2X x1s(x1x2.block(0, 0, 2, n));
    Mat2X x2s(x1x2.block(2, 0, 2, n));
    
    HomographyFromCorrespondencesLinear(x1s, x2s, &H);
    VLOG(4) << "x1\n" << x1s << "\n";
    VLOG(4) << "x2\n" << x2s << "\n";
    VLOG(4) << "H\n" << H << "\n";
    models->push_back(HomographyModel(H));
  }
  
  int MinimumSamples() {
    return 4;
  }
};

double HomographyError(const Mat3 &H, const Vec2 &x, const Vec2 &y){
  Vec3 yh_estimated = H * EuclideanToHomogeneous(x);
  return (y - HomogeneousToEuclidean(yh_estimated)).squaredNorm();
}

void HomographyFromCorrespondencesLinearRobust(const Mat &x,
                                               const Mat &y,
                                               double max_error,
                                               Mat3 *H,
                                               std::vector<int> *inliers){
  // The threshold is on the sum of the squared errors in the two images.
  double threshold = Square(max_error);
  Mat4X x1x2;
  VerticalStack(x, y, &x1x2);
  HomographyModel model =
      Estimate<HomographyModel>(x1x2,
                                HomographyFitter(),
                                ThresholdClassifier(threshold),
                                MLECost(threshold),
                                inliers);
  *H = model.H;                                
}



}  // namespace libmv
