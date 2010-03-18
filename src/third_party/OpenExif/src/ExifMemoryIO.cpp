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


#include "ExifConf.h"
#include "ExifMemoryIO.h"

// Seek to the specified offset
exifoff_t ExifMemoryIO::seek( exifoff_t off, int whence )
{
    switch(whence)
    {
        case SEEK_CUR:
            mCurBufOffset += off;
            break;
        case SEEK_END:
            mCurBufOffset = mBytesWritten + off;
            break;
	    case SEEK_SET:
        default:
            mCurBufOffset = off;
            break;
    }

    // Make sure mCurBufOffset is within the boundaries
    if ( mCurBufOffset < 0 )
        mCurBufOffset = 0 ;
    else if ( mCurBufOffset > mBytesWritten )
        mCurBufOffset = mBytesWritten ;

    return mCurBufOffset ;
}

// Read from the memory buffer
tsize_t ExifMemoryIO::read( tdata_t buf, tsize_t size )
{
    tsize_t returnValue = -1;
    if ( (mParent.mode() == O_RDONLY) || (mParent.mode() == O_RDWR) )
    {
        if (mCurBufOffset + size <= (exifoff_t)mBytesWritten)
        {
            memcpy( (char*)buf, mImageBuffer + mCurBufOffset, size ) ;
            mCurBufOffset += size ;
            returnValue = size ;
        }
    }

    return returnValue ;
}

// Write to the memory buffer
tsize_t ExifMemoryIO::write( tdata_t buf, tsize_t size )
{
    tsize_t returnValue = -1 ;
    if ( (mParent.mode() == O_RDWR) || (mParent.mode() == O_CREAT) )
    {
        if (mCurBufOffset + size <= (exifoff_t)mBufferSize)
        {
            memcpy( mImageBuffer + mCurBufOffset, (char*)buf, size ) ;
            mCurBufOffset += size ;
            if (mCurBufOffset > (exifoff_t)mBytesWritten)
                mBytesWritten = mCurBufOffset ;
            returnValue = size ;
        }
    }

    return returnValue ;
}

// Compute and return the size of the memory file/buffer
exifoff_t ExifMemoryIO::size()
{
    return mBytesWritten ;
}

// Close the memory buffer (does nothing)
ExifStatus ExifMemoryIO::close()
{
    return EXIF_OK ;
}

