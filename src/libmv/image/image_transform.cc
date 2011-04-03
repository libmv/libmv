// Copyright (c) 2011 libmv authors.
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

#include "libmv/image/image_transform.h"

#include "libmv/image/image_drawing.h"
#include "libmv/image/sample.h"
#include "libmv/logging/logging.h"

namespace libmv {

/**
 * Computes the bounding box of a image warp.
 */
void ComputeBoundingBox(const Vec2u &image_size,
                        const Mat3 &H,
                        Vec4i *bbox) {
  Mat34 q_bounds;
  q_bounds << 0, 0,              image_size(0), image_size(0),
              0, image_size(1), image_size(1), 0,
              1, 1,              1,              1;
  
  (*bbox) << image_size(0), 0, image_size(1), 0;
  Vec3 q;
  for (int i = 0; i < 4; ++i) {
    q = H * q_bounds.col(i);
    q /= q(2);
    q(0) = ceil0<double>(q(0));
    q(1) = ceil0<double>(q(1));
    if (q(0) < (*bbox)(0))
      (*bbox)(0) = q(0);
    else if (q(0) > (*bbox)(1))
      (*bbox)(1) = q(0);
    if (q(1) < (*bbox)(2))
      (*bbox)(2) = q(1);
    else if (q(1) > (*bbox)(3))
      (*bbox)(3) = q(1);
  }
}

/**
 * Resize an image so that all the warpped image will be kept.
 */
void ResizeImage(const Vec2u &image_size,
                 const Mat3 &H,
                 FloatImage *image,
                 Mat3  *Hreg,
                 Vec4i *bbox) {
  assert(image_out != NULL);
  Vec4i bbox_loc;
  Vec4i *bbox_ptr = bbox;
  if (bbox_ptr == NULL)
    bbox_ptr = &bbox_loc;
  
  ComputeBoundingBox(image_size, H, bbox_ptr);
  assert((*bbox_ptr)(1) < (*bbox_ptr)(0));
  assert((*bbox_ptr)(3) < (*bbox_ptr)(2));
  
  const unsigned int w = (*bbox_ptr)(1) - (*bbox_ptr)(0);
  const unsigned int h = (*bbox_ptr)(3) - (*bbox_ptr)(2);
  image->Resize(h, w, image->Depth());
  // Register the image so that the min (x, y) are (0, 0)
  if (Hreg) {
    (*Hreg) << 1, 0, -(*bbox_ptr)(0),
               0, 1, -(*bbox_ptr)(2),
               0, 0, 1;
    (*Hreg) = (*Hreg) * H;
  }
}

/**
 * Translates the image.
 */
void TranslateImage(const FloatImage &image_in,
                    double dx,
                    double dy,
                    FloatImage *image_out,
                    bool adapt_img_size) {
  Mat3 H;
  H << 1, 0, dx,
       0, 1, dy,
       0, 0, 1;
  if (adapt_img_size) {
    Vec2u image_size;
    image_size << image_in.Width(), image_in.Height();
    ResizeImage(image_size, H, image_out, &H);
  }
  WarpImage(image_in, H, image_out);
}

/**
 * Rotates the image.
 */
void RotateImage(const FloatImage &image_in,
                 double angle,
                 FloatImage *image_out,
                 bool adapt_img_size) {
  Mat3 H, Hr, Ht;
  Hr << cos(angle), -sin(angle),  0,
        sin(angle),  cos(angle),  0,
               0,           0,    1;
  Ht << 1, 0, -image_in.Height()/2,
        0, 1, -image_in.Width()/2,
        0, 0,  1;
  H = Ht.inverse() * Hr * Ht;
  if (adapt_img_size) {
    Vec2u image_size;
    image_size << image_in.Width(), image_in.Height();
    ResizeImage(image_size, H, image_out, &H);
  }
  WarpImage(image_in, H, image_out);
}

/**
 * Warps an input image by a 3x3 matrix H and write the result in another image.
 */
void WarpImage(const FloatImage &image_in,
               const Mat3 &H,
               FloatImage *image_out,
               bool adapt_img_size) {
  assert(image_out != NULL);
  assert(image_in.Depth() == image_out->Depth());
  Vec4i bbox;
  Vec2u image_size;
  image_size << image_in.Width(), image_in.Height();
  Mat3 Hbis = H;
  if (adapt_img_size) {
    ResizeImage(image_size, H, image_out, &Hbis, &bbox);
  } else {
    ComputeBoundingBox(image_size, H, &bbox);
  }
  const Mat3 Hinv = Hbis.inverse();
  //-- (Backward mapping. For the destination pixel search which pixel
  //    contribute ?).
  Vec3 qi, qm;
  for (int j = bbox(2); j <= bbox(3); ++j)
    for (int i = bbox(0); i <= bbox(1); ++i)
      if (image_out->Contains(j, i)) {
        qm << i, j, 1.0;
        qi = Hinv * qm;
        qi /= qi(2);
        const int xImage = static_cast<int>(qi(0));
        const int yImage = static_cast<int>(qi(1));
        if (image_in.Contains(yImage, xImage)) {
          for (int d = 0; d < image_out->Depth(); ++d) 
            (*image_out)(j, i, d) = SampleLinear(image_in, qi(1), qi(0), d);
        }
      }
}

/**
 * Warps an input image and blend it with the content of the output image.
 */
void WarpImageBlend(const FloatImage &image_in,
                    const Mat3 &H,
                    FloatImage *image_out,
                    float blending_ratio) {
  assert(image_out != NULL);
  assert(image_in.Depth() == image_out->Depth());
  Vec4i bbox;
  const Mat3 Hinv = H.inverse();
  Vec2u image_size;
  image_size << image_in.Width(), image_in.Height();
  ComputeBoundingBox(image_size, H, &bbox);
  
  //-- Fill destination image
  //-- (Backward mapping. For the destination pixel search which pixel
  //    contribute ?).
  Vec3 qi, qm;
  bool bOutContrib=false;
  for (int j = bbox(2); j <= bbox(3); ++j)
    for (int i = bbox(0); i <= bbox(1); ++i)
      if (image_out->Contains(j, i)) {
        //- Algo :
        // For the destination pixel (i,j) search which pixel from image_in
        //  and image_out contribute.
        // Perform a mean blending in the overlap zone, transfert original
        //  value in the other part.
        qm << i, j, 1.0;
        qi = Hinv * qm;
        qi /= qi(2);
        const int xImage = static_cast<int>(qi(0));
        const int yImage = static_cast<int>(qi(1));
        if (image_in.Contains(yImage, xImage)) {
          bOutContrib = true;
          for (int d = 0; d < image_out->Depth(); ++d) 
            if ((*image_out)(j, i, d) > 0) {
              bOutContrib = false;
              break;
            }
          if(bOutContrib) { //mean blending between image_out and image_in
            for (int d = 0; d < image_out->Depth(); ++d) {
              // Let's fade the previous frames
              (*image_out)(j, i, d) = 
                 (1 - blending_ratio) * (*image_out)(j, i, d) +
                 blending_ratio * SampleLinear(image_in, qi(1), qi(0), d);
            }
            continue;
          } else { //only image_in contrib
            for (int d = 0; d < image_out->Depth(); ++d) 
              (*image_out)(j, i, d) = SampleLinear(image_in, qi(1), qi(0), d);
            continue;
          }
        }
      }
}
} // namespace libmv 
