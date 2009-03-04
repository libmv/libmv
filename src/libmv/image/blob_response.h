// Copyright (c) 2007, 2008, 2009 libmv authors.
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

#ifndef LIBMV_IMAGE_BLOB_RESPONSE_H
#define LIBMV_IMAGE_BLOB_RESPONSE_H

#include <cmath>

#include "libmv/numeric/numeric.h"
#include "libmv/image/integral_image.h"
#include "libmv/logging/logging.h"

namespace libmv {

// Compute the 'interestingness' of an image at a certain scale
// Compute an approximate hessian for each pixel for the image used to generate
// integral_image. integral_image must be an integral image as computed by
// IntegralImage(). The typical gaussian filters are replaced with box filter
// approximations. If downsample_hessian_by is set to something other than 1,
// the result is downsampled before storing in hessian_image.
template<typename TImage, typename TBlobResponse>
inline void BlobResponse(const TImage &integral_image,
                         int lobe_size,
                         int scale,
                         TBlobResponse *blob_response) {
  typedef typename TImage::Scalar Scalar;

  // See Figure 5 (on page 5) from the SURF paper. The filter size is
  // determined by the lobe size, which must increase by steps of 2 to maintain
  // the odd size (i.e. there is a central pixel). In practice this means
  // filter sizes go by 9, 15, 21, 27, 33, 39, 45, etc.
  const int L = lobe_size;
  const int W = 3*L;
  Scalar inverse_area = Scalar(1.0) / W / W;

  LOG(INFO) << "Filtering a " << integral_image.cols()
            << "x" << integral_image.rows()
            << " image with a kernel size: " << W << "x" << W;

  blob_response->resize(integral_image.rows() / scale,
                        integral_image.cols() / scale);
  // TODO(keir): Re-enable the below line once eigen trunk is merged.
  //blob_response->fill(0.);  // TODO(keir): Make border clearing smarter.

  int B = W / 2;
  // Make the top left border so that UnsafeBoxIntegral is in bounds.
  for (int r = B + 1; r < integral_image.rows() - B; r += scale) {
    for (int c = B + 1; c < integral_image.cols() - B; c += scale) {
      // Compute filter responses, which approximate filtering by the
      // derivative of a gaussian kernel (like in the KLT code).
      const TImage &ii = integral_image;
      Scalar Dxx, Dxy, Dyy;
      Dxx =   UnsafeBoxIntegral(ii, r - L + 1, c - B,     2*L - 1, W)
            - UnsafeBoxIntegral(ii, r - L + 1, c - L / 2, 2*L - 1, L)*3;
      Dyy =   UnsafeBoxIntegral(ii, r - B,     c - L + 1, W, 2*L - 1)
            - UnsafeBoxIntegral(ii, r - L / 2, c - L + 1, L, 2*L - 1)*3;
      Dxy = + UnsafeBoxIntegral(ii, r - L, c + 1, L, L)
            + UnsafeBoxIntegral(ii, r + 1, c - L, L, L)
            - UnsafeBoxIntegral(ii, r - L, c - L, L, L)
            - UnsafeBoxIntegral(ii, r + 1, c + 1, L, L);

      // Filter size should not affect response, so normalize by area.
      Dxx *= inverse_area;
      Dyy *= inverse_area;
      Dxy *= inverse_area;

      // The 0.91 magic number is from the SURF paper; Equation 4 on page 4.
      Scalar determinant = (Dxx*Dyy - pow(Scalar(0.91)*Dxy, 2));

      // Clamp negative determinants, which indicate an edge rather than a blob.
      Scalar response = (determinant < 0.0) ? 0 : determinant;
      (*blob_response)(r / scale, c / scale) = response;
    }
  }
}

}  // namespace libmv
#endif  // LIBMV_IMAGE_BLOB_RESPONSE_H
