// Copyright 2008 Keir Mierle, all rights reserved.
//
// Concrete image classes.
//
extern "C" {
//#include "opencv/cv.h"
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

// TESTING
//#include <vil/vil_image_view.h>
//#include <vil/vil_load.h>
//#include <vil/vil_save.h>
//#include <vil/vil_convert.h>

/*
typedef vil_image_view<float> Image;

Image Image_load(const char *filename)
{
        vil_image_view<vxl_byte> tmp;
        tmp = vil_load(filename);
        Image res(tmp.ni(),tmp.nj(),1,tmp.nplanes());
        vil_convert_cast(tmp,res);
        return res;
}
void Image_save(const Image &im, const char *filename)
{
        vil_image_view<vxl_byte> tmp(im.ni(),im.nj(),1,im.nplanes());
        vil_convert_cast(im,tmp);
        vil_save(tmp,filename);
}
*/
// END TESTING

}  // namespace libmv
