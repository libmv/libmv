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
 * Portions of the Original Code are
 * Copyright (c) 1988-1996 Sam Leffler
 * Copyright (c) 1991-1996 Silicon Graphics, Inc.
 *
 * Creation Date: 07/14/2001
 *
 * Original Author: 
 * George Sotak george.sotak@kodak.com 
 *
 * Contributor(s): 
 * Ricardo Rosario ricardo.rosario@kodak.com
 */ 

#include "ExifConf.h"
#include "ExifErrorCodes.h"
#include "ExifIO.h"
#include "ExifOpenFile.h"

ExifOpenFile::~ExifOpenFile()
{
    close() ;
}

ExifStatus ExifOpenFile::close( void )
{
    if ( mExifio )
    {
        if( mIOwnExifIO )
        {
            delete mExifio ;
        }
        mExifio = NULL ;
    }
    
    return EXIF_OK;
}


ExifStatus ExifOpenFile::open( const char* filename, const char* cmode)
{
    ExifStatus status = EXIF_FILE_OPEN_ERROR ;

    mIOwnExifIO = true ;
    mExifio = ExifIO::open( status, filename, cmode ) ;
  
    if( mExifio )
    {
        mFileName = filename ;
        status = initAfterOpen( cmode ) ;
    }

    return status ;
}

ExifStatus ExifOpenFile::open(fhandle_t fileHandle, const char* name,
                              const char* cmode)
{
    ExifStatus status ;

    mExifio = new ExifIO( status, fileHandle, name, cmode ) ;
    if( status != EXIF_OK )
    {
        delete mExifio ;
        mExifio = NULL ;
    }
    else
    {
        mFileName = name ;
        mIOwnExifIO = true ;
        status = initAfterOpen( cmode ) ;
    }

    return status ;
}

// Open a file from memory
ExifStatus ExifOpenFile::open(void* imgBuf, tsize_t bufSize, const char* cmode,
    tsize_t bytesWritten)
{
    ExifStatus status = EXIF_OK;
    
    mExifio = new ExifIO(status, imgBuf, bufSize, cmode, bytesWritten) ;
    if( status != EXIF_OK )
    {
        delete mExifio ;
        mExifio = NULL ;
    }
    else
    {
        mIOwnExifIO = true ;
        status = initAfterOpen(cmode) ;
    }

    return status ;
}
