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
 * Creation Date: 02/28/2003
 *
 * Original Author: 
 * Ricardo Rosario ricardo.rosario@kodak.com
 *
 * Contributor(s): 
 * FirstName LastName <email address>
 * FirstName LastName <email address>
 */ 


#ifndef _EXIF_APP_SEG_MANAGER_H
#define	_EXIF_APP_SEG_MANAGER_H

#include <vector>

#include "ExifConf.h"
#include "ExifTypeDefs.h"
#include "ExifErrorCodes.h"
#include "ExifAppSegment.h"

/*!
  \author Ricardo Rosario <ricardo.rosario@kodak.com>
  \date   Fri Feb 28 13:46:00 EST 2003
  
  \brief  The Application Segment Manager

*/

class ExifAppSegManager
{
    public:
        //! Default Constructor
        ExifAppSegManager()
          : mChild(NULL), mAppSeg(NULL), mIsConverted(false) { }
        
        //! Destructor
        ~ExifAppSegManager() ;

        //! Insert an Application Segment.
        /*!
            HERE HERE NOTE (NEED TO REVIEW AND DISCUSS): Once you insert the
            app seg, the AppSegManager owns it and will delete it when you call
            one of the remove fuctions or when the destructor gets called.
            RLR: This sounds bad. But I've made it this way (for now at least) 
            because I realized there is a lot more overhead if we want it to 
            clone the passed in AppSeg, because we end up doing a new, an insert
            (which basically is another new), then we do a delete of the
            original immediately after inserting. Given that ExifImageFile is
            the only class that has access to the AppSegManager and, therefore,
            no App should interact with it in any way other than through our
            ExifImageFile interface, this may be just fine. If an App passes in
            a app seg, then ExifImageFile can clone it before inserting it.
            
            \param appSeg - the ExifAppSegment to insert
            \return ExifStatus
         */
        ExifStatus insert( ExifAppSegment * appSeg ) ;
        
        //! Get the first application segment with the specified ID. 
        /*!
            This method returns the actual app seg, not a copy.
            Do not delete it!
            
            \param marker - the App Segment marker
            \param ID - the App Segment identifier
            \param tryToConvert - try to convert the raw app seg (default: true)
         */
        ExifAppSegment* getAppSeg( uint16 marker, const char * ID,
            bool tryToConvert = true ) ;
        
        /*! Get all the application segments with the specified ID.
        
            This method returns the actual app segs, not copies.
            Do not delete them!
            
            \param marker - the App Segment marker
            \param ID - the App Segment identifier
            \param tryToConvert - try to convert the raw app seg (default: true)
         */
        std::vector<ExifAppSegment*> getAllAppSegs( uint16 marker, const char * ID, 
            bool tryToConvert = true ) ;

        //! Get all the application segments.
        /*!
            This method returns the actual app segs, not copies.
            Do not delete them!
            
            \param tryToConvert - try to convert the raw app seg (default: true)
         */
        std::vector<ExifAppSegment*> getAllAppSegs( bool tryToConvert = true ) ;

        //! Get the first raw application segment with the specified ID.
        /*!
            This method returns NULL if their is no such raw app seg.
            Otherwise, it returns the actual app seg, not a copy.
            Do not delete it!
            
            \param marker - the App Segment marker
            \param ID - the App Segment identifier
            \param tryToConvert - try to convert the raw app seg (default: true)
         */
        ExifAppSegment* getRawAppSeg( uint16 marker, const char * ID,
            bool tryToConvert = true ) ;

        //! Get all the raw application segments with the specified ID.
        /*!
            This method returns the actual app segs, not copies.
            Do not delete them!
            
            \param ID - the App Segment identifier
            \param tryToConvert - try to convert the raw app seg (default: true)
         */
        std::vector<ExifAppSegment*> getAllRawAppSegs( uint16 marker,
            const char * ID, bool tryToConvert = true ) ;

        //! Get all the raw application segments.
        /*! 
            This method returns the actual app segs, not copies.
            Do not delete them!
            
            \param tryToConvert - try to convert the raw app seg (default: true)
         */
        std::vector<ExifAppSegment*> getAllRawAppSegs( bool tryToConvert = true ) ;

        //! Get all the tiff based application segments.
        /*!
            This method returns the actual app segs, not copies.
            Do not delete them!
         */
        std::vector<ExifAppSegment*> getAllTiffAppSegs() ;

        //! Delete an Application Segment with the specified ID.
        /*!  
            If there is more than one App Segment with the ID, only the first
            one will get deleted. Returns EXIF_ERROR if no app seg with the ID
            is found, otherwise returns EXIF_OK.
            
            \param marker - the App Segment marker
            \param ID - the App Segment identifier
         */
        ExifStatus remove( uint16 marker, const char * ID ) ;
        
        //! Delete all the Application Segments with the specified ID.
        /*!
            \param marker - the App Segment marker
            \param ID - the App Segment identifier
         */
        ExifStatus removeAll( uint16 marker, const char * ID ) ;

        //! Delete all the app segments
        void clear() ;
    private:
        
        //! Private Constructor
        ExifAppSegManager( ExifAppSegment* appSeg, 
                           ExifAppSegManager* child = NULL ) ;
    
        //! This method tries to convert the appseg, if necessary and returns it
        inline ExifAppSegment* getThisAppSeg() ;

        //! A pointer to the next ExifAppSegManager node
        ExifAppSegManager* mChild ;
        
        //! The actual Application Segment
        ExifAppSegment* mAppSeg ;
        
        //! A state variable. True if we've tried to convert the raw app seg
        bool mIsConverted ;
        
        // The following functions are called by the public functions with the
        // same name. Their purpose is to avoid copying a vector for every
        // recursive call.
        
        //! This method gets all the application segments with the specified ID
        void getAllAppSegs( uint16 marker, const char * ID, bool tryToConvert,
            std::vector<ExifAppSegment*>& vec ) ;
    
        //! This method gets all the application segments
        void getAllAppSegs( bool tryToConvert, std::vector<ExifAppSegment*>& vec ) ;

        //! Get all the raw application segments with the specified ID
        void getAllRawAppSegs( uint16 marker, const char * ID,
            bool tryToConvert, std::vector<ExifAppSegment*>& vec  ) ;

        //! Get all the raw application segments
        void getAllRawAppSegs(bool tryToConvert, std::vector<ExifAppSegment*>& vec);
        
        //! Get all the tiff application segments
        void getAllTiffAppSegs( std::vector<ExifAppSegment*>& vec );
};

#endif // _EXIF_APP_SEG_MANAGER_H
