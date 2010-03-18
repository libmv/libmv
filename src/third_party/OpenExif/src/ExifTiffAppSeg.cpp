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


#if (defined _MSC_VER)
#pragma warning( disable : 4786 )
#endif

#include "ExifConf.h"
#include "ExifIO.h"
#include "ExifTagEntry.h"
#include "ExifIFD.h"
#include "ExifTiffAppSeg.h"


ExifTiffAppSeg::~ExifTiffAppSeg()
{
    // Clean Up the IFDs 
    ExifIFDVecIter crnt = mMainIFDs.begin() ;
    ExifIFDVecIter end = mMainIFDs.end() ;
    while ( crnt != end )
    {
        delete (*crnt) ;
        crnt ++;
    }
    mMainIFDs.clear() ;
}

void ExifTiffAppSeg::readIFDs( ExifIO* exifio )
{  
    ExifStatus status = EXIF_OK;
    uint16 ifdcount ;
    exifoff_t ifdOffset;
    exifoff_t nextOff = mHeader.ifdoff ;

    ExifIFDMap mainIFDs;
    
    uint16 idx = 0;

    // Loop until we reach the last IFD to read (nextOff = 0)
	// or we've come to the end of the app segment
    while ((nextOff != 0)&&(nextOff < mLength)&&(status==EXIF_OK))
    {
        ifdOffset = nextOff + mExifOffset;

        exifio->seek( ifdOffset, SEEK_SET ) ;
      
        exifio->read( &ifdcount, sizeof(uint16) ) ;
      
        exifio->swabShort( &ifdcount ) ;
      
		if ( ifdcount <= 0 )
			break;

        exifio->seek( ifdcount*sizeof(ExifDirEntry), SEEK_CUR ) ;
      
        exifio->read( &nextOff, sizeof(exifoff_t) ) ;

        exifio->swabLong( (exif_uint32*)(&nextOff) ) ;

        ExifIFDMapIter mapIter = mainIFDs.find(ifdOffset);
        if (mapIter == mainIFDs.end())
        {
            ExifIFD * ifd = new ExifIFD( EXIF_MAINIFD,
                ifdOffset, mExifOffset, idx, &mainIFDs, NULL );
            mMainIFDs.push_back(ifd);
            mainIFDs.insert(ExifIFDMap::value_type(ifdOffset,ifd));

            ifd->readDirTree( exifio ) ;
        }
        else 
            status = EXIF_FILE_READ_ERROR;
            
        idx++ ;
    }
}


ExifStatus ExifTiffAppSeg::write( ExifIO* exifio )
{
    exifio->swabShort( &mMarker ) ;
    exifio->write( &mMarker, sizeof(uint16) ) ;

    // no need to swab length, this isn't a real number at this point
    mOffsetToLength = exifio->seek( 0, SEEK_CUR ) ;
    exifio->write( &mLength, sizeof(uint16) ) ;
    // byte data -- no need to swab
    exifio->write( &mIdent[0], 6*sizeof(char) ) ;
       
    setMyExifioFlags( exifio ) ;

    exifoff_t tiffHeaderOffset ;
    
    // This is the Tiff header
    writeHeader( exifio, tiffHeaderOffset ) ;
    // Now write out all the directories
    exifoff_t endOfData = writeIFDs( exifio, tiffHeaderOffset ) ;

    uint16 theLength = (uint16) (endOfData - mOffsetToLength) ;

    restoreExifioFlags( exifio ) ;
    
    exifio->seek( mOffsetToLength, SEEK_SET ) ;
    exifio->swabShort( &theLength ) ;
    exifio->write( &theLength, sizeof(uint16) ) ;
    exifio->seek( endOfData, SEEK_SET ) ;

    return EXIF_OK ;
}


ExifStatus ExifTiffAppSeg::writeHeader( ExifIO* exifio,
                                       exifoff_t& tiffHeaderOffset )
{
    tiffHeaderOffset = exifio->seek( 0, SEEK_CUR ) ;

    // header.version was swabbed in initEndian, need to swab back before
    // writing
    exifio->swabShort(&mHeader.version);

    if( exifio->write( (tdata_t)&mHeader, sizeof (ExifTiffHeader) ) !=
        sizeof(ExifTiffHeader) )
    {
        return EXIF_FILE_WRITE_ERROR;
    }

  return EXIF_OK ;
}


