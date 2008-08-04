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

#ifndef LIBMV_IMAGE_PYRAMID_SEQUENCE_H_
#define LIBMV_IMAGE_PYRAMID_SEQUENCE_H_

namespace libmv {

class ImagePyramid;

class PyramidSequence {
 public:
  virtual ~PyramidSequence();

  // Length of underlying image sequence.
  virtual int Length() = 0;

  // Get an image pyramid constructed from frame_number in the source sequence.
  // The PyramidSequence retains ownership of the ImagePyramid (callers must
  // not delete it). The ImagePyramid remains valid while the PyramidSequence
  // is in scope.
  virtual ImagePyramid *Pyramid(int frame_number) = 0;
};

PyramidSequence *MakePyramidSequence(ImageSequence *sequence,
                                     int levels,
                                     double sigma);

// This is pau trying things
PyramidSequence *MakeSimplePyramidSequence(ImageSequence *sequence,
                                           int levels,
                                           double sigma);
}  // namespace libmv

#endif  // LIBMV_IMAGE_PYRAMID_SEQUENCE_H_
