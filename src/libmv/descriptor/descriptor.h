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

namespace libmv {

class Feature;

namespace detector {
class DetectorData;
}  // namespace detector

namespace descriptor {

class Descriptor {

};

/**
 * Interface for computing descriptors of features in images.
 */
class Describer {
 public:
  /**
   * Describes features in an image, in preparation for matching.
   *
   * \param[in]  features      The features to find descriptions for.
   * \param[in]  image         The image to compute descriptions from.
   * \param[in]  detector_data Data from the detector or NULL.
   * \param[out] descriptors   The computed descriptors, in the same order as
   *                           the features vector. A NULL entry indicates that
   *                           the description couldn't be computed for that
   *                           feature.
   *
   * This computes descriptions for localized features in an image.
   * Examples include the SURF descriptor based on box filters or KLT
   * features which are image patches. 
   *
   * Some implementations can reuse data from the detector; see the detector
   * description.
   */
  virtual void Describe(const vector<Feature *> &features,
                        const Image &image,
                        const DetectorData *detector_data,
                        vector<Descriptor *> *descriptors) = 0;
};

}  // namespace descriptor
}  // namespace libmv
