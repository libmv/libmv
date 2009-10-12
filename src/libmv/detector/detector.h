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

#ifndef LIBMV_DETECTOR_DETECTOR_H
#define LIBMV_DETECTOR_DETECTOR_H

#include "libmv/base/vector.h"

namespace libmv {

class Image;
class Feature;

namespace detector {

/**
 * Opaque detector data class. Implementations of the Describer interface
 * should use dynamic cast to see if the detector data is of the right type.
 */
class DetectorData {
 public:
  virtual ~DetectorData() {};
};

/**
 * Interface for feature detectors.
 */
class Detector {
 public:
  virtual ~Detector() {};
  /**
   * Detects features in an image.
   *
   * \param[in]  image     The image to detect features in.
   * \param[out] features  The detected features. Caller owns contents.
   * \param[out] data      Extra data from a particular detector
   *                       implementation. If NULL, ignored. If a particular
   *                       implementation doesn't create detector data, NULL is
   *                       assigned to *data. If *data is non-null, then the
   *                       caller owns the result.
   *
   * This detects features in an image. Examples include the SURF detector
   * which looks for blobby regions, or the Harris corner detector.
   *
   * Some implementations, while extracting features, may produce extra data
   * such as image pyramids or integral images. The descriptor implementation
   * may be able to make use of that information; for example, the SURF
   * detector produces detection data that can be re-used by the SURF
   * descriptor code.
   */
  virtual void Detect(const Image &image,
                      vector<Feature *> *features,
                      DetectorData **data) = 0;
};

}  // namespace detector
}  // namespace libmv

#endif  // LIBMV_DETECTOR_DETECTOR_H
