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

#include <vector>

#include "libmv/image/cached_image_sequence.h"

namespace libmv {

class MockImageSequence : public CachedImageSequence {
 public:
  virtual ~MockImageSequence() {}

  MockImageSequence(ImageCache *cache)
      : CachedImageSequence(cache) {}

  virtual Image *LoadImage(int i) {
    // Always produces a fresh copy, because the cache owns the result of this
    // function.
    return new Image(new Array3Df(*images_[i]));
  }

  virtual int Length() {
    return images_.size();
  }

  void Append(Array3Df *image) {
    images_.push_back(image);
  }

 private:
  std::vector<Array3Df *> images_;
};

}  // namespace libmv
