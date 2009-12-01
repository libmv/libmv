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

namespace libmv {

class Image;
class ImageCache;

// An image sequence. The image sequence always owns the images.  Callers must
// Unpin() images that they access.
class ImageSequence {
 public:
  virtual ~ImageSequence();

  // Retreive an image from the sequence. The image sequence retains ownership
  // of the image. Callers must Unpin() the image after they are done with it.
  virtual Image *GetImage(int i) = 0;
  virtual FloatImage *GetFloatImage(int i);

  // Call this after an image is no longer in use, with an index matching that
  // which was retrieved with GetImage() or GetFloatImage().
  virtual void Unpin(int i) = 0;

  // Number of frames in the sequence.
  virtual int Length() = 0;

  // The image cache. Null if the sequence is uncached.
  virtual ImageCache *Cache();
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_SEQUENCE_H_
