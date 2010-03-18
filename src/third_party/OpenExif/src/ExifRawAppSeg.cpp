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
 * Creation Date: 02/19/2003
 *
 * Original Author: 
 * Sam Fryer <samuel.fryer@kodak.com>
 *
 * Contributor(s): 
 * Ricardo Rosario <ricardo.rosario@kodak.com>
 */ 


#if (defined _MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "ExifIO.h"
#include "ExifRawAppSeg.h"

// This is registered as a default so that it will automatically
// be chosen for any app segment that we don't have another registered 
// class for.
// EK_PROVIDE(ExifRawAppSeg, EXIF_APP_SEG_TYPE_INIT("default", "RAW", 0x0000, ExifRawAppSeg))


// Copy Constructor
ExifRawAppSeg::ExifRawAppSeg( const ExifRawAppSeg& theSrc )
  : ExifAppSegment(theSrc), mRawData(NULL)
{
    setRawData(theSrc.mRawData, theSrc.mLength) ;
}

// Routine to initialize the class and read the data in from a file
ExifStatus ExifRawAppSeg::init( ExifIO* exifio, uint16 _length,
    exifoff_t _exifHeaderOffset  )
{
    ExifAppSegment::init( exifio, _length, _exifHeaderOffset ) ;
    
    // the exifio's current position should be the AppSeg Marker. Seek
    // to right after that...
    exifio->seek(2, SEEK_CUR) ;
    return read( exifio ) ;
}

// routine to copy the class
void ExifRawAppSeg::copy( const ExifRawAppSeg* theSrc )
{
    if (mRawData) delete[] mRawData;
    *this = *theSrc ;
    mRawData = new char[mLength];
    memcpy(mRawData,theSrc->mRawData, mLength);

    return ;
}

// routine to set the data to new data
void ExifRawAppSeg::setRawData( const char* theData,
    const unsigned int theLength)
{
    if (mRawData) delete[] mRawData;
    mRawData = new char[theLength];
    if (theData != NULL)
    {
        memcpy(mRawData,theData,theLength);
        mLength = theLength;
    }
    else 
        mLength = 0;
    
    return ;
}

// routine to write the data out via the exifio write method.
ExifStatus ExifRawAppSeg::write( ExifIO* exifio )
{
    exifio->swabShort( &mMarker ) ;
    exifio->write( &mMarker, sizeof(uint16) ) ;

    exifio->write( mRawData, mLength );

    return EXIF_OK ;
}

// routine to read the data in via the exifio read method
ExifStatus ExifRawAppSeg::read( ExifIO* exifio )
{
    ExifStatus status = EXIF_OK;
    mRawData = new char[mLength];
    if ( mRawData )
    {
        if( exifio->read( mRawData, mLength ) != mLength)
            status = EXIF_FILE_READ_ERROR ;
    }
    else
        status = EXIF_ERROR ;

    return status ;
}

