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
 * Creation Date: 06/05/2005
 *
 * Original Author: 
 * Sam Fryer samuel.fryer@kodak.com 
 *
 * Contributor(s): 
 * 
 */ 

#ifndef _EXIF_COM_MARKER_H_
#define _EXIF_COM_MARKER_H_

class ExifComMarkerList;

class ExifComMarker
{
    public:
            
        ExifComMarker() : next(NULL), mSize(0), mData(NULL) {};
    
        ~ExifComMarker()
        {
            if (mData != NULL) delete[] mData;
            if (next != NULL) delete next;
        };
        
        const uint8 * getData(tsize_t &size)
        {
            size = mSize;
            return mData;
        };
        
        tsize_t setData(uint8 * data, tsize_t size)
        {
            mSize = size;
            mData = new uint8[size];
            memcpy(mData,data,size);
            return mSize;
        };
        
        friend class ExifComMarkerList;
        
    private:
            
        ExifComMarker * next;
        tsize_t mSize;
        uint8 * mData;
};


class ExifComMarkerList
{
    public:

        ExifComMarkerList() : mCount(0),mMarkers(NULL) {};
    
        ~ExifComMarkerList()
        {
            if (mMarkers != NULL) delete mMarkers;
        };
        
        const unsigned int size()
        {
            return mCount;
        }
        
        ExifComMarker * getComMarker (unsigned int index)
        {
            unsigned int i = 0;
            ExifComMarker * res = NULL;
            ExifComMarker * marker = mMarkers;
            if (mCount > 0)
            {
                while ((i < mCount-1) && (i<index) && (marker != NULL))
                { marker = marker->next; i++; }
                if (i == index) res = marker;
            }
            return res;
        }
        
        unsigned int addData(uint8 * data, tsize_t dataSize)
        {
            ExifComMarker * newMarker = new ExifComMarker();
            newMarker->setData(data,dataSize);
            if (mMarkers == NULL)
            {
                mMarkers = newMarker;
            }
            else
            {
               ExifComMarker * marker = mMarkers;
               while ( marker->next != NULL) marker = marker->next;
               marker->next = newMarker;
            }
            mCount++;
            return mCount;
        }
        
    private:
            
        unsigned int mCount;
        ExifComMarker * mMarkers;
};

  
#endif
