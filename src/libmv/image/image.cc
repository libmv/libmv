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

#include "libmv/image/image.h"

namespace libmv {

int ReadPgm(const char *filename, ByteImage *im) {
  FILE *file = fopen(filename,"r");
  if (!file) {
    return 0;
  }
  int res = ReadPgmStream(file, im);
  fclose(file);
  return res;
}

int ReadPgmStream(FILE *file, ByteImage *im) {
  int magicnumber, width, height, maxval;
  int res;

  // Check magic number.
  res = fscanf(file, "P%d", &magicnumber); 
  if (res != 1 || magicnumber != 5) {
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
  im->Resize(height,width,1);
  res = fread(im->Data(), 1, im->Size(), file);
  if (res != im->Size()) {
    return 0;
  }
  return 1;
}

int WritePgm(const ByteImage &im, const char *filename) {
  FILE *file = fopen(filename,"w");
  if (!file) {
    return 0;
  }
  int res = WritePgmStream(im, file);
  fclose(file);
  return res;
}

int WritePgmStream(const ByteImage &im, FILE *file) {
  int res;

  // Write magic number.
  fprintf(file, "P5\n"); 

  // Write sizes.
  fprintf(file, "%d %d %d\n", im.Width(), im.Height(), 255);

  // Write pixels.
  res = fwrite(im.Data(), 1, im.Size(), file);
  if (res != im.Size()) {
    return 0;
  }
  return 1;
}

void ConvertFloatImageToByteImage(const FloatImage &float_image,
                                  ByteImage *byte_image) {
  byte_image->ResizeLike(float_image);
  float minval=HUGE_VAL, maxval=-HUGE_VAL;
  for (int i = 0; i < float_image.Height(); ++i) {
    for (int j = 0; j < float_image.Width(); ++j) {
      minval = std::min(minval, float_image(i,j));
      maxval = std::max(maxval, float_image(i,j));
    }
  }
  for (int i = 0; i < float_image.Height(); ++i) {
    for (int j = 0; j < float_image.Width(); ++j) {
      float unscaled = (float_image(i,j) - minval) / (maxval - minval);
      (*byte_image)(i,j) = (unsigned char)(255 * unscaled);
    }
  }
}

}  // namespace libmv
