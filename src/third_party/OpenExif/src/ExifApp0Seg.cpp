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
 */ 

#if (defined _MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "ExifIO.h"
#include "ExifApp0Seg.h"


ExifApp0Seg::~ExifApp0Seg()
{

}


ExifStatus ExifApp0Seg::init( ExifIO* exifio, uint16 _length, exifoff_t _exifHeaderOffset  )
{
    ExifAppSegment::init( exifio, _length, _exifHeaderOffset ) ;
    
    /*
     *   Default is to return data MSB2LSB
     */
    setMyExifioFlags( exifio ) ;
    
    ExifStatus status = read( exifio );
     
    restoreExifioFlags( exifio ) ;
    
    return  status;
}

// Saves current ExifIO flags and set them to mMyExifioFlags
void ExifApp0Seg::setMyExifioFlags( ExifIO* exifio )
{
    mSavedExifioFlags = exifio->flags() ;
    mSavedEndianState = exifio->bigEndian() ;

    // This is a major hack and should be re-examined.
    if ( !HOST_BIGENDIAN )
        exifio->flags() |= EXIF_SWAB;
}


void ExifApp0Seg::copy( const ExifApp0Seg* theSrc )
{
    *this = *theSrc ;

    return ;
}



ExifStatus ExifApp0Seg::write( ExifIO* exifio )
{
    exifio->swabShort( &mMarker ) ;
    exifio->write( &mMarker, sizeof(uint16) ) ;

    uint16 length = 16 ;
    exifio->swabShort( &length ) ;
    exifio->write( &length, sizeof(uint16) ) ;

    exifio->write( &mIdent[0], 5*sizeof(char) ) ;

    exifio->swabShort( &mVersion ) ;
    if( exifio->write( &mVersion, sizeof(uint16) ) != sizeof(uint16))
        return EXIF_FILE_WRITE_ERROR ;
    if( exifio->write( &mUnitsId, sizeof(uint8) ) != sizeof(uint8))
        return EXIF_FILE_WRITE_ERROR ;

    exifio->swabShort( &mXdensity ) ;
    if( exifio->write( &mXdensity, sizeof(uint16) ) != sizeof(uint16))
        return EXIF_FILE_WRITE_ERROR ;

    exifio->swabShort( &mYdensity ) ;
    if( exifio->write( &mYdensity, sizeof(uint16) ) != sizeof(uint16))
        return EXIF_FILE_WRITE_ERROR ;

    if( exifio->write( &mXthumbnail, sizeof(uint8) ) != sizeof(uint8))
        return EXIF_FILE_WRITE_ERROR ;

    if( exifio->write( &mYthumbnail, sizeof(uint8) ) != sizeof(uint8))
        return EXIF_FILE_WRITE_ERROR ;

    return EXIF_OK ;
}

ExifStatus ExifApp0Seg::read( ExifIO* exifio )
{
    if( exifio->read( &mVersion, sizeof(uint16) ) != sizeof(uint16))
        return EXIF_FILE_READ_ERROR ;
    exifio->swabShort( &mVersion ) ;

    if( exifio->read( &mUnitsId, sizeof(uint8) ) != sizeof(uint8))
        return EXIF_FILE_READ_ERROR ;

    if( exifio->read( &mXdensity, sizeof(uint16) ) != sizeof(uint16))
        return EXIF_FILE_READ_ERROR ;
    exifio->swabShort( &mXdensity ) ;

    if( exifio->read( &mYdensity, sizeof(uint16) ) != sizeof(uint16))
        return EXIF_FILE_READ_ERROR ;
    exifio->swabShort( &mYdensity ) ;
   
    if( exifio->read( &mXthumbnail, sizeof(uint8) ) != sizeof(uint8))
        return EXIF_FILE_READ_ERROR ;

    if( exifio->read( &mYthumbnail, sizeof(uint8) ) != sizeof(uint8))
        return EXIF_FILE_READ_ERROR ;

    return EXIF_OK ;
}
