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
 * Ricardo Rosario <ricardo.rosario@kodak.com>
 *
 * Contributor(s): 
 * FirstName LastName <email address>
 * FirstName LastName <email address>
 */ 
 
#include "ExifAppSegManager.h"
#include "ExifErrorCodes.h"
#include "ExifRawAppSeg.h"

// Private Constructor. Note that the top (first) appseg manager node will
// contain no real data other than a pointer to his child. This consumes a
// little extra memory but makes it simpler and faster. The user of this class
// (ExifImageFile), will only interract with this top node.
ExifAppSegManager::ExifAppSegManager( ExifAppSegment* appSeg, 
    ExifAppSegManager* child)
  : mChild(child), mAppSeg(appSeg)
{ 
    if ( (appSeg) && ( appSeg->isRaw() ) )
        mIsConverted = false ;
    else
        mIsConverted = true ;
}

// Destructor.
ExifAppSegManager::~ExifAppSegManager()
{
    // Cleanup
    if ( mChild )
        delete mChild ;
    if ( mAppSeg )
        delete mAppSeg ;
}

// Insert an Application Segment
ExifStatus ExifAppSegManager::insert( ExifAppSegment* appSeg )
{
    ExifStatus status = EXIF_OK ;
    
    if ( !mChild )
    {
        // We are at the end of the list, so append a new node
        mChild = new ExifAppSegManager( appSeg, NULL ) ;
    }
    else
    {
        // if the next app seg marker is greater than the one we are inserting,
        // then insert here, otherwise move on to next node
        if ( appSeg->getAppSegmentMarker() < 
            mChild->mAppSeg->getAppSegmentMarker() )
        {
            mChild = new ExifAppSegManager( appSeg, mChild ) ;
        }
        else
            status = mChild->insert( appSeg ) ;
    }
    
    return status ;
}


// Get the first application segment with the specified ID
ExifAppSegment* ExifAppSegManager::getAppSeg( uint16 marker, const char * ID,
    bool tryToConvert )
{
    ExifAppSegment* appSeg = NULL ;
    
    // if there is no child, we will return NULL
    if ( mChild )
    {
        // if the child app seg has the specified ID, we set our return value
        // to it, otherwise we move on to next node
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 ) 
        {
            if ( tryToConvert )
                appSeg = mChild->getThisAppSeg() ;
            else
                appSeg = mChild->mAppSeg ;
        }
        else
            appSeg = mChild->getAppSeg( marker, ID, tryToConvert ) ;
    }
    
    return appSeg ;
}

// Get all the application segments with the specified ID
std::vector< ExifAppSegment* > ExifAppSegManager::getAllAppSegs( uint16 marker, 
    const char * ID, bool tryToConvert )
{
    std::vector<ExifAppSegment*> appSegs ;
    
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            if ( tryToConvert )
                appSegs.push_back( mChild->getThisAppSeg() ) ;
            else
                appSegs.push_back( mChild->mAppSeg ) ;
        }

        mChild->getAllAppSegs( marker, ID, tryToConvert, appSegs ) ;    
    }
    
    return appSegs ;
}

// Get all the application segments.
std::vector< ExifAppSegment* > ExifAppSegManager::getAllAppSegs( bool tryToConvert )
{
    std::vector<ExifAppSegment*> appSegs ;
    
    if ( mChild )
    {
        if ( tryToConvert )
            appSegs.push_back( mChild->getThisAppSeg() ) ;
        else
            appSegs.push_back( mChild->mAppSeg ) ;

        mChild->getAllAppSegs( tryToConvert, appSegs ) ;
    }
    
    return appSegs ;
}

// Get the first raw application segment with the specified ID.
ExifAppSegment* ExifAppSegManager::getRawAppSeg( uint16 marker,
    const char * ID, bool tryToConvert )
{
    ExifAppSegment* appSeg = NULL ;
    
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            if ( tryToConvert )
                appSeg = mChild->getThisAppSeg() ;
            else
                appSeg = mChild->mAppSeg ;
            
            // If it is not raw, keep on searching
            if ( !appSeg->isRaw() )
                appSeg = mChild->getRawAppSeg( marker, ID, tryToConvert ) ;
        }
        else
            appSeg = mChild->getRawAppSeg( marker, ID, tryToConvert ) ;
    }
    
    return appSeg ;
}

