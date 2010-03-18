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

#ifndef _EXIF_TIFF_APP_SEG_H
#define	_EXIF_TIFF_APP_SEG_H

#include <vector>
#include <string>

#include "ExifConf.h"
#include "ExifTypeDefs.h"
#include "ExifAppSegment.h"
#include "ExifTagEntry.h"
#include "ExifIFD.h"
#include "ExifIO.h"

/*!
  \author George Sotak <george.sotak@kodak.com>
  \date   Mon Jan 21 06:43:31 2002
  
  \brief  Provides the interface to Tiff-based application segments
  
  This class may be used for any Tiff-based application segment. The
  toolkit automatically uses it for Application Segment 1 - "Exif". To
  use it for other application segments, register it with the application
  segment factory with the appropriate application segment marker and
  identification string. See documentation on registering application
  segements for details.
  
*/

class EXIF_DECL ExifTiffAppSeg : public ExifAppSegment
{
    public:
	    typedef std::vector<ExifIFD*> ExifIFDVec;
        typedef ExifIFDVec::iterator ExifIFDVecIter;

	    //! Constructor.
        ExifTiffAppSeg( uint16 _marker, const char* _ident ) 
            : ExifAppSegment( _marker, _ident)
            { }

        //! Constructor
        ExifTiffAppSeg( ExifIO* exifio, uint16 _marker, uint16 _length,
            char* _ident, exifoff_t _exifHeaderOffset ) 
            : ExifAppSegment( _marker, _length, _ident, _exifHeaderOffset)
              
            { init( exifio ) ; }

        /*  Copy constructor
        
            This constructor creates a deep copy of the source.
         */
 //       ExifTiffAppSeg( const ExifTiffAppSeg& theSrc ) ;

        //! Destructor
        virtual ~ExifTiffAppSeg();
 
        //! Factory method - creates a copy of the ExifTiffAppSeg.
        inline virtual ExifAppSegment* clone() const
        {
            return new ExifTiffAppSeg(*this);
        }

        // declare that we are a TIFF App Segment!
        virtual bool isTiff()
        { return true; }
        
        //! Initialize the application segment after factory instantiation
        virtual ExifStatus init( ExifIO* exifio, uint16 _length,
            exifoff_t _exifHeaderOffset ) ;

	    /*! Get an existing main IFD corresponding to the provided index.
          
            \param index - The index of main IFD.
            \return A pointer to the ExifIFD object if the IFD found;
                    otherwise NULL.
        */
	    ExifIFD* getIFD( uint16 index) const ;
	
	    /*! Get an existing main or sub IFD.
        
            \param dirPath - Directory path of the IFD to retrieve. 
	        \return A pointer to the ExifIFD object if the IFD found; otherwise NULL.
        */
        ExifIFD* getIFD( const ExifIFDPath& ifdPath ) const ;

	    /*! Create a main IFD.
        
            \return A pointer to the ExifIFD object just created.

            The index of main IFD starts from 0 and increments by 1 with each 
            call to this function.
        */
	    ExifIFD* createIFD( void );

        /*! Create main/sub IFDs specified in the ExifIFDPath.
        
            \param dirPath IFDs to create, specified in ExifIFDPath format

            If the IFDs already exists then nothing happens. Note that each 
            of the IFD indexes in the ExifIFDPath should be less or equal to 
            the next available index for the corresponding IFD tag
        */
	    ExifStatus createIFD( const ExifIFDPath& dirPath);

	    //! Return the number of main directories.
	    inline int numMainIFDs() { return mMainIFDs.size(); }

        /*! Get all the metadata tags in this application segment
        
            \param pathsTags the metadata retrieved from this application segment
            \sa ExifImageFile::getAllTags()

            This is here to support ExifImageFile::getAllTags().
        */
        void getAllTags( ExifPathsTags &pathsTags ) ;
        
        /*! Support for read access to TIFF tag based metadata.
        
            \param tag       the tag number to retrieve.
            \param ifdPath   the IFD path to the tag
            \return pointer to an ExifTagEntry holding the info for the
                    requested tag if found, otherwise NULL
        */
        ExifTagEntry* getGenericTag( exiftag_t tag,
            const ExifIFDPath& ifdPath ) const;

        /*! Set all the metadata tags provided into this application segment
          
            \param pathsTags the metadata to be set into the specified
                           application segment
            \sa ExifImageFile::setAllTags()

            This is here to support ExifImageFile::setAllTags().
        */
        ExifStatus setAllTags( const ExifPathsTags &pathsTags ) ;
        
        /*! Support for write access to TIFF tag based metadata.
          
            \param entry     an instance of ExifTagEntry holding the metadata
                           info to be written
            \param idfPath   the IFD path to the tag
            \return errRtn   error return value
        */
        ExifStatus setGenericTag( const ExifTagEntry& entry,
            const ExifIFDPath& ifdPath  ) ;
        
        /*! Remove the specified tag from the specified IFD
        
            \param tag       the tag number to retrieve.
            \param ifdPath   the IFD path to the tag
         */
        void removeGenericTag( exiftag_t tag, const ExifIFDPath& ifdPath );
            
        /*! Write out the App Segment to the file
        
            \param exifio - handle to the ExifIO to where we will write
         */
        virtual ExifStatus write( ExifIO* exifio ) ;
                
    private:
        ExifTiffHeader mHeader ;
        ExifIFDVec     mMainIFDs;

        ExifStatus init( ExifIO* exifio ) ;
        ExifStatus readHeader( ExifIO* exifio ) ;
        ExifStatus writeHeader( ExifIO* exifio, exifoff_t& tiffHeaderOffset ) ;

        ExifStatus initEndian( ExifIO* exifio, bool bigendian ) ;

        void initHeader( ExifIO* exifio, bool bigendian ) ;
        void readIFDs( ExifIO* exifio );
        exifoff_t writeIFDs( ExifIO* exifio, exifoff_t& tiffHeaderOffset );
};


#endif // _EXIF_TIFF_APP_SEG_H

