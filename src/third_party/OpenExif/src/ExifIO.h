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

#ifndef _EXIF_IO_H_
#define _EXIF_IO_H_

#include <cstdio>
#include <string>

#include "ExifErrorCodes.h"
#include "ExifTypeDefs.h"

typedef	FILE* fhandle_t;	/* client data handle */


class ExifIO;



/*!
  \author George Sotak <george.sotak@kodak.com>
  \date   Sun Jan 20 16:44:00 2002
  
  \brief  Base class for FileIO implementations
  
  OpenExif provides support for accessing Exif Image File data
  through either tradition file io (read / write from / to a file
  descriptior), and access to a real memory buffer holding the 
  image file data.
  
*/

class ExifHandleIO
{
    public:

        //! Constructor
        ExifHandleIO( ExifIO& theParent)
            : mParent( theParent ) {}

        //! Destructor
        virtual ~ExifHandleIO( ) {}

        //! read size bytes from the file into the buf
        virtual tsize_t read( tdata_t buf, tsize_t size ) = 0;

        //! write size bytes from the buf into the file
        virtual tsize_t write( const tdata_t buf, const tsize_t size ) = 0;

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
        virtual exifoff_t seek( exifoff_t off, int whence ) = 0;

        //! returns the size of the file in bytes
        virtual exifoff_t size() = 0;

        //! close the file
        virtual ExifStatus close() = 0;

        virtual uint8* base( void ) const
            { return NULL ; }
        virtual tsize_t memSize() const
            { return 0; }

    protected:
        ExifIO& mParent;

};

/*!
  \author George Sotak <george.sotak@kodak.com>
  \date   Sun Jan 20 16:44:00 2002
  
  \brief  Interface class for FileIO implementations
  
  
*/

class EXIF_DECL ExifIO
{
    public:

        //! Constructor to use when dealing with a file handle
        /*!
            \param status - Return of the error status
            \param theFileHandle - file handle
            \param filename - name of the file
            \param cmode - open mode
         */
        ExifIO(ExifStatus& status, fhandle_t theFileHandle,
            const std::string& filename, const char* cmode );

        //! Constructor to use when dealing with a memory buffer
        /*!
            \param status - Return of the error status
            \param imgBuf - buffer containing the image file
            \param bufSize - size of the buffer
            \param cmode - open mode
            \param bytesWritten - The actual size of the file within the buffer.
                                  If no value is passed, it will be assumed to
                                  be equal to bufSize for "r" and "r+" modes and
                                  equal to bufSize for "w" mode.
         */
        ExifIO(ExifStatus& status, void* imgBuf, tsize_t bufSize,
            const char* cmode, tsize_t bytesWritten = -1 );

#ifdef WIN32
#ifdef INTERNET_REQUIRED
        //! Access to the file over the network
        ExifIO( ExifStatus& status, HINTERNET hInet, const string& filename,
            const char* cmode) ;
#endif // INTERNET_REQUIRED
#endif // WIN32

        //! Destructor
        ~ExifIO( void ) { close(); }

        //! Retrieve a handle to the IO-Handler
        ExifHandleIO& ioHandler( void ) const
            { return *mHandleIO ; }
        
        //! Open a file and return a handle to the ExifIO instance
        /*!
          \param status  return of the error status
          \param filename  string holding the path to the file and its name
          \param mode    the file mode with which to open the file

          \sa ExifOpenFile::open() for details on mode
        */
        static ExifIO* open( ExifStatus& status, const char* filename,
            const char* mode );
        

        // A lot of classes use this method, so leave
        // it here even if memory mapping is disabled.
        // It returns the correct result in either case.
        //! \name Memory Mapped File IO Management
        //@{
        //! returns true if file is currently memory mapped, otherwise false
        bool isMapped() const
            { return (mFlags & EXIF_MAPPED)?true:false ; }

        //@}

        //! set / get accessor for ExifIO control flags
        /*!
          See ExifDefs.h for flag definitions
        */
        exif_uint32& flags( void ) 
            { return mFlags; }

        //! Retrieve the integer version of the file mode
        const exif_uint32& mode( void ) const
            { return mMode; }

        //! Retrieve the filename that is currently being accessed
        const std::string& name() const
            { return mFileName; }

        fhandle_t& getFileHandle( void )
            {return mFileHandle; }
        FILE* getFilePtr( void ) ;

        //! see ExifHandleIO::read()
        tsize_t read( tdata_t buf, tsize_t size )
            { return mHandleIO->read( buf, size ); }

        //! see ExifHandleIO::write()
        tsize_t write( const tdata_t buf, const tsize_t size )
            { return mHandleIO->write( buf, size ); }

        //! see ExifHandleIO::seek()
        exifoff_t seek( exifoff_t off, int whence )
            { return mHandleIO->seek( off, whence ); }

        //! close the file
        void close( void );

        //! return the size of the file in bytes
        exifoff_t size( void );
        
        //! \name get / set accessors for endianess
        //@{
        bool bigEndian( void ) const
            { return mBigEndian; }
        void bigEndian( bool state )
            { mBigEndian = state;  return; }
        //@}
        
        //! returns true if swabbing of bytes needs to be done, otherwise false
        bool doSwab( void ) const
            { return (mFlags & EXIF_SWAB)?true:false; }
        
        //! \name Helper methods for byte swabbing
        //@{
        void swabShort( uint16* wp );
        void swabLong( exif_uint32* lp );
        void swabArrayOfShort( uint16* wp, exif_uint32 n );
        void swabArrayOfLong( exif_uint32* lp, exif_uint32 n );
        void swabDouble( double *dp );
        void swabArrayOfDouble( double* dp, exif_uint32 n );
        //@}
        
    private:
        ExifHandleIO* mHandleIO;
    
        exif_uint32  mFlags;
        std::string  mFileName;
        exif_uint32  mMode;
        std::string  mCMode ;
        bool    mBigEndian;

        fhandle_t  mFileHandle;

        ExifIO( fhandle_t theFileHandle, const std::string& filename,
            const char* cmode, exif_uint32 imode );
        static ExifStatus convertMode( const char* cmode, exif_uint32 &imode );
};

#endif // _EXIF_IO_H_
