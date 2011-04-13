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

#include "libmv/image/image.h"
#include "libmv/numeric/numeric.h"

namespace libmv {
  
// TODO(julien) ReflectHoriImage
// TODO(julien) ReflectVertImage
// TODO(julien) ScaleImage
// TODO(julien) ShearHoriImage
// TODO(julien) SquezeImage

/**
 * Computes the bounding box of a image warp.
 *
 * \param image_size The image size (width, height)
 * \param H The 2D warp matrix
 * \param bbox The bounding box (xmin, xmax, ymin, ymax)
 */
void ComputeBoundingBox(const Vec2u &image_size,
                        const Mat3 &H,
                        Vec4i *bbox);

/**
 * Resize an image so that all the warpped image will be kept.
 *
 * \param image_size The new image size (width, height)
 * \param H The 2D warp matrix
 * \param image The image to resize
 * \param Hreg The new warp matrix in that translates the original image.
 * \param bbox The bounding box (xmin, xmax, ymin, ymax)
 *
 * A typical use is:
 *   FloatImage image_in, image_out;
 *   Mat3 H; // setup the warp matric here
 *   ResizeImage(image_size, H, &image_out, &H);
 *   image_out.fill(0); //fill with black pixels
 *   WarpImage(image_in, H,image_size, 0.5, &image_out);
 */
void ResizeImage(const Vec2u &image_size,
                 const Mat3 &H,
                 FloatImage *image,
                 Mat3  *Hreg = NULL,
                 Vec4i *bbox = NULL);

/**
 * Translate the image.
 *
 * \param image_in The input image
 * \param dx The x translation (in pixel)
 * \param dy The x translation (in pixel)
 * \param image_out The output image that will contains the translated input 
 *                  image
 * \param adapt_img_size The output image will be resized to contain all 
 *                       the translated input image
 * 
 */
void TranslateImage(const FloatImage &image_in,
                    double dx,
                    double dy,
                    FloatImage *image_out,
                    bool adapt_img_size = false);

/**
 * Rotates the image.
 * 
 * The rotation's center is the center of the image
 *
 * \param image_in The input image
 * \param angle The rotation's angle (radian)
 * \param image_out The output image that will contains the rotated input image
 * \param adapt_img_size The output image will be resized to contain all 
 *                       the rotated input image
 */
void RotateImage(const FloatImage &image_in,
                 double angle,
                 FloatImage *image_out,
                 bool adapt_img_size = false);

/**
 * Warps an input image by a 3x3 matrix H and write the result in another image.
 *
 * \param image_in The input image
 * \param H The 2D warp matrix
 * \param image_out The output image that will contains the warpped input image
 * \param adapt_img_size The output image will be resized to contain all 
 *                       the rotated input image
 *
 * \note image_out SHOULD NOT be the image_in! (no local copy)
 *                 use XXX instead (TODO(julien) make a WarpImageMe function
 */
void WarpImage(const FloatImage &image_in,
               const Mat3 &H,
               FloatImage *image_out,
               bool adapt_img_size = false);

/**
 * Warps an input image and blend it with the content of the output image.
 * 
 * Black pixels of image_out are not blender but overwritten.
 *
 * \param image_in The input image
 * \param H The 2D warp matrix
 * \param image_out The output image that will contains the warpped input image
 * \param blending_ratio The blending ratio for overlapping zones, 
 *                       a typical value is 0.5
 *
 * \note image_out SHOULD NOT be the image_in! (no local copy)
 * \note image_out is not resized.
 * TODO(julien) put this in image_blending
 */
void WarpImageBlend(const FloatImage &image_in,
                    const Mat3 &H,
                    FloatImage *image_out,
                    float blending_ratio = 0.5);
} // namespace libmv 
