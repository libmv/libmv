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
 * Sam Fryer samuel.fryer@kodak.com
 */ 

#ifndef _EXIF_APP_SEGMENT_H_
#define _EXIF_APP_SEGMENT_H_

#if (defined _MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "ExifConf.h"
#include "ExifTypeDefs.h"
#include "ExifIO.h"

#define EXIFAPPSEGMENT_IDENT_SIZE 128

// The idea was to use strcpy_s and strncpy_s for buffer safety reasons,
// but that introduced a whole new set of problems (bugs in compilers(VC8)).
// So here's an implementation that should work on all platforms
inline void openexif_strcpy_s(char *cpy, int max_size, const char *org)
{
	if (cpy && org){
		int i = 0;
		for (i=0;(i<max_size)&&(*org != 0);cpy++,i++,org++) *cpy = *org;
		*cpy = 0;
	}
}
#define OPENEXIF_STRCPY_S openexif_strcpy_s

/*!
  \author  George Sotak <george.sotak@kodak.com>
  \date   Sun Jan 20 09:18:08 2002
  
  \brief  Base class for all Application Segment implementations
  
  
*/

class EXIF_DECL ExifAppSegment
{
    public:
    
        //! Destructor
        virtual ~ExifAppSegment()
        {  }

        /*! Creates the Application Identification string
        
            \param marker    the app segment marker (e.g., 0xFFE1, 0xFFE2, etc.)
            \param ident     the app segment identifier (e.g., "EXIF")
            \param id        the output identification string
         */
        static void createAppId(uint16 marker,const std::string& ident,
                                std::string& id ) ;

        /*! Factory method for the instantiation of App Segment implementations
            
            \param ident   the app segment identifier in toolkit expected format
            \param defaultToRaw  return a raw app segment if no other is found.
            \return        a base class pointer to the instantiated
                         implementation if previously registered with factory,
                         otherwise NULL

            Example:
            \code
            string id ;
            ExifAppSegment::createAppId( 0xFFE1, "Exif", id ) ;
            ExifAppSegement* appSeg = ExifAppSegment::create( id ) ;
            \endcode
         */
        static ExifAppSegment* create( const std::string& ident, 
                                       bool defaultToRaw = true );
        static ExifAppSegment* create( uint16 marker, const std::string& id, 
                                       bool defaultToRaw = true );
            
        //inline static int addEntry(string key, ExifAppSegment * appSeg) 
        //{ msFactory.addEntry(key,appSeg); return 0;};

        virtual ExifAppSegment* clone( void ) const = 0 ;
        
        // Is this a TIFF (IFD) Based AppSegment?
        virtual bool isTiff() 
        { return false; }
        
        // Is this a RAW app segment?
        virtual bool isRaw()
        { return false; }

        virtual ExifStatus init( ExifIO* exifio, uint16 _length,
            exifoff_t _exifHeaderOffset ) ;
        
        /*! Get the length of this app Segment in bytes.
        
            \return length of the app segment.
        */
        uint16  getLength( void ) const
        { return mLength; }

        /*! Get the app Segment Label.
        
            \return The app segment label.
        */
        const char*  getAppIdent( void ) const
        { return mIdent; }

        /*! Get the app Segment marker.
        
            \return the app segment marker.
        */
        uint16  getAppSegmentMarker( void ) const
        { return mMarker; }


        //! Write out the App Segment to the file
        virtual ExifStatus write( ExifIO* exifio ) = 0 ;
    
    protected:
        //static ExifAppSegFactoryT msFactory ;
        exifoff_t mExifOffset ;
        exifoff_t mOffsetToLength ;
            
        uint16 mMarker;
        uint16 mLength;

        exif_uint32 mSavedExifioFlags ;
        bool mSavedEndianState ;
        
        exif_uint32 mMyExifioFlags ;
        bool mMyEndianState ;
        

        // We currently support an identifier up to 128 bytes.
        // Is this enough?? Or do we need to be more flexible??
        char mIdent[EXIFAPPSEGMENT_IDENT_SIZE];

        ExifAppSegment( uint16 _appMarker, uint16 _length, const char* _ident,
                        exifoff_t _exifOffset ) 
          : mExifOffset(_exifOffset), mOffsetToLength(0),
            mMarker(_appMarker), mLength(_length),
            mSavedExifioFlags(0), mSavedEndianState(0), mMyExifioFlags(0),
            mMyEndianState(0)
		{ 
            initIdent(); 
			OPENEXIF_STRCPY_S(mIdent, EXIFAPPSEGMENT_IDENT_SIZE, _ident) ; 
        }

        ExifAppSegment( uint16 _appMarker, const char* _ident ) 
          : mExifOffset(0),  mOffsetToLength(0), mMarker(_appMarker),
            mLength(0),mSavedExifioFlags(0), mSavedEndianState(0), mMyExifioFlags(0),
            mMyEndianState(0)
		{ 
            initIdent(); 
			OPENEXIF_STRCPY_S(mIdent, EXIFAPPSEGMENT_IDENT_SIZE, _ident) ; 
        }

        // Copy Constructor
        ExifAppSegment( const ExifAppSegment& theSrc )
          : mExifOffset(theSrc.mExifOffset),
            mOffsetToLength(theSrc.mOffsetToLength),
            mMarker(theSrc.mMarker), mLength(theSrc.mLength),
            mSavedExifioFlags(theSrc.mSavedExifioFlags),
            mSavedEndianState(theSrc.mSavedEndianState),
            mMyExifioFlags(theSrc.mMyExifioFlags),
            mMyEndianState(theSrc.mMyEndianState)
		{ 
			const char * tmpOrig= theSrc.mIdent;
			char * tmpCopy = mIdent;
			for(int i=0; 
				i < EXIFAPPSEGMENT_IDENT_SIZE;
				i++,tmpCopy++,tmpOrig++)
				*tmpCopy = *tmpOrig;
        }

        void initIdent( void )
        {
            memset( static_cast<void*>(&mIdent[0]), 0, EXIFAPPSEGMENT_IDENT_SIZE) ;
        }
        
        // Saves current ExifIO flags and set them to mMyExifioFlags
        virtual void setMyExifioFlags( ExifIO* exifio ) ;

        // Restores saved ExifIO flags
        virtual void restoreExifioFlags( ExifIO* exifio ) ;


} ;

#endif //_EXIF_APP_SEGMENT_H_