// Get all the raw application segments with the specified ID
std::vector<ExifAppSegment*> ExifAppSegManager::getAllRawAppSegs( uint16 marker,
    const char * ID, bool tryToConvert )
{
    std::vector<ExifAppSegment*> appSegs ;
    
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            if ( tryToConvert )
                mChild->getThisAppSeg() ;
            
            if ( mChild->mAppSeg->isRaw() )
                appSegs.push_back( mChild->mAppSeg ) ;
        }
        
        mChild->getAllRawAppSegs( marker, ID, tryToConvert, appSegs ) ; 
    }
    
    return appSegs ;

}

// Get all the raw application segments.
std::vector<ExifAppSegment*> ExifAppSegManager::getAllRawAppSegs( bool tryToConvert )
{
    std::vector<ExifAppSegment*> appSegs ;
    
    if ( mChild )
    {
        if ( tryToConvert )
            mChild->getThisAppSeg() ;
            
        if ( mChild->mAppSeg->isRaw() )
            appSegs.push_back( mChild->mAppSeg ) ;

        mChild->getAllRawAppSegs( tryToConvert, appSegs ) ;
    }
    
    return appSegs ;
}

// Get all the tiff based application segments.
std::vector<ExifAppSegment*> ExifAppSegManager::getAllTiffAppSegs()
{
    std::vector<ExifAppSegment*> appSegs ;
    
    if ( mChild )
    {
        // This will convert the app seg, if necessary
        mChild->getThisAppSeg() ;
            
        if ( mChild->mAppSeg->isTiff() )
            appSegs.push_back( mChild->mAppSeg ) ;

        mChild->getAllTiffAppSegs( appSegs ) ;
    }
    
    return appSegs ;
}

// Delete the specified Application Segment. If there is more than one App
// Segment with the ID, only the first one will get deleted.
ExifStatus ExifAppSegManager::remove( uint16 marker, const char * ID )
{
    ExifStatus status = EXIF_OK ;
    
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            ExifAppSegManager* grandChild = mChild->mChild ;

            // Before deleting, need to set the child's child to NULL so that
            // the child's destructor doesn't go on deleting the rest of the
            // nodes in the list
            mChild->mChild = NULL ;
            delete mChild ;
            
            // now our new child is the node that used to be the grandchild
            mChild = grandChild ;
        }
        else
            status = mChild->remove( marker, ID ) ;
    }
    else
        status = EXIF_ERROR ;

    return status ;
}

// Delete all Application Segments with the specified ID
ExifStatus ExifAppSegManager::removeAll( uint16 marker, const char * ID ) 
{
    ExifStatus status = EXIF_OK ;
    
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            ExifAppSegManager* grandChild = mChild->mChild ;

            // Before deleting, need to set my child's child to NULL so that
            // my child's destructor doesn't go on deleting the rest of the
            // nodes in the list
            mChild->mChild = NULL ;
            delete mChild ;
            
            // now my new child is the node that used to be my grandchild
            mChild = grandChild ;
            
            // I have a new child so I have recurse on myself
            status = removeAll( marker, ID ) ;
        }
        else
            status = mChild->removeAll( marker, ID ) ;
    }

    return status ;
}

