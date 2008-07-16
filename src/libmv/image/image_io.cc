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

#include <iostream>

#include "libmv/image/image_io.h"

namespace libmv {

int ReadPnm(const char *filename, ByteImage *im) {
  FILE *file = fopen(filename,"r");
  if (!file) {
    return 0;
  }
  int res = ReadPnmStream(file, im);
  fclose(file);
  return res;
}

int ReadPnm(const char *filename, FloatImage *im) {
  ByteImage byte_image;
  int res = ReadPnm(filename, &byte_image);
  if (!res) {
    return res;
  }
  ConvertByteImageToFloatImage(byte_image, im);
  return res;
}

//TODO(pau): should ignore comment lines.
int ReadPnmStream(FILE *file, ByteImage *im) {
  int magicnumber, width, height, depth, maxval;
  int res;

  // Check magic number.
  res = fscanf(file, "P%d", &magicnumber);
  if (res != 1) {
    return 0;
  }
  if (magicnumber == 5) {
    depth = 1;
  } else if (magicnumber == 6) {
    depth = 3;
  } else {
    return 0;
  }

  // Read sizes.
  res = fscanf(file, "%d %d %d", &width, &height, &maxval);
  if (res != 3 || maxval > 255) {
    return 0;
  }

  // Read last white space.
  fseek(file, 1, SEEK_CUR);

  // Read pixels.
  im->Resize(height, width, depth);
  res = fread(im->Data(), 1, im->Size(), file);
  if (res != im->Size()) {
    return 0;
  }
  return 1;
}

int WritePnm(const ByteImage &im, const char *filename) {
  FILE *file = fopen(filename,"w");
  if (!file) {
    return 0;
  }
  int res = WritePnmStream(im, file);
  fclose(file);
  return res;
}

int WritePnm(const FloatImage &image, const char *filename) {
  ByteImage byte_image;
  ConvertFloatImageToByteImage(image, &byte_image);
  return WritePnm(byte_image, filename);
}

int WritePnmStream(const ByteImage &im, FILE *file) {
  int res;

  // Write magic number.
  if (im.Depth() == 1) {
    fprintf(file, "P5\n");
  } else if(im.Depth() == 3) {
    fprintf(file, "P6\n");
  } else {
    return 0;
  }

  // Write sizes.
  fprintf(file, "%d %d %d\n", im.Width(), im.Height(), 255);

  // Write pixels.
  res = fwrite(im.Data(), 1, im.Size(), file);
  if (res != im.Size()) {
    return 0;
  }
  return 1;
}

}  // namespace libmv
