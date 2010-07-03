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

#include <cstring>

#include <iostream>

extern "C" {
#include "jpeglib.h"
#include "png.h"
}

#include "libmv/image/image_io.h"
#include "libmv/logging/logging.h"

namespace libmv {

static bool CmpFormatExt(const char *a, const char *b) {
  int len_a = strlen(a);
  int len_b = strlen(b);
  if (len_a != len_b) return false;
  for (int i = 0; i < len_a; ++i)
    if (tolower(a[i]) != tolower(b[i]))
      return false;
  return true;
}

Format GetFormat(const char *c) {
  const char *p = strrchr (c, '.');

  if (p == NULL)
    return Unknown;

  if (CmpFormatExt(p, ".png")) return Png;
  if (CmpFormatExt(p, ".ppm")) return Pnm;
  if (CmpFormatExt(p, ".pgm")) return Pnm;
  if (CmpFormatExt(p, ".pbm")) return Pnm;
  if (CmpFormatExt(p, ".pnm")) return Pnm;
  if (CmpFormatExt(p, ".jpg")) return Jpg;
  if (CmpFormatExt(p, ".jpeg")) return Jpg;
  
  LOG(ERROR) << "Error: Couldn't open " << c << " Unknown file format";
  return Unknown;
}

int ReadImage(const char *filename, ByteImage *im){
  Format f = GetFormat(filename);

  switch (f) {
    case Pnm:
      return ReadPnm(filename, im);
    case Png:
      return ReadPng(filename, im);
    case Jpg:
      return ReadJpg(filename, im);
    default:
      return 0;
  };
}

int ReadImage(const char *filename, FloatImage *im){
  Format f = GetFormat(filename);

  switch (f) {
    case Pnm:
      return ReadPnm(filename, im);
    case Png:
      return ReadPng(filename, im);
    case Jpg:
      return ReadJpg(filename, im);
    default:
      return 0;
  };
}

int WriteImage(const ByteImage &im, const char *filename){
  Format f = GetFormat(filename);

  switch (f) {
    case Pnm:
      return WritePnm(im, filename);
    case Png:
      return WritePng(im, filename);
    case Jpg:
      return WriteJpg(im, filename);
    default:
      return 0;
  };
}

int WriteImage(const FloatImage &im, const char *filename){
  Format f = GetFormat(filename);

  switch (f) {
    case Pnm:
      return WritePnm(im, filename);
    case Png:
      return WritePng(im, filename);
    case Jpg:
      return WriteJpg(im, filename);
    default:
      return 0;
  };
}

int ReadJpg(const char *filename, ByteImage *im) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    LOG(ERROR) << "Error: Couldn't open " << filename << " fopen returned 0";
    return 0;
  }
  int res = ReadJpgStream(file, im);
  fclose(file);
  return res;
}

int ReadJpg(const char *filename, FloatImage *image) {
  ByteImage byte_image;
  int res = ReadJpg(filename, &byte_image);
  if (!res) {
    return res;
  }
  ByteArrayToScaledFloatArray(byte_image, image);
  return res;
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

METHODDEF(void)
jpeg_error (j_common_ptr cinfo)
{
  my_error_mgr *myerr = (my_error_mgr*) cinfo->err;
  (*cinfo->err->output_message) (cinfo);
  longjmp(myerr->setjmp_buffer, 1);
}

int ReadJpgStream(FILE *file, ByteImage *im) {
  jpeg_decompress_struct cinfo;
  struct my_error_mgr jerr;
  JSAMPARRAY buffer;
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = &jpeg_error;

  if (setjmp(jerr.setjmp_buffer)) {
    jpeg_destroy_decompress(&cinfo);
    return 0;
  }

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  int row_stride = cinfo.output_width * cinfo.output_components;

  buffer = (*cinfo.mem->alloc_sarray)
    ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

  im->Resize(cinfo.output_height, cinfo.output_width, cinfo.output_components);

  unsigned char *ptr = im->Data();

  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);

    int i;
    for (i = 0; i < row_stride; ++i) {
      *ptr = (*buffer)[i];
      ptr++;
    }
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  return 1;
}

