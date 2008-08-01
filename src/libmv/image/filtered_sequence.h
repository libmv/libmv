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

#ifndef LIBMV_IMAGE_FILTERED_SEQUENCE_H_
#define LIBMV_IMAGE_FILTERED_SEQUENCE_H_

#include "libmv/image/cached_image_sequence.h"

namespace libmv {

class Filter {
 public:
  virtual ~Filter();
  virtual void RunFilter(const Array3Df &source, Array3Df *destination) = 0;
};

class FilteredImageSequence : public CachedImageSequence {
 public:
  FilteredImageSequence(ImageSequence *source, Filter *filter);
  virtual ~FilteredImageSequence();
  virtual Image *LoadImage(int i);
  virtual int Length() {
    return source_->Length();
  }

 private:
  ImageSequence *source_;
  Filter *filter_;
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_FILTERED_SEQUENCE_H_