exifoff_t ExifTiffAppSeg::writeIFDs( ExifIO* exifio, exifoff_t& tiffHeaderOffset )
{
    exifoff_t linkDirOffset = tiffHeaderOffset + sizeof(ExifTiffHeader) -
        sizeof(exif_uint32) ;
    exifoff_t retval = 0;

    ExifIFDVecIter i = mMainIFDs.begin();
    while(i != mMainIFDs.end())
    {   
        (*i)->setDataOffset( linkDirOffset ) ;
        linkDirOffset = (*i)->writeDirTree( exifio, tiffHeaderOffset );
        retval = (*i)->getEndOfData();
        i++;
    }
    return retval ;
    
}


ExifIFD* ExifTiffAppSeg::getIFD( uint16 index) const
{
    ExifIFD* returnValue = NULL ;
    if ( index < mMainIFDs.size() )
    {
        ExifIFDVecConstIter i = mMainIFDs.begin();
        while ((i != mMainIFDs.end())&&((*i)->getIndex() != index)) i++;
        if (i != mMainIFDs.end())
            returnValue = (*i);
    }
    return returnValue ;
}


ExifIFD* ExifTiffAppSeg::getIFD( const ExifIFDPath& ifdPath ) const
{
    if (ifdPath.empty())
        return NULL ;

    ExifIFD* crntIFD = getIFD(ifdPath[0].second) ;
    if( crntIFD == 0)
        return crntIFD;

    for (unsigned int i=1; i<ifdPath.size(); i++)
    {
        ExifIFDVec ifdVec;
        if (crntIFD->getSubIFDVec(ifdPath[i].first, ifdVec) &&
            ifdPath[i].second < ifdVec.size())
        {
            crntIFD = ifdVec[ifdPath[i].second];		
        }
        else
            return NULL;
    }

    return crntIFD ;
}

  
void ExifTiffAppSeg::getAllTags( ExifPathsTags &pathsTags )
{
    ExifIFDVecIter crnt = mMainIFDs.begin() ;
    ExifIFDVecIter end = mMainIFDs.end() ;

    while( crnt != end )
    {
        (*crnt)->getAllTags( pathsTags ) ;
        crnt++ ;
    }
}

        
    
ExifTagEntry* ExifTiffAppSeg::getGenericTag( exiftag_t tag,
                                            const ExifIFDPath& ifdPath ) const
{
    const ExifIFD* ifd = getIFD( ifdPath ) ;

    if( ifd != NULL )
        return ifd->getGenericTag( tag ) ;
    else
        return NULL ;
}

// Remove the specified tag from the specified IFD
void ExifTiffAppSeg::removeGenericTag( exiftag_t tag,
                                            const ExifIFDPath& ifdPath )
{
    ExifIFD* ifd = getIFD( ifdPath ) ;

    if( ifd != NULL )
        ifd->removeGenericTag( tag ) ;
}

ExifStatus ExifTiffAppSeg::setAllTags( const ExifPathsTags &pathsTags )
{

    ExifPathsTagsConstIter crntPathsTags = pathsTags.begin() ;
    ExifPathsTagsConstIter endPathsTags = pathsTags.end() ;
    while( crntPathsTags != endPathsTags )
    {

        const ExifIFDPath& ifdPath = (*crntPathsTags).first ;
        ExifIFD* ifd = getIFD( ifdPath );
         
        if (!ifd)
        {
            ExifStatus error ;
            if( (error = createIFD( ifdPath)) == EXIF_OK )          
                ifd = getIFD(ifdPath);
        }
        
        if (ifd)
        {
            ExifTagsConstIter crnt = (*crntPathsTags).second.begin() ;
            ExifTagsConstIter end = (*crntPathsTags).second.end() ;

            while( crnt != end )
            {
                ExifTagEntry* tag = *(crnt) ;
                ifd->setGenericTag( *tag ); 
                crnt++ ;
            }
        }
        crntPathsTags++ ;
    }
 
    return EXIF_OK ;
}

ExifStatus ExifTiffAppSeg::setGenericTag( const ExifTagEntry& entry,
                              const ExifIFDPath& ifdPath )
{
    ExifIFD* ifd = getIFD(ifdPath);
    if (!ifd)
    {
        ExifStatus error ;
        if( (error = createIFD( ifdPath)) != EXIF_OK )
            return error ;
        
        ifd = getIFD(ifdPath);
    }

    ifd->setGenericTag(entry);

    return EXIF_OK;
}