int WriteJpg(const ByteImage &im, const char *filename, int quality) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    LOG(ERROR) << "Error: Couldn't open " << filename << " fopen returned 0";
    return 0;
  }
  int res = WriteJpgStream(im, file, quality);
  fclose(file);
  return res;
}

int WriteJpg(const FloatImage &image, const char *filename, int quality) {
  ByteImage byte_image;
  FloatArrayToScaledByteArray(image, &byte_image);
  return WriteJpg(byte_image, filename, quality);
}

int WriteJpgStream(const ByteImage &im, FILE *file, int quality) {
  if (quality < 0 || quality > 100)
    LOG(ERROR) << "Error: The quality parameter should be between 0 and 100";

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);
  jpeg_stdio_dest(&cinfo, file);

  cinfo.image_width = im.Width();
  cinfo.image_height = im.Height();
  cinfo.input_components = im.Depth();

  if (cinfo.input_components==3) {
    cinfo.in_color_space = JCS_RGB;
  } else if (cinfo.input_components==1) {
    cinfo.in_color_space = JCS_GRAYSCALE;
  } else {
    LOG(ERROR) << "Error: Unsupported number of channels in file";
    jpeg_destroy_compress(&cinfo);
    return 0;
  }

  jpeg_set_defaults(&cinfo);
  jpeg_set_quality(&cinfo, quality, TRUE);
  jpeg_start_compress(&cinfo, TRUE);

  const unsigned char *ptr = im.Data();
  int row_bytes = cinfo.image_width*cinfo.input_components;

  JSAMPLE *row = new JSAMPLE[row_bytes];

  while (cinfo.next_scanline < cinfo.image_height) {
    int i;
    for (i = 0; i < row_bytes; ++i)
    	row[i] = ptr[i];
    jpeg_write_scanlines(&cinfo, &row, 1);
    ptr += row_bytes;
  }

  delete [] row;

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  return 1;
}

int ReadPng(const char *filename, ByteImage *im) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    LOG(ERROR) << "Error: Couldn't open " << filename << " fopen returned 0";
    return 0;
  }
  int res = ReadPngStream(file, im);
  fclose(file);
  return res;
}

int ReadPng(const char *filename, FloatImage *image) {
  ByteImage byte_image;
  int res = ReadPng(filename, &byte_image);
  if (!res) {
    return res;
  }
  ByteArrayToScaledFloatArray(byte_image, image);
  return res;
}

// The writing and reading functions using libpng are based on http://zarb.org/~gc/html/libpng.html
int ReadPngStream(FILE *file, ByteImage *im) {
  png_byte header[8];

  if (fread(header, 1, 8, file) != 8) {
    LOG(FATAL) << "fread failed.";
  }
  if (png_sig_cmp(header, 0, 8))
    return 0;

  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                               NULL, NULL, NULL);

  if (!png_ptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    return 0;

  if (setjmp(png_jmpbuf(png_ptr)))
    return 0;

  png_init_io(png_ptr, file);
  png_set_sig_bytes(png_ptr, 8);

  png_read_info(png_ptr, info_ptr);

  im->Resize(info_ptr->height, info_ptr->width, info_ptr->channels);

  png_read_update_info(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr)))
    return 0;

  png_bytep *row_pointers =
       (png_bytep*)malloc(sizeof(png_bytep) * info_ptr->channels *
       im->Height());

  unsigned char *ptr = (unsigned char *)im->Data();
  int y;
  for (y = 0; y < im->Height(); ++y)
    row_pointers[y] = (png_byte*) ptr + info_ptr->rowbytes*y;

  png_read_image(png_ptr, row_pointers);

  free(row_pointers);

  return 1;
}

int WritePng(const ByteImage &im, const char *filename) {
  FILE *file = fopen(filename, "wb");
  if (!file) {
    LOG(ERROR) << "Error: Couldn't open " << filename << " fopen returned 0";
    return 0;
  }
  int res = WritePngStream(im, file);
  fclose(file);
  return res;
}

