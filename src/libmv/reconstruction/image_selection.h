// Copyright (c) 2010 libmv authors.
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

#ifndef LIBMV_RECONSTRUCTION_IMAGE_ORDER_SELECTION_H_
#define LIBMV_RECONSTRUCTION_IMAGE_ORDER_SELECTION_H_

#include "libmv/reconstruction/reconstruction.h"

namespace libmv {

// This method selects an efficient order of images based on an image
// criterion: median homography error x number of common matches
// The outpout images_list contains a list of connected graphs
// (vectors), each vector contains the ImageID ordered by the criterion.
void SelectEfficientImageOrder(
  const Matches &matches, 
  std::list<vector<Matches::ImageID> >*images_list);

}  // namespace libmv

#endif  // LIBMV_RECONSTRUCTION_IMAGE_ORDER_SELECTION_H_
