// Copyright 2008 Keir Mierle, all rights reserved.
//
// Image class and loading functions for libmv.
//

#ifndef LIBMV_IMAGE_IMAGE_H
#define LIBMV_IMAGE_IMAGE_H

#include <string>
using std::string;

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

#endif  // LIBMV_IMAGE_H
