// Copyright 2008 Keir Mierle, all rights reserved.
//
// Image class and loading functions for libmv.
//

#ifndef SRC_LIBMV_PUBLIC_IMAGE_H
#define SRC_LIBMV_PUBLIC_IMAGE_H

class string;

namespace libmv {

class Image {
 public:
  virtual ~Image() {}
  virtual char *Pixels() = 0;
  virtual int Width() = 0;
  virtual int Height() = 0;
  virtual int Depth() = 0;
};

Image *LoadImage(const char *filename);
Image *LoadImage(const string &filename);

}  // namespace libmv

#endif  // SRC_LIBMV_PUBLIC_IMAGE_H