ExifStatus ExifTiffAppSeg::init( ExifIO* exifio, uint16 _length,
                                exifoff_t _exifHeaderOffset )
{
    ExifAppSegment::init( exifio, _length, _exifHeaderOffset ) ;
    
    return init( exifio ) ;
}

    
ExifStatus ExifTiffAppSeg::init( ExifIO* exifio )
{
    ExifStatus error = EXIF_OK;
    bool bigendian = HOST_BIGENDIAN ;

    /*
     *   Default is to return data MSB2LSB
     */
    setMyExifioFlags( exifio ) ;
    
    exifio->flags() |= FILLORDER_MSB2LSB; // recommended setting by Tiff spec.

    // Setup for writing new
    if( exifio->mode() & O_CREAT )
    {
        initHeader( exifio, bigendian ) ;
        error = initEndian( exifio, bigendian );
        /* END OF Setting up a new file for writing */
    }
    else
    {
        if( (error = readHeader( exifio )) == EXIF_OK)
        {
            if( (error = initEndian( exifio, bigendian )) == EXIF_OK )
            {
                // verify version
                if( mHeader.version == EXIF_VERSION )
                {
                    /*
                     * Setup initial directory.
                     */
                    readIFDs( exifio );
                }
                else
                    error = EXIF_INVALID_FORMAT_ERROR ;
            }
        }
		// If we can't read it, it must be a new app seg, so we need 
		// to init the header.
		else if ( exifio->mode() & O_RDWR )
		{
			initHeader( exifio, bigendian ) ;
		}
    }
    
    restoreExifioFlags( exifio ) ;
    
    return error;
}

ExifStatus ExifTiffAppSeg::readHeader( ExifIO* exifio )
{
    ExifStatus returnValue = EXIF_OK ;
    if( ( exifio->read( &mHeader.magic, 2 ) != 2 ) ||
      ( exifio->read( &mHeader.version, 2 ) != 2 ) ||
      ( exifio->read( &mHeader.ifdoff, 4 ) != 4 ) )
        returnValue = EXIF_FILE_READ_ERROR ;

    return returnValue ;
}


void ExifTiffAppSeg::initHeader( ExifIO* exifio, bool bigendian )
{
  mHeader.magic = exifio->flags() & EXIF_SWAB
    ? (bigendian ? EXIF_LITTLEENDIAN : EXIF_BIGENDIAN)
    : (bigendian ? EXIF_BIGENDIAN : EXIF_LITTLEENDIAN);

  mHeader.version = EXIF_VERSION;

  mHeader.ifdoff = 0;	/* filled in later */

  return ;
}


ExifStatus ExifTiffAppSeg::initEndian( ExifIO* exifio, bool bigendian )
{
  if(mHeader.magic == EXIF_BIGENDIAN) 
    {
      exifio->bigEndian( true ) ;
       if( !bigendian )
        exifio->flags() |= EXIF_SWAB;
    } 
  else 
      if (mHeader.magic == EXIF_LITTLEENDIAN)
      {
          exifio->bigEndian( false ) ;
          if (bigendian)
              exifio->flags() |= EXIF_SWAB;
      }
      else
      {
          return EXIF_BAD_MAGIC_NUMBER ;
      }
  
  // Swab header
  exifio->swabShort(&mHeader.version);
  exifio->swabLong(&mHeader.ifdoff);

  return EXIF_OK;
}


ExifIFD* ExifTiffAppSeg::createIFD( void )
{
    int16 ifdIndex = mMainIFDs.size();
    ExifIFD * ifd = new ExifIFD(EXIF_MAINIFD, ifdIndex, NULL,NULL);
    mMainIFDs.push_back(ifd);
    return ifd;
}


ExifStatus ExifTiffAppSeg::createIFD( const ExifIFDPath& ifdPath)
{
    if (ifdPath.empty())
        return EXIF_BAD_IFD_PATH ;

    int nextInd = mMainIFDs.size();
    int ind = ifdPath[0].second;
    if (nextInd == ind)
        createIFD( ) ;
    else 
        if (nextInd < ind)
	{
            return EXIF_BAD_IFD_PATH ;
	}

    ExifIFD* crntPath = getIFD(ifdPath[0].second) ;
    for (unsigned int i=1; i<ifdPath.size(); i++)
    {
        ExifIFDVec ifdVec;
        crntPath->getSubIFDVec(ifdPath[i].first, ifdVec);
        nextInd = ifdVec.size();
        ind = ifdPath[i].second;
        if (nextInd == ind)
            crntPath->addSubIFD(ifdPath[i].first);
        else 
            if (nextInd < ind)
            {
                return EXIF_BAD_IFD_PATH ;
            }
        crntPath = crntPath->getSubIFD(ifdPath[i].first, ifdPath[i].second);
    }

    return EXIF_OK ;
}

