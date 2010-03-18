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
#include "ExifIO.h"
#include "ExifFileIO.h"
#ifdef WIN32
#ifdef INTERNET_REQUIRED
#include "ExifInternetIO.h"
#endif // INTERNET_REQUIRED
#endif // WIN32
#include "ExifMemoryIO.h"

#if (defined _MSC_VER) && (defined WIN32) 
#include <io.h>
#include <sys/stat.h>
#endif

ExifIO::ExifIO( fhandle_t theFileHandle, const std::string& filename,
    const char* cmode, exif_uint32 imode )
  : mFlags(0), mFileName(filename), mMode(imode), mCMode(cmode),
    mFileHandle( theFileHandle )
{
    mHandleIO = new ExifFileIO( *this, mFileHandle ) ;
}

ExifIO::ExifIO( ExifStatus& status, fhandle_t theFileHandle,
    const std::string& filename, const char* cmode )
  : mFlags(0), mFileName(filename), mCMode(cmode),
    mFileHandle( theFileHandle )
{
    mHandleIO = NULL;
    if( (status = convertMode( cmode, mMode )) != EXIF_OK )
        return ;

    mHandleIO = new ExifFileIO( *this, mFileHandle ) ;
}

ExifIO::ExifIO(ExifStatus& status, void* imgBuf, tsize_t bufSize,
      const char* cmode, tsize_t bytesWritten )
    : mFlags(0), mFileName(""), mCMode(cmode), mFileHandle(NULL)
{
    mHandleIO = NULL;
    if( (status = convertMode( cmode, mMode )) != EXIF_OK )
        return ;

    // If -1 or no value was passed in for bytes written, then we need to
    // initialize bytesWritten to 0, for writeonly mode, or to bufSize
    // for readwrite and readonly
    if ( bytesWritten == -1 )
    {
        if ( mMode & O_WRONLY )
            bytesWritten = 0 ;
        else
            bytesWritten = bufSize ;
    }

    mHandleIO = new ExifMemoryIO(*this, imgBuf, bufSize, bytesWritten);

    flags() |= EXIF_MAPPED ;
}

#ifdef WIN32
#ifdef INTERNET_REQUIRED
ExifIO::ExifIO( ExifStatus& status, HINTERNET hInet, const string& filename,
      const char* cmode )
    : mFlags(0), mFileName(filename), mCMode(cmode), mFileHandle(NULL)
{
    mHandleIO = NULL;
    if( (status = convertMode( cmode, mMode )) != EXIF_OK )
        return ;

    mHandleIO = new ExifInternetIO(*this, hInet);
}
#endif // INTERNET_REQUIRED
#endif // WIN32
 
/*
 * Open a EXIF file for read/writing.
 */
ExifIO* ExifIO::open(ExifStatus& status, const char* filename,
    const char* cmode)
{
    ExifIO* returnValue = NULL ;
    exif_uint32 imode  ;
    fhandle_t fh = NULL;
    if( (status = convertMode( cmode, imode )) != EXIF_OK )
        returnValue = NULL ;
    else
    {
        if( imode & O_CREAT)
        {
            fh = fopen( filename,"wb" ) ;
        }
        else if ( imode & O_RDWR)
        {
            fh = fopen( filename,"rb+" ) ;
        }
        else if ( imode == O_RDONLY)
        {
            fh = fopen( filename,"rb" ) ;
        }

        if ( fh == NULL )
        {
            status = EXIF_FILE_OPEN_ERROR ;
            returnValue = NULL ;
        }
    }

    if ( status == EXIF_OK )
        returnValue = new ExifIO( (fhandle_t)fh, filename, cmode, imode ) ;

    return returnValue ;
}
 
void ExifIO::close()
{
    if (mHandleIO != NULL)
    {
        mHandleIO->close();
        delete mHandleIO;
        mHandleIO = NULL;
    }
}

