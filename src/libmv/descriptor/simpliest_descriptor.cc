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

#include "libmv/base/vector.h"
#include "libmv/logging/logging.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"
#include "libmv/correspondence/feature.h"
#include "libmv/image/image.h"
#include "libmv/image/sample.h"
#include <cmath>

namespace libmv {
namespace descriptor {

/// Normalize the input signal to be invariant to bias and gain.
template < class T>
void normalize(T * fsrc, T * fdst, int size, T &mean, T &stddev)  {
	mean = stddev = 0;
	// Compute mean and standard deviation.
	for(int i=0; i<size; ++i)	{
		const T & val = fsrc[i];
		mean += val;
		stddev += val*val;
	}
	mean /= size;
	stddev = sqrt((stddev - (mean*mean) )/(size-1));
	// Normalize input data.
	for(int i=0; i<size; ++i)
		fdst[i] = (fsrc[i]-mean) / stddev;
}

/// Fill the data patch with image data around keypoint.
// A sampled version of the local image data.
// Use backward rotation to ensure have smoothed data with rotation.
//
// Note :
// Angle is in radian.
// data the output array (must be allocated to 8*8).
template <typename TImage,typename T>
void PickPatch(const TImage & image, float x, float y, int scale, double angle,
                T * data) {

  const int WINDOW_SIZE = 8;
  const int STEP = scale;

	// Inverse rotation (for each output point search into the input image
  // where points we must take into account).

  // Setup the rotation center.
  float & cx = x, & cy = y;
  // Rotation matrix.
  libmv::vector<double> matXY(4);
  // Clockwise rotation matrix.
  matXY[0]=	cos(angle);	matXY[1]=sin(angle);
  matXY[2]=	-sin(angle);	matXY[3]=cos(angle);

  for(int i=0; i < WINDOW_SIZE; ++i)
  {
    for(int j=0; j < WINDOW_SIZE; ++j)
    {
      float ox = (float)(i*STEP -WINDOW_SIZE/2);
      float oy = (float)(j*STEP -WINDOW_SIZE/2);

      float rotX = (matXY[0]*ox+matXY[1]*oy);
      float rotY = (matXY[2]*ox+matXY[3]*oy);
      // Translate the rotated point to the local coordinate system.
      int xx = (int)(rotX)+cx;
      int yy = (int)(rotY)+cy;

      float s1 = 0.0f;
      // Test if the transformed point can be taken in the input image.
      if(xx>=0 && yy>=0 && xx+1< image.Width() && yy+1<image.Height() )
      {
        // Bilinear interpolation
        s1 = SampleLinear(image, (rotY)+cy, (rotX)+cx );
      }
      //else (we cannot take a bilinear sampled value)
      //always return 0 (sampling point outside the image)

      data[j * WINDOW_SIZE + i]=s1;
    }
  }
  // Normalize the input signal to be invariant to luminance.
  float mean=0.0f,stddev=0.0f;
  normalize(data,data,WINDOW_SIZE*WINDOW_SIZE,mean,stddev);
}

class SimpliestDescriber : public Describer {
 public:
  virtual void Describe(const vector<Feature *> &features,
                        const Image &image,
                        const detector::DetectorData *detector_data,
                        vector<Descriptor *> *descriptors) {
    (void) detector_data;  // There is no matching detector for SIMPLIEST.

    const int SIMPLIEST_DESC_SIZE = 64;
    descriptors->resize(features.size());
    for (int i = 0; i < features.size(); ++i) {
      PointFeature *point = dynamic_cast<PointFeature *>(features[i]);
      VecfDescriptor *descriptor = NULL;
      if (point) {
        descriptor = new VecfDescriptor(SIMPLIEST_DESC_SIZE);
        PickPatch( *(image.AsArray3Du()),
                  point->x(),
                  point->y(),
                  point->scale,
                  point->orientation,
                  descriptor->coords.data());
      }
      (*descriptors)[i] = descriptor;
    }
  }
};

Describer *CreateSimpliestDescriber() {
  return new SimpliestDescriber;
}

}  // namespace descriptor
}  // namespace libmv
