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

#include "libmv/logging/logging.h"
#include "libmv/descriptor/descriptor.h"
#include "libmv/descriptor/vector_descriptor.h"
#include "libmv/correspondence/feature.h"
#include "libmv/image/image.h"
#include "third_party/daisy/include/daisy/daisy.h"

namespace libmv {
namespace descriptor {

// TODO(keir): This is utterly untested!
class DaisyDescriber : public Describer {
 public:
  virtual void Describe(const vector<Feature *> &features,
                        const Image &image,
                        const detector::DetectorData *detector_data,
                        vector<Descriptor *> *descriptors) {
    (void) detector_data;  // There is no matching detector for DAISY.

    daisy desc;

    // TODO(keir): DAISY has extensive configuration options; consider exposing
    // them via some sort of config system.

    // Defaults from README.
    desc.set_parameters(15, 3, 8, 8);

    // Push the image into daisy. This will make a copy and convert to float.
    ByteImage *byte_image = image.AsArray3Du();
    desc.set_image(byte_image->Data(),
                   byte_image->Height(),
                   byte_image->Width());

    // Only use sparse descriptors; the default is dense.
    desc.initialize_single_descriptor_mode();

    descriptors->resize(features.size());
    for (int i = 0; i < features.size(); ++i) {
      PointFeature *point = dynamic_cast<PointFeature *>(features[i]);
      VecfDescriptor *descriptor = NULL;
      if (point) {
        descriptor = new VecfDescriptor(desc.descriptor_size());
        desc.get_descriptor(point->y(),
                            point->x(),
                            point->orientation * 180.0 / 3.14159f,
                            descriptor->coords.data());
      }
      (*descriptors)[i] = descriptor;
    }
  }
};

Describer *CreateDaisyDescriber() {
  return new DaisyDescriber;
}

}  // namespace descriptor
}  // namespace libmv