exifoff_t ExifIO::size()
{
    exifoff_t returnValue;
    if (mHandleIO)
        returnValue = mHandleIO->size();
    else
        returnValue = 0;
    
    return returnValue;
}


FILE* ExifIO::getFilePtr( void )
{
    this->seek(0, SEEK_SET);
    return mFileHandle ;
}

// Convert a character mode to a numeric (integer) mode
ExifStatus ExifIO::convertMode( const char* cmode, exif_uint32 &imode )
{
    ExifStatus returnValue = EXIF_OK;
    if (cmode)
    {
        switch (*cmode) 
        { 
            case 'r':
                if( *(cmode+1) == '+' )
                    imode = O_RDWR ;
                else
                    imode = O_RDONLY ;
                break;
            
            case 'w':
                if( *(cmode+1) == '+' )
                    returnValue = EXIF_FILE_OPEN_MODE_NOT_SUPPORTED ;
                    // Do not support "w+" mode for now
                    //imode = O_RDWR | O_CREAT | O_TRUNC ;
                else
                    imode = O_WRONLY | O_CREAT | O_TRUNC ;
                break ;
            
            // Doesn't make sense to open an Exif file in append mode
            //case 'a':
                //if( *(cmode+1) == '+' )
                //    imode = O_RDWR | O_CREAT  ;
                //else
                //    imode = O_WRONLY | O_CREAT ;
                //break;

            default:
                returnValue = EXIF_FILE_OPEN_MODE_NOT_SUPPORTED ;
        }
    }
    return returnValue ;
}

void ExifIO::swabShort(uint16* wp)
{
    if ( doSwab() )
    {
        uint8* cp = (uint8*) wp;
        uint8 t;
        // Swap the the two bytes
        t = cp[1]; cp[1] = cp[0]; cp[0] = t;
    }
}

void ExifIO::swabLong(exif_uint32* lp)
{
    if ( doSwab() )
    {
        uint8* cp = (uint8*) lp;
        uint8 t;
        // Swap the first byte with the last byte
        t = cp[3]; cp[3] = cp[0]; cp[0] = t;
        // Swap the second byte with the third byte
        t = cp[2]; cp[2] = cp[1]; cp[1] = t;
    }
}

void ExifIO::swabArrayOfShort(uint16* wp, exif_uint32 n)
{
    if( doSwab() )
    {
        uint8* cp;
        uint8 t;
        /* XXX unroll loop some */
        while (n-- > 0) 
        {
            cp = (uint8*) wp;
            // Swap the two bytes
            t = cp[1]; cp[1] = cp[0]; cp[0] = t;
            wp++;
        }
    }
}

void ExifIO::swabArrayOfLong(exif_uint32* lp, exif_uint32 n)
{
    if ( doSwab() )
    {
        uint8 *cp;
        uint8 t;
        /* XXX unroll loop some */
        while (n-- > 0) 
        {
            cp = (unsigned char *)lp;
            // Swap the first byte with the last byte
            t = cp[3]; cp[3] = cp[0]; cp[0] = t;
            // Swap the second byte with the third byte
            t = cp[2]; cp[2] = cp[1]; cp[1] = t;
            lp++;
        }
    }
}

void ExifIO::swabDouble(double *dp)
{
    if( doSwab() )
    {
        exif_uint32* lp = (exif_uint32*) dp;
        exif_uint32 t;
        swabArrayOfLong(lp, 2);
        t = lp[0]; lp[0] = lp[1]; lp[1] = t;
    }
}

void ExifIO::swabArrayOfDouble(double* dp, exif_uint32 n)
{
    if ( doSwab() )
    {
        exif_uint32* lp = (exif_uint32*) dp;
        exif_uint32 t;
        swabArrayOfLong(lp, n + n);
        while (n-- > 0) 
        {
            t = lp[0]; lp[0] = lp[1]; lp[1] = t;
            lp += 2;
        }
    }
}
