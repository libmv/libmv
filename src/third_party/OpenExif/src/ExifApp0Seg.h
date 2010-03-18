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
 * FirstName LastName <email address>
 * FirstName LastName <email address>
 */ 

#ifndef _EXIF_APP0_SEG_H
#define	_EXIF_APP0_SEG_H


#include <vector>
#include <string>


#include "ExifConf.h"
#include "ExifTypeDefs.h"
#include "ExifAppSegment.h"

class ExifIO ;
class ExifApp0Seg ;

/*!
  \author  George Sotak <george.sotak@kodak.com>
  \date   Sun Jan 20 15:57:47 2002
  
  \brief  Interface to Application Segment 0

  Application Segment 0 holds the following information:
  - Version - the most significant byte is used for major revisions,
              the least significant is used for minor revisions.
  - Units - Units X and Y densities. Default: 0x00
  - Xdensity - Horizontal pixel density. Default: 0x0001
  - Ydensity - Vertical pixel density. Default: 0x0001
  - Xthumbnail - thumbnail horizontal pixel count. Default: 0x00
  - Ythumbnail - thumbnail horizontal pixel count. Default: 0x00
*/

class ExifApp0Seg : public ExifAppSegment
{
    public:

        //! Constructor for direct instantiation
        ExifApp0Seg( uint16 _marker, const char* _ident ) 
            : ExifAppSegment( _marker, _ident), mVersion(0), mUnitsId(0), 
              mXdensity(1), mYdensity(1), mXthumbnail(0), mYthumbnail(0)
            { }

        //! Destructor
        virtual ~ExifApp0Seg();
        
        //! Factory method - return a copy of the ExifApp0Seg.
        inline virtual ExifAppSegment* clone() const
        {
            return new ExifApp0Seg(*this);
        }       

        /*! Init the instance after instantiation from the factory
        
            \param exifio  handle to the ExifIO class, provides access to the file
            \param _length  length of the app segment in the file
            \param _exifHeaderOffset  offset due to the Exif header
        */
        virtual ExifStatus init( ExifIO* exifio, uint16 _length,
            exifoff_t _exifHeaderOffset ) ;

        /*! Copy an app segment 0
        
            \param theSrc    source app segment 0
        */
        void copy( const ExifApp0Seg* theSrc ) ;

        //! \name get / set accessors for the version field
        //@{
        const uint16& version() const { return mVersion ; }
        uint16& version() { return mVersion ; }
        //@}

        //! @name get / set accessors for the density units field
        //@{
        const uint8& unitsId() const { return mUnitsId ; }
        uint8& unitsId() { return mUnitsId ; }
        //@}

        //! @name get / set accessors for the x,y density fields
        //@{
        const uint16& xDensity() const { return mXdensity ; }
        uint16& xDensity() { return mXdensity ; }

        const uint16& yDensity() const { return mYdensity ; }
        uint16& yDensity() { return mYdensity ; }
        //@}

        //! @name get / set accessors for the x,y thumbnail fields
        //@{
        const uint8& xThumbnail() const { return mXthumbnail ; }
        uint8& xThumbnail() { return mXthumbnail ; }

        const uint8& yThumbnail() const { return mYthumbnail ; }
        uint8& yThumbnail() { return mYthumbnail ; }
        //@}

        virtual ExifStatus write( ExifIO* exifio ) ;
    
    protected:
        //! read the application segment from the file
        ExifStatus read( ExifIO* exifio ) ;
        
        // Saves current ExifIO flags and set them to mMyExifioFlags
        virtual void setMyExifioFlags( ExifIO* exifio ) ;

    private:
        //! Version - the most significant byte is used for major revisions,
        //! the least significant is used for minor revisions.
        uint16 mVersion;
        
        //! Units - Units X and Y densities. Default: 0x00
        uint8 mUnitsId;
        
        //! Xdensity - Horizontal pixel density. Default: 0x0001
        uint16 mXdensity;
        
        //! Ydensity - Vertical pixel density. Default: 0x0001
        uint16 mYdensity;
        
        //! Xthumbnail - thumbnail horizontal pixel count. Default: 0x00
        uint8 mXthumbnail;
        
        //! Ythumbnail - thumbnail horizontal pixel count. Default: 0x00
        uint8 mYthumbnail;
        
};

#endif // _EXIF_APP0_SEG_H

