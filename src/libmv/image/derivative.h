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

#ifndef LIBMV_IMAGE_DERIVATIVE_H
#define LIBMV_IMAGE_DERIVATIVE_H

#include <cmath>

#include "libmv/numeric/numeric.h"

namespace libmv {

// Central difference gradient approximation to a discrete 3D function. The
// function must be defined at x, y, z +/- 1 as no bounds checking is
// performed.
template<typename TArray>
Vec3 Gradient3D(const TArray &f, int x, int y, int z) {
  typename TArray::Scalar dx, dy, dz;
  dx = ( f( x + 1, y,     z     ) - f( x - 1, y,     z     ) ) / 2.;
  dy = ( f( x,     y + 1, z     ) - f( x,     y - 1, z     ) ) / 2.;
  dz = ( f( x,     y,     z + 1 ) - f( x,     y,     z - 1 ) ) / 2.;

  Vec3 d; d << dx, dy, dz;
  return d;
}

// Central difference hessian approximation to a discrete 3D function. The
// function must be defined at x, y, z +/- 1 as no bounds checking is
// performed.
template<typename TArray>
Mat3 Hessian3D(const TArray &f, int x, int y, int z) {
  // As found at http://en.wikipedia.org/wiki/Finite_difference
  typename TArray::Scalar dxx, dyy, dzz, dxy, dxz, dyz;
  dxx = ( f( x + 1, y,     z     ) +
          f( x - 1, y,     z     ) -
      2 * f( x,     y,     z     ) );
  dyy = ( f( x,     y + 1, z     ) +
          f( x,     y - 1, z     ) -
      2 * f( x,     y,     z     ) );
  dzz = ( f( x,     y,     z + 1 ) + 
          f( x,     y,     z - 1 ) -
      2 * f( x,     y,     z     ) );
  dxy = ( f( x + 1, y + 1, z     ) -
          f( x + 1, y - 1, z     ) -
          f( x - 1, y + 1, z     ) +
          f( x - 1, y - 1, z     ) ) / 4.;
  dxz = ( f( x + 1, y,     z + 1 ) -
          f( x + 1, y,     z - 1 ) -
          f( x - 1, y,     z + 1 ) +
          f( x - 1, y,     z - 1 ) ) / 4.;
  dyz = ( f( x,     y + 1, z + 1 ) -
          f( x,     y + 1, z - 1 ) -
          f( x,     y - 1, z + 1 ) +
          f( x,     y - 1, z - 1 ) ) / 4.;

  Mat3 H;
  H << dxx, dxy, dxz,
       dxy, dyy, dyz,
       dxz, dyz, dzz;

  return H;
}

}  // namespace libmv

#endif  // LIBMV_IMAGE_DERIVATIVE_H
