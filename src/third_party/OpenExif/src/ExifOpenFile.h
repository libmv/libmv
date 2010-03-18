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


#include <string>

#include "ExifConf.h"
#include "ExifErrorCodes.h"
#include "ExifTypeDefs.h"

class ExifIO ;

//! Base class providing file open / close facilities
/*!
  \author  George Sotak <george.sotak@kodak.com>
  \date   Sat Jan 19 18:51:24 2002

  For details on usage see ExifImageFile or ExifJpegImage
*/

class EXIF_DECL ExifOpenFile
{
    public:
 
        //! The destructor
        virtual ~ExifOpenFile();
        
        //! Open an image file.
        /*! 
          \param name - Name of the image file.
          \param cmode - Open mode.

          Possible Values for cmode (combinations also allowed):
          "r"    open for reading, file must exist else an error
          "r+"   open for read / write, file must exist else an error
          "w"    open for writing only, create file if does not exist,
                 truncate if it does.

          "l"    use little-endian byte order for creating a file.
          "b"    use big-endian byte order for creating a file
          "L"    read/write information using LSB2MSB bit order
          "B"    read/write information using MSB2LSB bit order
          "H"    read/write information using host bit order
        */
        ExifStatus open(const char* filename, const char* cmode);
        
        //! Open an image file with the given file handle. 
        /*! 
          \param fileHandle File handle.
          \param name Name of the image file. It's not actually used to open
                      the file, just for the information purpose.
          \param cmode Open mode
          \sa ExifOpenFile::open(const char* name, const char* cmode) for
              details on cmode.
         */
        ExifStatus open(fhandle_t fileHandle, const char* name,
            const char* cmode);

        //* Open an in-memory image file.
        /*!
          \param imgBuf - buffer containin the image file
          \param bufSize - Size of image buffer.
          \param cmode - Open mode.
          \param bytesWritten - The actual size of the file within the buffer.
                                If no value is passed, it will be assumed to be
                                equal to bufSize for "r" and "r+" modes and
                                equal to bufSize for "w" mode.
          \sa ExifOpenFile::open(const char* name, const char* cmode) for
              details on cmode.
         */
        ExifStatus open(void* imgBuf, tsize_t bufSize, const char* cmode,
            tsize_t bytesWritten = -1);

#ifdef WIN32
#ifdef INTERNET_REQUIRED
        //! Open an image file with the supplied internet file handle.
        /*!
          \param hInternet - Internet file handle.
          \param name - File name, just for information purpose.
        */
        ExifStatus open(HINTERNET hInet, const char* name);
#endif // INTERNET_REQUIRED
#endif // WIN32
    
        //! Close the image file.
        virtual ExifStatus close( void ) ;

    protected:
        ExifIO*    mExifio;
        bool       mIOwnExifIO ;
        std::string     mFileName;

        ExifOpenFile( void ) : mExifio(NULL), mIOwnExifIO(false) {}
        ExifOpenFile( ExifIO* exifIO ) : mExifio(exifIO), mIOwnExifIO(false) {}

        virtual ExifStatus initAfterOpen( const char* cmode ) = 0 ;
};