// This method returns the app seg in the current node
ExifAppSegment* ExifAppSegManager::getThisAppSeg()
{
    if ( mAppSeg && (mAppSeg->isRaw()))
    {
        // If we have an appseg and haven't tried converting it already, then
        // try to convert
        
        // Try to get an implementation from the factory
        std::string id ;
        ExifAppSegment::createAppId(mAppSeg->getAppSegmentMarker(),
                                    mAppSeg->getAppIdent(), id ) ;
        ExifAppSegment* tempAppSeg = ExifAppSegment::create( id, false ) ;
        if ( tempAppSeg )
        {
            // if there is an implementation, then convert from the raw appseg
            // to the implemented one.
            ExifRawAppSeg* rawAppSeg = dynamic_cast<ExifRawAppSeg*>(mAppSeg) ;
            if ( rawAppSeg )
            {
                ExifStatus status ;
                char * rawAppSegData = rawAppSeg->getRawData() ;
                ExifIO* exifio = new ExifIO( status, rawAppSegData,
                    rawAppSeg->getLength(), "r+", rawAppSeg->getLength() ) ;
                if ( status == EXIF_OK )
                {
                    // we need to pass to init() the exifio with the current
                    // position just past the identifier (first non-Null byte
                    // after identifier). 
                    int i = 2 ;
                    while( rawAppSegData[i] != '\0' ) i++ ;
                    i++; // get us past the first NULL
                    if ( tempAppSeg->isTiff() )
                        while( rawAppSegData[i] == '\0' ) i++ ;
                    exifio->seek( i, SEEK_SET ) ;
                    status = tempAppSeg->init(exifio,rawAppSeg->getLength(),i) ;
                    
                    if ( status == EXIF_OK )
                    {
                        // if all goes well, replace raw app seg with converted
                        // one
                        ExifAppSegment * tmp = mAppSeg ;
                        mAppSeg = tempAppSeg ;
                        tempAppSeg = tmp;
                    }
                }
                delete exifio ;
            }
            delete tempAppSeg ;
        }

        // we tried converting, so set flag to true
        mIsConverted = true ;
    }
    
    return mAppSeg ;
}

// This method gets all the application segments with the specified ID
// (private version)
void ExifAppSegManager::getAllAppSegs( uint16 marker, const char * ID,
    bool tryToConvert, std::vector<ExifAppSegment*>& vec )
{
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            if ( tryToConvert )
                vec.push_back( mChild->getThisAppSeg() ) ;
            else
                vec.push_back( mChild->mAppSeg ) ;
        }
        
        mChild->getAllAppSegs( marker, ID, tryToConvert, vec ) ;    
    }
}

// This method gets all the application segments
// (private version)
void ExifAppSegManager::getAllAppSegs( bool tryToConvert,
    std::vector<ExifAppSegment*>& vec )
{
    if ( mChild )
    {
        if ( tryToConvert )
            vec.push_back( mChild->getThisAppSeg() ) ;
        else
            vec.push_back( mChild->mAppSeg ) ;
        
        mChild->getAllAppSegs( tryToConvert, vec ) ;
    }
}

// Get all the raw application segments with the specified ID
// (private version)
void ExifAppSegManager::getAllRawAppSegs( uint16 marker, const char * ID,
    bool tryToConvert,
    std::vector<ExifAppSegment*>& vec  )
{
    if ( mChild )
    {
        if ( mChild->mAppSeg->getAppSegmentMarker() == marker &&
            strcmp(mChild->mAppSeg->getAppIdent(),ID) == 0 )
        {
            if ( tryToConvert )
                mChild->getThisAppSeg() ;
            
            if ( mChild->mAppSeg->isRaw() )
                vec.push_back( mChild->mAppSeg ) ;
        }
        
        mChild->getAllRawAppSegs( marker, ID, tryToConvert, vec ) ; 
    }
}

// Get all the raw application segments
// (private version)
void ExifAppSegManager::getAllRawAppSegs( bool tryToConvert,
    std::vector<ExifAppSegment*>& vec )
{
    if ( mChild )
    {
        if ( tryToConvert )
            mChild->getThisAppSeg() ;

        if ( mChild->mAppSeg->isRaw() )
            vec.push_back( mChild->mAppSeg ) ;
        
        mChild->getAllRawAppSegs( tryToConvert, vec ) ; 
    }
}

// Get all the tiff application segments
// (private version)
void ExifAppSegManager::getAllTiffAppSegs(std::vector<ExifAppSegment*>& vec)
{
    if ( mChild )
    {
        // try to convert
        mChild->getThisAppSeg() ;

        if ( mChild->mAppSeg->isTiff() )
            vec.push_back( mChild->mAppSeg ) ;
        
        mChild->getAllTiffAppSegs( vec ) ; 
    }
}

// clear the list
void ExifAppSegManager::clear()
{
    if ( mChild )
    {
        delete mChild ;
        mChild = NULL ;
    }
    if ( mAppSeg )
    {
        delete mAppSeg ;
        mAppSeg = NULL ;
    }
}