int WritePng(const FloatImage &image, const char *filename) {
  ByteImage byte_image;
  FloatArrayToScaledByteArray(image, &byte_image);
  return WritePng(byte_image, filename);
}

int WritePngStream(const ByteImage &im, FILE *file) {
  png_structp png_ptr =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr)
    return 0;

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
    return 0;

  if (setjmp(png_jmpbuf(png_ptr)))
    return 0;

  png_init_io(png_ptr, file);

  if (setjmp(png_jmpbuf(png_ptr)))
    return 0;

  // Colour types are defined at png.h:841+.
  char colour;
  if (im.Depth() == 3)
    colour = PNG_COLOR_TYPE_RGB;
  else if (im.Depth() == 1)
    colour = PNG_COLOR_TYPE_GRAY;
  else
    return 0;

  png_set_IHDR(png_ptr, info_ptr, im.Width(), im.Height(),
      8, colour, PNG_INTERLACE_NONE,
      PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

  png_write_info(png_ptr, info_ptr);

  if (setjmp(png_jmpbuf(png_ptr)))
    return 0;

  png_bytep *row_pointers =
      (png_bytep*) malloc(sizeof(png_bytep) *im.Depth()* im.Height());

  unsigned char *ptr = (unsigned char *)im.Data();
  int y;
  for (y = 0; y < im.Height(); ++y)
    row_pointers[y] = (png_byte*) ptr + info_ptr->rowbytes*y;

  png_write_image(png_ptr, row_pointers);

  if (setjmp(png_jmpbuf(png_ptr)))
    return 0;

  free(row_pointers);

  png_write_end(png_ptr, NULL);

  return 1;
}

int ReadPnm(const char *filename, ByteImage *im) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    LOG(ERROR) << "Error: Couldn't open " << filename << " fopen returned 0";
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
// TODO(maclean): Consider making this templated to support loading into both
// Eigen and Array3D's.
// Comment handling as per the description provided at
//   http://netpbm.sourceforge.net/doc/pgm.html
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

  // the following loop parses the PNM header one character at a time, looking
  // for the int tokens width, height and maxValues (in that order), and
  // discarding all comment (everything from '#' to '\n' inclusive), where
  // comments *may occur inside tokens*. Each token must be terminate with a
  // whitespace character, and only one whitespace char is eaten after the
  // third int token is parsed.
  while (valuesIndex < NUM_VALUES) {
    char nextChar ;
    res = fread(&nextChar,1,1,file);
    if (res == 0) return 0; // read failed, EOF?

    if (isspace(nextChar)) {
      if (inToken) { // we were reading a token, so this white space delimits it
        inToken = 0;
        intBuffer[intIndex] = 0 ; // NULL-terminate the string
        values[valuesIndex++] = atoi(intBuffer);
        intIndex = 0; // reset for next int token
        // use this line if image class aloows 2-byte grey-scale
//        if (valuesIndex == 3 && values[2] > 65535) return 0 ;
        // to conform with current image class
        if (valuesIndex == 3 && values[2] > 255) return 0;
      }
    }
    else if (isdigit(nextChar)) {
      inToken = 1 ; // in case it's not already set
      intBuffer[intIndex++] = nextChar ;
      if (intIndex == INT_BUFFER_SIZE) // tokens should never be this long
        return 0;
    }
    else if (nextChar == '#') {
      do { // eat all characters from input stream until newline
        res = fread(&nextChar,1,1,file);
      } while (res == 1 && nextChar != '\n');
      if (res == 0) return 0; // read failed, EOF?
    }
    else {
      // Encountered a non-whitepace, non-digit outside a comment - bail out.
      return 0;
    }
  }

  // use this line if image class aloows 2-byte grey-scale
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
  FILE *file = fopen(filename, "wb");
  if (!file) {
    LOG(ERROR) << "Error: Couldn't open " << filename << " fopen returned 0";
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
  } else if (im.Depth() == 3) {
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
