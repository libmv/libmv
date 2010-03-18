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
 * Portions of the Original Code are
 * Copyright (c) 1988-1996 Sam Leffler
 * Copyright (c) 1991-1996 Silicon Graphics, Inc.
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


#include "ExifImageDescUtils.h"

ExifStatus exifAllocImageDesc( ExifImageInfo& imgInfo,
                              ExifImageDesc &imageDesc)
{
    imageDesc.numberOfColumns = imgInfo.width ;
    imageDesc.numberOfRows = imgInfo.height ;
    imageDesc.numberOfComponents = imgInfo.numChannels ;

    uint8 *tmpReadBuf = 
        new uint8[imgInfo.width*imgInfo.height*imgInfo.numChannels];

    if(!tmpReadBuf)
        return EXIF_LOW_MEMORY_ERROR;

    for( int i=0; i<imgInfo.numChannels; i++)
    {
        imageDesc.components[i].myColorType.myDataType = UNSIGNED_BYTE;
        imageDesc.components[i].columnStride = imgInfo.numChannels;
        imageDesc.components[i].lineStride = imgInfo.numChannels*imgInfo.width;
        // Data is interleaved, e.g., RGBRGB
        imageDesc.components[i].theData = tmpReadBuf+i;
    }

    return EXIF_OK ;
}

ExifStatus exifDeleteImageDesc( ExifImageDesc &imageDesc )
{

    delete [] imageDesc.components[0].theData ;
    
    return EXIF_OK ;
}
