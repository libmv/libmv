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

#include "third_party/OpenExif/src/ExifImageFile.h"
#include <string>
using namespace std;

#ifndef LIBMV_TOOLS_EXIF_READER_H_
#define LIBMV_TOOLS_EXIF_READER_H_

namespace libmv {
namespace tools {

/// Help class to read JPEG EXIF specific value (focal, image resolution)
class ExifToolHelper
{
  public:
  /// Get the focal in mm from Exif data. Return false, and -1 if not found.
  static bool GetFocalmm(const string & sFilename, float * focalmm);

  /// Indicate metric information.  Return false, and -1 if not found.
  static bool GetFocalPlaneUnit(const string & sFilename,
                                float * focalPlaceUnit);

  /// Get FocalPlaneXResolution from Exif data. Return false, and -1 if not found.
  static bool GetFocalPlaneXRes(const string & sFilename,
                                float * focalPlaceXRes);

  /// Get the 35mm focal equivalent from Exif data.
  ///  Return false, and -1 if not found.
  ///  Experimental.
  static bool GetFocalEquiv35mm(const string & sFilename,
                                float * focalEquiv35mm);

  /// Get Image Width and Height from Jpeg Header.
  static bool GetImageWidthAndHeight(const string & sFilename,
                                     unsigned int * width,
                                     unsigned int * height);
};

class ExifReader
{
  public:

  virtual ~ExifReader();

  /// Return true if the JPEG file have embed EXIF/IPTC information.
  static bool doesHaveExifInfo(const string & sFilename);

  /// Parse the file and store EXIF/IPTC/TAGS data readed.
  bool readExifInfo(const string & sFilename);

  /// Display values readed by readExifInfo() (send it to std::cout)
  /// /!\ readExifInfo must be called before
  void displayInfo();

  /// Getters
  const ExifImageInfo & getExifImageInfoRef() const { return  info_;}
  /// Getters
  const ExifPathsTags & getExifTagRef()       const { return  app1PathsTags_;}
  /// Getters
  const ExifPathsTags & getMetaTagRef()       const { return  app3PathsTags_;}
  /// Getters
  const vector<ExifAppSegment*> & getAppSegmentRef()  const { return  appSegs_;}
  /// Getters
  const ExifComMarkerList * getComMarkerRef()         const { return  comList_;}

  private:

  ExifImageFile inImageFile_;       // Exif High Level class managment
  //-- Exif info
  ExifPathsTags app1PathsTags_;     // App Seg 1 "Exif tags"
  ExifPathsTags app3PathsTags_;     // App Seg 3 "Meta tags"
  vector<ExifAppSegment*> appSegs_; // App segments.
  ExifComMarkerList * comList_;     // COM marker data
  ExifImageInfo info_;              // SOF info
};

} // namespace tools
} // namespace libmv

#endif // LIBMV_TOOLS_EXIF_READER_H_
