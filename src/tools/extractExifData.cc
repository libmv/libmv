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

 /*
 * Program Usage:
 *    extractExifData <filename>
 *       where <filename> is the name of the Exif file to read the Exif Tags
 *       from.
 */

#include "libmv/tools/ExifReader.h"
#include <iostream>
using namespace std;
using namespace libmv::tools;


// The main entry point.
int main(int argc, char *argv[])
{
  if (argc == 2)
  {
    (void)argv;
    cout << endl << ExifReader::doesHaveExifInfo( argv[1] ) << endl;
    ExifReader exifReader;
    if (exifReader.readExifInfo(argv[1]))
    {
      exifReader.displayInfo();
    }

    float t;
    ExifToolHelper::GetFocalmm(argv[1], &t);
    cout << endl << "focal : " << t;

    ExifToolHelper::GetFocalPlaneUnit(argv[1], &t);
    cout << endl << "FocalPlaneUnit : " << t;

    ExifToolHelper::GetFocalPlaneXRes(argv[1], &t);
    cout << endl << "GetFocalPlaneXRes : " << t;

    ExifToolHelper::GetFocalEquiv35mm(argv[1], &t);
    cout << endl << "GetFocalEquiv35mm : " << t;

    unsigned int width, height;
    ExifToolHelper::GetImageWidthAndHeight(argv[1], &width, &height);
    cout << endl << "GetImageWidth : " << width;
    cout << endl << "GetImageHeight : " << height;

    cout << endl;


  }
  return 1;
}
