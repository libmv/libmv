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

int ReadPnm(const char *filename, FloatImage *image) {
  ByteImage byte_image;
  int res = ReadPnm(filename, &byte_image);
  if (!res) {
    return res;
  }
  ByteArrayToScaledFloatArray(byte_image, image);
  return res;
}

// TODO(maclean): Add support for PNG and JPG via libjpeg and libpng. Will
// require some cmake hackery and #define madness though.  Consider examples at
// http://trac.astrometry.net/browser/trunk/src/astrometry/util/cairoutils.c
// TODO(maclean): Consider making this templated to support loading into both
// Eigen and Array3D's.
// Comment handling as per the description provided at http://netpbm.sourceforge.net/doc/pgm.html
// and http://netpbm.sourceforge.net/doc/pbm.html
int ReadPnmStream(FILE *file, ByteImage *im) {

  const int NUM_VALUES = 3 ;
  const int INT_BUFFER_SIZE = 256 ;

  int magicnumber, depth ;
  char intBuffer[INT_BUFFER_SIZE];
  int values[NUM_VALUES], valuesIndex = 0, intIndex = 0, inToken = 0 ;
  int res;
  // values[0] = width, values[1] = height, values[2] = maxValue

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

  // the following loop parses the PNM header one character at a time, looking for
  // the int tokens width, height and maxValues (in that order), and discarding
  // all comment (everything from '#' to '\n' inclusive), where comments *may occur
  // inside tokens*. Each token must be terminate with a whitespace character, and only
  // one whitespace char is eaten after the third int token is parsed.
  while (valuesIndex < NUM_VALUES)
  {
    char nextChar ;
    res = fread(&nextChar,1,1,file);
    if (res == 0) return 0 ; // read failed, EOF?

    if (isspace(nextChar))
    {
      if (inToken) // we were reading a token, so this white space delimits it
      {
        inToken = 0 ;
        intBuffer[intIndex] = 0 ; // NULL-terminate the string
        values[valuesIndex++] = atoi(intBuffer);
        intIndex = 0 ; // reset for next int token
//        if (valuesIndex == 3 && values[2] > 65535) return 0 ; // use this line if image class aloows 2-byte grey-scale
        if (valuesIndex == 3 && values[2] > 255) return 0 ; // to conform with current image class
      }
    }
    else if (isdigit(nextChar))
    {
      inToken = 1 ; // in case it's not already set
      intBuffer[intIndex++] = nextChar ;
      if (intIndex == INT_BUFFER_SIZE) return 0 ; // tokens should never be this long
    }
    else if (nextChar == '#')
    {
      do // eat all characters from input stream until newline
      {
        res = fread(&nextChar,1,1,file);
      } while (res == 1 && nextChar != '\n') ;
      if (res == 0) return 0 ; // read failed, EOF?
    }
    else // encountered a non-whitepace, non-digit outside a comment - bail out
    {
      return 0 ;
    }
  }

//  if (values[2] > 255 && magicnumber == 5) depth = 2 ;

  // Read pixels.
  im->Resize(values[1],values[0], depth);
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

// TODO(maclean) Look into using StringPiece here (do a codesearch) to allow
// passing both strings and const char *'s.
int WritePnm(const FloatImage &image, const char *filename) {
  ByteImage byte_image;
  FloatArrayToScaledByteArray(image, &byte_image);
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
