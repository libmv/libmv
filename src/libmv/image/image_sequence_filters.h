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

#ifndef LIBMV_IMAGE_IMAGE_SEQUENCE_FILTERS_H_
#define LIBMV_IMAGE_IMAGE_SEQUENCE_FILTERS_H_

namespace libmv {

class ImageSequence;

// Produce a three-channel sequence from a monochrome sequence with:
// Channel 0: Source image Gaussian blurred with a kernel of variance sigma.
// Channel 1: X derivative of channel 0.
// Channel 2: Y derivative of channel 0.
ImageSequence *BlurSequenceAndTakeDerivatives(ImageSequence *source,
                                              double sigma);

// Downsample each image in source by 2 in each dimension.
ImageSequence *DownsampleSequenceBy2(ImageSequence *source);

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_SEQUENCE_FILTERS_H_
