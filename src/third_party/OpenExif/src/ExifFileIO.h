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


#ifndef _EXIF_FILE_IO_H_
#define _EXIF_FILE_IO_H_

#include "ExifErrorCodes.h"
#include "ExifIO.h"

/*!
  \author  George Sotak <george.sotak@kodak.com>
  \date   Sun Jan 20 16:26:06 2002
  
  \brief  Implementation of the File IO.

  This class is never used directly, all access is through
  the ExifIO class.  
  
*/

class ExifFileIO : public ExifHandleIO
{   
    public:
        //! Constructor
        /*!
          \param theParent   handle to the ExifIO parent instance
          \param fhandle     handle to the file itself
        */
        ExifFileIO( ExifIO& theParent, fhandle_t& fhandle )
            : ExifHandleIO(theParent), mFileHandle(fhandle) {}

        //! Destructor
        ~ExifFileIO( ) {}
        
        //! read size bytes from the file into the buf
        virtual tsize_t read( tdata_t buf, tsize_t size ) ;

        //! write size bytes from the buf into the file
        virtual tsize_t write( const tdata_t buf, const tsize_t size ) ;

        //! seek to the specified byte offset in the file
        /*!
          \param off    byte off to which to seek
          \param whence indication of where offset is relative to

          Values for whence:
             SEEK_SET - offset is relative to start of file
             SEEK_CUR - offset is relative to the current file pointer position
             SEEK_END - offset is relative to end of file
          \code 
          seek( 0, SEEK_SET) - seek to the beginning of the file
          seek( 0, SEEK_END) - seek to the end of the file
          \endcode
        */
        virtual exifoff_t seek( exifoff_t off, int whence ) ;

        //! returns the size of the file in bytes
        virtual exifoff_t size() ;

        //! close the file
        virtual ExifStatus close();

    protected:
        fhandle_t& mFileHandle ;
        
};

#endif // _EXIF_FILE_IO_H_
