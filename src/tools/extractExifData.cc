// Set licence here

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
