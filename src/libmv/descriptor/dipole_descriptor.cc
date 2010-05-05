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

#include "libmv/base/vector.h"
#include "libmv/logging/logging.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"
#include "libmv/correspondence/feature.h"
#include "libmv/image/convolve.h"
#include "libmv/image/image.h"
#include "libmv/image/sample.h"
#include <cmath>

namespace libmv {
namespace descriptor {

//
// Note :
// - Angle is in radian.
// - data the output array (must be allocated to 20 values).
template <typename TImage,typename T>
void PickDipole(const TImage & image, float x, float y, float scale,
                double angle, T * data) {

  // Setup the rotation center.
  float & cx = x, & cy = y;

  double lambda1 = scale;
  double lambda2 = lambda1 / 2.0;
  double angleSubdiv = 2.0 * M_PI / 12.0;

  Vecf dipoleF1(12);
  for (int i = 0; i < 12; ++i)  {
    float xi = cx + lambda1 * cos(angle + i * angleSubdiv);
    float yi = cy + lambda1 * sin(angle + i * angleSubdiv);
    float s1 = 0.0f;
    if (image.Contains(yi,xi) ) {
      // Bilinear interpolation
      s1 = SampleLinear(image, yi, xi);
    }
    dipoleF1(i) = s1;
  }
  Matf A(8,12);
  A <<  0, 0, 0, 1, 0, 0, 0, 0, 0,-1, 0, 0,
        0,-1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 0, 0, 0, 0,-1, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 1, 0, 0,-1, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 1, 0, 0,-1, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,-1,
        0,-1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        1, 0, 0,-1, 0, 0, 0, 0, 0, 0, 0, 0;

  // Add the second order F2 dipole
  Vecf dipoleF2(12);
  for (int i = 0; i < 12; ++i)  {
    double angleSample = i * angleSubdiv;
    float xi = cx + (lambda1 + lambda2) * cos(angle + angleSample);
    float yi = cy + (lambda1 + lambda2) * sin(angle + angleSample);

    float xii = cx + (lambda1 - lambda2) * cos(angle + angleSample);
    float yii = cy + (lambda1 - lambda2) * sin(angle + angleSample);

    float s1 = 0.0f;
    if (image.Contains(yi,xi) && image.Contains(yii,xii)) {
      // Bilinear interpolation
      s1 = SampleLinear(image, yi, xi) - SampleLinear(image, yii, xii);
    }
    dipoleF2(i) = s1;
  }

  (*data).template block<8,1>(0,0) = A * dipoleF1;
  (*data).template block<12,1>(8,0) = dipoleF2;
  // Normalize to be affine luminance invariant (a*I(x,y)+b).
  (*data).normalize();
}

class DipoleDescriber : public Describer {
 public:
  virtual void Describe(const vector<Feature *> &features,
                        const Image &image,
                        const detector::DetectorData *detector_data,
                        vector<Descriptor *> *descriptors) {
    (void) detector_data; // There is no matching detector for DipoleDescriptor.

    const int DIPOLE_DESC_SIZE = 20;
    descriptors->resize(features.size());
    for (int i = 0; i < features.size(); ++i) {
      PointFeature *point = dynamic_cast<PointFeature *>(features[i]);
      VecfDescriptor *descriptor = NULL;
      if (point) {
        descriptor = new VecfDescriptor(DIPOLE_DESC_SIZE);
        PickDipole( *(image.AsArray3Du()),
                  point->x(),
                  point->y(),
                  point->scale,
                  point->orientation,
                  &(descriptor->coords));
      }
      (*descriptors)[i] = descriptor;
    }
  }
};

Describer *CreateDipoleDescriber() {
  return new DipoleDescriber;
}

}  // namespace descriptor
}  // namespace libmv
