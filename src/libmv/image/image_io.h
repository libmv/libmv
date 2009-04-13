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

#ifndef LIBMV_IMAGE_IMAGE_IMAGE_IO_H
#define LIBMV_IMAGE_IMAGE_IMAGE_IO_H

#include <math.h>

#include "libmv/image/image.h"

namespace libmv {

enum Format {
  Pnm, Png, Jpg, Tga, Unknown
};

Format GetFormat(const char *c);

int ReadImage(const char *, ByteImage *);
int ReadImage(const char *, FloatImage *);
int WriteImage(const ByteImage &, const char *);
int WriteImage(const FloatImage &, const char *);

int ReadPng(const char *, ByteImage *);
int ReadPng(const char *, FloatImage *);
int ReadPngStream(FILE *, ByteImage *);
int WritePng(const ByteImage &, const char *);
int WritePng(const FloatImage &, const char *);
int WritePngStream(const ByteImage &, FILE *);

int ReadJpg(const char *, ByteImage *);
int ReadJpg(const char *, FloatImage *);
int ReadJpgStream(FILE *, ByteImage *);
int WriteJpg(const ByteImage &, const char *, int quality=90);
int WriteJpg(const FloatImage &, const char *, int quality=90);
int WriteJpgStream(const ByteImage &, FILE *, int quality=90);

int ReadPnm(const char *filename, ByteImage *im);
int ReadPnm(const char *filename, FloatImage *im);
int ReadPnmStream(FILE *file, ByteImage *im);
int WritePnm(const ByteImage &im, const char *filename);
int WritePnm(const FloatImage &im, const char *filename);
int WritePnmStream(const ByteImage &im, FILE *file);

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_IMAGE_IO_H
