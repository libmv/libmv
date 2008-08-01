// Copyright (c) 2007, 2008 libmv authors.
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

#include "libmv/image/filtered_sequence.h"

namespace libmv {

Filter::~Filter() {}

FilteredImageSequence::FilteredImageSequence(ImageSequence *source,
                                             Filter *filter)
      : CachedImageSequence(source->Cache()),
        source_(source),
        filter_(filter) {}

FilteredImageSequence::~FilteredImageSequence() {
  delete filter_;
}

Image *FilteredImageSequence::LoadImage(int i) {
  Array3Df *destination_image = new Array3Df;
  // TODO(keir): Make this not specfic to float images...
  Array3Df *source_image = source_->GetFloatImage(i);
  filter_->RunFilter(*source_image, destination_image);
  source_->Unpin(i);
  return new Image(destination_image);
}

}  // namespace libmv
