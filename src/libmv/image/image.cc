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
//
// Concrete image classes. For now based on OpenCV.

extern "C" {
#include "opencv/highgui.h"
}

#include <string>

#include "libmv/image/image.h"

namespace libmv {

// Private class that wraps an IplImage
class LibmvImage : public Image {
 public:
  // Takes ownership of image.
  LibmvImage(IplImage *image)
    : image_(image) {};

  ~LibmvImage() {
    cvReleaseImage(&image_);
  }
  char *Pixels() {
    return image_->imageData;
  }
  int Width() {
    return image_->width;
  }
  int Height() {
    return image_->height;
  }
  int Depth() {
    return image_->depth;
  }
 private:
  LibmvImage();
  IplImage *image_;
};

Image *LoadImage(const char *filename) {
  IplImage *image = cvLoadImage(filename);
  if (image) {
    return new LibmvImage(image);
  }
  return NULL;
}

Image *LoadImage(const string &filename) {
  return LoadImage(filename.c_str());
}
}  // namespace libmv
