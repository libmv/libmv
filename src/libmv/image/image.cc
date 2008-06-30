// Copyright 2008 Keir Mierle, all rights reserved.
//
// Concrete image classes. For now based on OpenCV.

extern "C" {
#include "opencv/highgui.h"
}

#include <string>

#include "libmv/public/base.h"
#include "libmv/public/image.h"

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
