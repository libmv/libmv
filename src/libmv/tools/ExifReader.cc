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

#include "libmv/tools/ExifReader.h"
#include <iomanip>
#include <iostream>
using namespace std;

namespace libmv {
namespace tools {

/// Get the focal in mm from Exif data. Return false, and -1 if not found.
bool ExifToolHelper::GetFocalmm(const string & sFilename, float * focalmm)
{
  *focalmm = -1.0f;
  ExifImageFile inImageFile_;
  if (inImageFile_.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    ExifStatus errRtn;
    ExifTagEntry* entryTag = inImageFile_.getGenericTag( EXIFTAG_FOCALLENGTH,
     EXIF_APP1_EXIFIFD, errRtn);
    if (entryTag!=NULL && entryTag->getType() == EXIF_RATIONAL)
    {
      *focalmm = (dynamic_cast < ExifTagEntryT<float> *> (entryTag))->getValue();
      return true;
    }
  }
  return false;
}

/// Indicate metric information.  Return false, and -1 if not found.
bool ExifToolHelper::GetFocalPlaneUnit(const string & sFilename, float * focalPlaceUnit)
{
  *focalPlaceUnit = -1.0f;
  ExifImageFile inImageFile_;
  if (inImageFile_.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    ExifStatus errRtn;
    ExifTagEntry* entryTag = inImageFile_.getGenericTag( EXIFTAG_FOCALPLANERESOLUTIONUNIT,
     EXIF_APP1_EXIFIFD, errRtn);
    if (entryTag!=NULL && entryTag->getType() == EXIF_SHORT)
    {
      *focalPlaceUnit = (dynamic_cast < ExifTagEntryT<unsigned short> *> (entryTag))->getValue();
      return true;
    }
    else
    {
      cout << endl << "BAD INFO" << endl;
    }
  }
  return false;
}

/// Get FocalPlaneXResolution from Exif data. Return false, and -1 if not found.
bool ExifToolHelper::GetFocalPlaneXRes(const string & sFilename, float * focalPlaceXRes)
{
  *focalPlaceXRes = -1.0f;
  ExifImageFile inImageFile_;
  if (inImageFile_.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    ExifStatus errRtn;
    ExifTagEntry* entryTag = inImageFile_.getGenericTag( EXIFTAG_FOCALPLANEXRESOLUTION,
     EXIF_APP1_EXIFIFD, errRtn);
    if (entryTag!=NULL && entryTag->getType() == EXIF_RATIONAL)
    {
      *focalPlaceXRes = (dynamic_cast < ExifTagEntryT<float> *> (entryTag))->getValue();
      return true;
    }
    else
    {
      cout << endl << "BAD INFO" << endl;
    }
  }
  return false;
}

/// Get the 35mm focal equivalent from Exif data. Return false, and -1 if not found.
/// Experimental.
bool ExifToolHelper::GetFocalEquiv35mm(const string & sFilename, float * focalEquiv35mm)
{
  *focalEquiv35mm = -1.0f;
  ExifImageFile inImageFile_;
  if (inImageFile_.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    ExifStatus errRtn;
    ExifTagEntry* entryTag =
      inImageFile_.getGenericTag( EXIFTAG_FOCALLENGTH_35MM, EXIF_APP1_EXIFIFD, errRtn);
    if (entryTag!=NULL && entryTag->getType() == EXIF_SHORT)
    {
      *focalEquiv35mm = (dynamic_cast < ExifTagEntryT<unsigned short> *> (entryTag))->getValue();
      return true;
    }
    else
    {
      cout << endl << "BAD INFO" << endl;
    }
  }
  return false;
}

/// Get Image Width and Height from Jpeg Header.
bool ExifToolHelper::GetImageWidthAndHeight(const string & sFilename,
              unsigned int * width, unsigned int * height)
{
  *width = -1;
  *height = -1;
  ExifImageFile inImageFile_;
  if (inImageFile_.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    ExifImageInfo info;
    inImageFile_.getImageInfo(info);
    *width = info.width;
    *height = info.height;
    return true;
  }
  return false;
}

/*// Compute the CCD width, in millimeters.
if (FocalplaneXRes != 0){
    // Note: With some cameras, its not possible to compute this correctly because
    // they don't adjust the indicated focal plane resolution units when using less
    // than maximum resolution, so the CCDWidth value comes out too small.  Nothing
    // that Jhad can do about it - its a camera problem.
    ImageInfo.CCDWidth = (float)(ExifImageWidth * FocalplaneUnits / FocalplaneXRes);

    if (ImageInfo.FocalLength && ImageInfo.FocalLength35mmEquiv == 0){
        // Compute 35 mm equivalent focal length based on sensor geometry if we haven't
        // already got it explicitly from a tag.
        ImageInfo.FocalLength35mmEquiv = (int)(ImageInfo.FocalLength/ImageInfo.CCDWidth*36 + 0.5);
    }
}*/


ExifReader::~ExifReader() {
  if( inImageFile_.close() != EXIF_OK )
  {
      cout << "Error: Could not close" << endl;
  }
}

bool ExifReader::doesHaveExifInfo(const string & sFilename) {
  bool bRet = false;
  // Create instance of ExifImageFile
  ExifImageFile inImageFile;

  // Open the file in read-only mode and verify that it succeeds
  if (inImageFile.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    if( inImageFile.close() != EXIF_OK )
    {
        cout << "Error: Could not close" << sFilename << endl;
    }
    else bRet = true;
  }
  return bRet;
}

bool ExifReader::readExifInfo(const string & sFilename) {
  bool bRet = false;

  // Open the file in read-only mode and verify that it succeeds
  if (inImageFile_.open( sFilename.c_str(), "r" ) == EXIF_OK)
  {
    //-- Read EXIF data
    {
      // Get all the AppSeg 1 - "Exif" tags and output them
      inImageFile_.getAllTags( 0xFFE1, "Exif", app1PathsTags_ );

      // Get all the AppSeg 3 - "Meta" tags and output them
      inImageFile_.getAllTags( 0xFFE3, "Meta", app3PathsTags_ );

      //Now, recognition of any other app segments:
      // Get a vector with all the application segments in the file
      appSegs_ = inImageFile_.getAllAppSegs();

      // Now, lets output any COM marker data
      comList_ = ( ExifComMarkerList * )inImageFile_.getComData();

      // And finally, let's output the SOF info
      inImageFile_.getImageInfo(info_);

    }
    bRet = true;
  }
  return bRet;
}

// readExifInfo must be called before
void ExifReader::displayInfo()  {
  // Display "Exif tags"
  if (app1PathsTags_.begin() != app1PathsTags_.end())
    cout << "\nApp1 - \"Exif\" entries:" << endl;
  for (ExifPathsTagsIter crntPathsTags = app1PathsTags_.begin();
       crntPathsTags != app1PathsTags_.end();
       crntPathsTags++ )
  {
    ExifIFDPathIter crntPath = (*crntPathsTags).first.begin();
    ExifIFDPathIter endPath = (*crntPathsTags).first.end();
    while( crntPath != endPath )
    {
      cout << "IFD: " << (*crntPath).first
           << "  Idx: " << (*crntPath).second << endl;
      crntPath++;
    }

    ExifTagsIter crnt = (*crntPathsTags).second.begin();
    ExifTagsIter end = (*crntPathsTags).second.end();

    cout << "Tag#\tType\tCount\tValue" << endl;
    while( crnt != end )
    {
      ExifTagEntry* tag = *(crnt);
      tag->print();
      cout << endl;
      crnt++;
    }
  }

  // Display Meta tags
  if (app3PathsTags_.begin() != app3PathsTags_.end())
    cout << "\nApp3 - \"Meta\" entries:" << endl;
  for (ExifPathsTagsIter crntPathsTags = app3PathsTags_.begin();
       crntPathsTags != app3PathsTags_.end();
       crntPathsTags++ )
  {
    ExifIFDPathIter crntPath = (*crntPathsTags).first.begin();
    ExifIFDPathIter endPath = (*crntPathsTags).first.end();
    while( crntPath != endPath )
    {
      cout << "IFD: " << (*crntPath).first
           << "  Idx: " << (*crntPath).second << endl;
      crntPath++;
    }

    ExifTagsIter crnt = (*crntPathsTags).second.begin();
    ExifTagsIter end = (*crntPathsTags).second.end();

    cout << "Tag#\tType\tCount\tValue" << endl;
    while( crnt != end )
    {
      ExifTagEntry* tag = *(crnt);
      tag->print();
      cout << endl;
      crnt++;
    }
  }

  // Application Segments
  int numOfAppSegs = appSegs_.size();

  cout << "\n\nNumber of Application Segments "
       << ": " << numOfAppSegs << endl
       << "Marker\tLength\tIdent" << endl;

  // Loop through the application segments outputting their marker,
  // length and identifier.
  for ( int i = 0; i < numOfAppSegs; ++i )
  {
      cout << appSegs_[i]->getAppSegmentMarker() << "\t"
           << appSegs_[i]->getLength() << "\t"
           << appSegs_[i]->getAppIdent() << endl;
  }

  // Now, lets output any COM marker data
  cout << endl << endl;
  for(unsigned int i = 0; i < comList_->size(); i++)
  {
      tsize_t dataSize;
      int j;
      ExifComMarker * marker = comList_->getComMarker(i);
      const uint8 * comData = marker->getData(dataSize);
      cout << "COM Marker #" << i+1 << " Data" << endl;
      for (j = 0; j < dataSize; ++j)
          cout << setbase(16) << (unsigned short)comData[j] << " ";
      cout << setbase(10); //reset display base
      for (j = 0; j < dataSize; ++j)
          cout << comData[j];
      cout << endl << endl;
  }

  // And finally, let's output the SOF info
  cout << "Image Information:\n";
  cout << "\twidth:\t\t" << info_.width << endl
       << "\theight:\t\t" << info_.height << endl
       << "\tchannels:\t" << info_.numChannels << endl
       << "\tbit depth:\t" << info_.precision << endl;

  cout << endl
       << "Summation" << endl
       << " Exif data occurences   : " << app1PathsTags_.size() << endl
       << " Meta data occurences   : " << app3PathsTags_.size() << endl
       << " App segment occurences : " << appSegs_.size() << endl
       << " Com marker  occurences : " << comList_->size() << endl;
}

} // namespace tools
} // namespace libmv
