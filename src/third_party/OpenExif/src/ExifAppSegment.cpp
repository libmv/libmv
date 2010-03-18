/*
 * Copyright (c) 2000-2009, Eastman Kodak Company
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification,are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, 
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the 
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Eastman Kodak Company nor the names of its 
 *       contributors may be used to endorse or promote products derived from 
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 * 
 *
 * Creation Date: 07/14/2001
 *
 * Original Author: 
 * George Sotak george.sotak@kodak.com 
 *
 * Contributor(s): 
 * Ricardo Rosario ricardo.rosario@kodak.com
 * Sam Fryer samuel.fryer@kodak.com
 */ 

#if (defined _MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "ExifAppSegment.h"
#include "ExifTags.h" // For  FILLORDER_MSB2LSB

// Include the other App Segments if needed...
#include "ExifRawAppSeg.h"
#include "ExifTiffAppSeg.h"
#include "ExifApp0Seg.h"


// The hex digits.  
// (this is less "elegant" than the iostream we had before, but it's 
//  a little more portable to embedded OSes!)
static const char hexchar[] = 
        {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void ExifAppSegment::createAppId( uint16 marker, const std::string& ident,
                                 std::string& id )
{
    // First, convert the marker to a hex string
    id = hexchar[marker>>12];
    id += hexchar[(marker>>8)&0x000F];
    id += hexchar[(marker>>4)&0x000F];
    id += hexchar[marker&0x000F];

    // then append the identifier
    id += ident ;

    return ;
}

// This function used to use a factory that was very complex, broke lots of 
// compilers and was not embedded friendly.  These new methods will allow 
// Openexif to work on many more platforms and hopefully be a little
// faster ... all at the expense of the ease of adding new types of 
// app segments.  Every app segment maintainer will now have to modify 
// this function.  (However, we haven't had any new app segment types 
// in many years, so I'm not too worried about this!)
ExifAppSegment* ExifAppSegment::create( const std::string& ident, 
                                        bool defaultToRaw)
{
    ExifAppSegment * appSeg = NULL;
    if (ident.length() >= 4)
    {
        // First, parse the ident string for id and marker
        uint16 marker = 0;
        for (unsigned int i = 0; i < 4 ; i++)
        {
            char t = ident[i];
            marker <<=4;
            if ((t >= '0') && (t <= '9'))
                marker += (t-'0');
            else if ((t >= 'A') && (t <= 'F'))
                marker += (t-'A')+10;
        }
        std::string id = ident.substr(4);

        // next, find if we can create one of the known app segs
        appSeg = ExifAppSegment::create(marker,id,defaultToRaw);
        if ((defaultToRaw) && (appSeg == NULL))
            appSeg = new ExifRawAppSeg(marker, id.c_str()) ;
    }
    return appSeg;
}
ExifAppSegment* ExifAppSegment::create( uint16 marker, const std::string& id, 
                                        bool defaultToRaw)
{
    ExifAppSegment * appSeg = NULL;

    if (marker == 0xFFE0)
        appSeg = new ExifApp0Seg(marker,id.c_str());
    else if ((marker == 0xFFE1) && (id=="Exif"))
        appSeg = new ExifTiffAppSeg(marker,id.c_str());
    else if ((marker == 0xFFE3) && 
            ((id == "EXIF")||(id=="Exif")||(id=="META")||(id=="Meta")))
        appSeg = new ExifTiffAppSeg(marker,id.c_str());
    else if (defaultToRaw)
        appSeg = new ExifRawAppSeg(marker, id.c_str()) ;
    return appSeg;
}


ExifStatus ExifAppSegment::init( ExifIO* exifio, uint16 _length,
                                exifoff_t _exifHeaderOffset )
{
    (void)exifio;
    mLength = _length ;
    mExifOffset = _exifHeaderOffset ;

    return EXIF_OK ;
}

// Saves current ExifIO flags and set them to mMyExifioFlags
void ExifAppSegment::setMyExifioFlags( ExifIO* exifio )
{
    mSavedExifioFlags = exifio->flags() ;
    mSavedEndianState = exifio->bigEndian() ;
    exifio->flags() = mMyExifioFlags ;
    exifio->bigEndian( mMyEndianState ) ;
}

// Restores saved ExifIO flags
void ExifAppSegment::restoreExifioFlags( ExifIO* exifio )
{
    mMyExifioFlags = exifio->flags() ;
    mMyEndianState = exifio->bigEndian() ;
    exifio->flags() = mSavedExifioFlags ;
    exifio->bigEndian( mSavedEndianState ) ;
}


