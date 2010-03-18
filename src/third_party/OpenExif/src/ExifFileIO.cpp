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
#include "ExifFileIO.h"

#if (defined _MSC_VER) && (defined WIN32)
#include <sys/stat.h>
#include <io.h>
#endif

#if defined(unix) || defined(__unix)
#include <sys/stat.h>
#elif defined(macintosh)
#include <stat.h>
#endif

// Read from the file
tsize_t ExifFileIO::read( tdata_t buf, tsize_t size)
{
    return fread(buf, size, 1, mFileHandle)*size ;
}

// Write to the file
tsize_t ExifFileIO::write( tdata_t buf, tsize_t size )
{
    return fwrite(buf, size, 1, mFileHandle)*size ;
}

// Seek to the given offset
exifoff_t ExifFileIO::seek( exifoff_t off, int whence)
{
    exifoff_t returnValue;
    if ( fseek(mFileHandle, off, whence) == 0 )
        returnValue = ftell(mFileHandle) ;
    else
        returnValue = -1 ;

    return returnValue ;
}

// Compute and return the size of the file
exifoff_t ExifFileIO::size()
{
    exifoff_t curPos;
    exifoff_t tempSize;
    curPos = this->seek(0, SEEK_CUR);
    tempSize = this->seek(0, SEEK_END);
    this->seek(curPos, SEEK_SET);
    return tempSize;
}

// Close the file
ExifStatus ExifFileIO::close()
{
    ExifStatus returnValue = EXIF_FILE_CLOSE_ERROR ;
    if ( mFileHandle )
    {
        if( fclose( mFileHandle ) == 0 )
            returnValue = EXIF_OK ;
    }
    return returnValue ;
}
