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

#ifndef LIBMV_IMAGE_IMAGE_SEQUENCE_H_
#define LIBMV_IMAGE_IMAGE_SEQUENCE_H_


// TODO(keir): This is TOTALLY UNFINISHED and needs to be adjusted to use the
// new generic Image() rather than the current floatarray/etc.

#include <string>
#include <vector>

#include "libmv/image/image.h"

namespace libmv {

// An image sequence with caching behaviour. Callers must Unpin() images that
// they load to allow cached images to be freed.
template<T>
class ImageSequence {
 public:
  virtual Image<T> *LoadPinned(int i) = 0;
  virtual void Unpin(int i) = 0;
  virtual int size() = 0;
  virtual int channels() = 0;
};

typedef ImageSequence<unsigned char> ByteImageSequence;
typedef ImageSequence<float> FloatImageSequence;

ByteImageSequence *ByteImageSequenceFromFiles(
    const std::vector<string> &filenames);

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_SEQUENCE_H_
