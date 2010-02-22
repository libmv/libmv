// Copyright (c) 2010 libmv authors.
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

#ifndef LIBMV_DETECTOR_ORIENTATION_DETECTOR_H
#define LIBMV_DETECTOR_ORIENTATION_DETECTOR_H

#include "libmv/base/vector.h"
#include "libmv/correspondence/feature.h"

namespace libmv {

class Image;
class Feature;

namespace detector {

/// Return the coterminal angle between [0;2*PI].
/// Angle value are considered in Radian.
inline float getCoterminalAngle(float angle)
{
  const float f2PI = 2.f*3.14159f;
  while (angle > f2PI) {
    angle -=f2PI;
  }
  while (angle < 0.0f) {
    angle +=f2PI;
  }
  return angle;
}

/// Detect the orientation of a given feature
template<class Image>
void fastRotationEstimation(const Image & ima, vector<Feature *> & features)
{
  // Build fast ring orientation and index.
  const double fast_ring_x[16] = {0, 1/sqrt(10.), 1/sqrt(2.), 3/sqrt(10.), 1,
    3/sqrt(10.), 1/sqrt(2.), 1/sqrt(10.), 0, -1/sqrt(10.), -1/sqrt(2.),
    -3/sqrt(10.), -1, -3/sqrt(10.), -1/sqrt(2.), -1/sqrt(10.)};
  const double fast_ring_y[16] = {-1, -3/sqrt(10.), -1/sqrt(2.),
    -1/sqrt(10.), 0, 1/sqrt(10.), 1/sqrt(2.), 3/sqrt(10.), 1, 3/sqrt(10.),
    1/sqrt(2.), 1/sqrt(10.), 0, -1/sqrt(10.), -1/sqrt(2.), -3/sqrt(10.)};

  const int indX[16] = {3,3,2,1,0,-1,-2,-3,-3,-3,-2,-1,0,1,2,3};
  const int indY[16] = {0,1,2,3,3,3,2,1,0,-1,-2,-3,-3,-3,-2,-1};

  // For each feature estimate the rotation angle.
  for (int j=0; j < features.size(); ++j) {
    double dx = 0.0;
    double dy = 0.0;

    const int xPos = ((PointFeature*)features[j])->x(),
              yPos = ((PointFeature*)features[j])->y();
    double centrepx = ima( yPos, xPos);
    // For the fast ring add weighted gradient.
    for (int px = 0; px < 16; ++px)  {
     double diff = ima( yPos + indY[px], xPos + indX[px]) - centrepx;
     dx += diff * fast_ring_x[px];
     dy += diff * fast_ring_y[px];
    }

    double fastrot = 0.0;
    if ( std::max(abs(dy), abs(dx)) > 0) {
     fastrot = atan2(dy, dx);
    }
    ((PointFeature*)features[j])->orientation = getCoterminalAngle(fastrot);
  }
}

/// Detect the orientation of a given feature
template<class Image>
void gradientBoxesRotationEstimation(const Image & ima, vector<Feature *> & features)
{
  vector<float> histogram(36);

  // Sum up direction weighted (gradient value).
  for (int j=0; j < features.size(); ++j) {
    const int x = ((PointFeature*)features[j])->x(),
              y = ((PointFeature*)features[j])->y();
    const int offset = 6; // Todo(pmoulon) Make it related to scale
    const int offsetDiv2 = offset/2;
    std::fill(histogram.begin(),histogram.end(),0.0f);
    for (int r=y-offset; r<=y+offset; ++r){
      for (int c=x-offset; c<=x+offset; ++c){
        // Todo(pmoulon) Compute it only in a circular area to be more robust to rotation.
        if ((c>0) && (r>0) && (c<ima.Width()-1) && (r<ima.Height()-1))  {

          double offsetX = c - x;
          double offsetY = r - y;
          double circularWeight = sqrt( (offsetX*offsetX + offsetY*offsetY) / offset );

          if (circularWeight<=1.0)  {

            double tmpX = ima(r,c+1) - ima(r,c-1);
            double tmpY = ima(r+1,c) - ima(r-1,c);
            double magnit = sqrt(tmpX*tmpX + tmpY*tmpY);
            double orient = atan2(tmpY, tmpX);

            double value = magnit * circularWeight;
            if (orient<0.0)  {
              orient += 2.0 * 3.14159;
            }
            int histindex = (int)(orient* 180.f / 3.14159f)/10;
            histogram[histindex] += value;
          }
        }
      }
    }
    // Locate peak in angle histogram.
    int index = 0;
    for (int k=1; k<36; ++k) {
      if(histogram[index]<histogram[k])
      index = k;
    }
    // Todo(pmoulon) subpixel angle according histogram value.
    // http://people.equars.com/~marco/poli/phd/node47.html
    ((PointFeature*)features[j])->orientation = getCoterminalAngle(index*10.0 * 3.14159 / 180.0);
  }
}


}  // namespace detector
}  // namespace libmv

#endif  // LIBMV_DETECTOR_ORIENTATION_DETECTOR_H
