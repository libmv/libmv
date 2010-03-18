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


#ifdef WIN32
#ifdef INTERNET_REQUIRED

#include "ExifInternetIO.h"

#include <WinInet.h>
#pragma comment (lib , "WinInet")

tsize_t
ExifInternetIO::read( tdata_t buf, tsize_t size)
{
	if (mHInternet) 
	{
		unsigned int szread;
		BOOL b = InternetReadFile(mHInternet, buf, size, &szread);
		return b? szread: 0;
	}

	return 0;
}



tsize_t
ExifInternetIO::write( tdata_t buf, tsize_t size )
{
	return 0;
}


exifoff_t
ExifInternetIO::seek( exifoff_t off, int whence)
{
	DWORD dwMoveMethod;
	switch(whence)
	{
	case 0:
		dwMoveMethod = FILE_BEGIN;
		break;
	case 1:
		dwMoveMethod = FILE_CURRENT;
		break;
	case 2:
		dwMoveMethod = FILE_END;
		break;
	default:
		dwMoveMethod = FILE_BEGIN;
		break;
	}
	return (exifoff_t)InternetSetFilePointer(mHInternet, off, 0, dwMoveMethod, 0);
}


exifoff_t ExifInternetIO::size()
{
	unsigned int sz;
	InternetQueryDataAvailable(mHInternet, &sz, 0, 0);
	return sz;
}


ExifStatus ExifInternetIO::close()
{
    InternetCloseHandle(mHInternet);
    
    return EXIF_OK ;
}

#endif // INTERNET_REQUIRED
#endif // WIN32
