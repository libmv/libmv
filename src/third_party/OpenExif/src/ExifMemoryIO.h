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


#ifndef _EXIF_MEMORY_IO_H_
#define _EXIF_MEMORY_IO_H_

#include "ExifIO.h"

/*!
  \author  George Sotak <george.sotak@kodak.com>
  \date   Sun Jan 20 18:22:48 2002
  
  \brief  Provides file-type access to a memory buffer.
  
*/

class ExifMemoryIO : public ExifHandleIO
{
    public:
        //! Constructor
        ExifMemoryIO( ExifIO& theParent, void* imgBuf, tsize_t bufSize,
            tsize_t bytesWritten )
          : ExifHandleIO(theParent), mImageBuffer((unsigned char*)imgBuf), 
            mBufferSize(bufSize), mBytesWritten(bytesWritten),
            mCurBufOffset(0) {}

        //! Destructor
        ~ExifMemoryIO( ) {}

        //! see ExifHandleIO::read()
        virtual tsize_t read( tdata_t buf, tsize_t size ) ;

        //! see ExifHandleIO::write()
        virtual tsize_t write( const tdata_t buf, const tsize_t size ) ;

        //! see ExifHandleIO::seek()
        virtual exifoff_t seek( exifoff_t off, int whence ) ;

        //! see ExifHandleIO::size()
        virtual exifoff_t size() ;

        //! close it up
        virtual ExifStatus close();

        virtual inline uint8* base() const { return mImageBuffer; }
        virtual inline tsize_t memSize() const { return mBufferSize; }

    protected:
        uint8* mImageBuffer;
        tsize_t	mBufferSize;
        tsize_t	mBytesWritten;
        exifoff_t	mCurBufOffset;

        void memCopy( void* d, const tdata_t s, tsize_t c ) ;


};


#endif // _EXIF_MEMORY_IO_H_

