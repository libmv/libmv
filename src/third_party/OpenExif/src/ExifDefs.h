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

#ifndef _EXIF_DEFS_H_
#define _EXIF_DEFS_H_
/*
 * This define can be used in code that requires
 * compilation-related definitions specific to a
 * version or versions of the library.  Runtime
 * version checking should be done based on the
 * string returned by EXIFGetVersion.
 */

/*!
  \file   ExifDefs.h
  \author  <george.sotak@kodak.com>
  \date   Sun Jan 20 17:48:59 2002
  
  \brief  Basic #defines for OpenExif

*/

#define	EXIFLIB_VERSION	19960307	/* March 7, 1996 */


#define	EXIF_VERSION	42

//! Tiff indicator for Big Endianess
#define	EXIF_BIGENDIAN		0x4D4D
//! Tiff indicator for Little Endianess
#define	EXIF_LITTLEENDIAN	0x4949


#ifndef NULL
#define	NULL	0
#endif

//! \name Exif / JPEG App Markers
//@{
#define EXIF_SOI_MARKER         0xFFD8   //!< Start of Image
#define EXIF_APP0_MARKER        0xFFE0   //!< Application Segement 0
#define EXIF_APP1_MARKER        0xFFE1   //!< Application Segement 1
#define EXIF_APP2_MARKER        0xFFE2   //!< Application Segement 3
#define EXIF_APP3_MARKER        0xFFE3   //!< Application Segement 3
#define EXIF_APP4_MARKER        0xFFE4   //!< Application Segement 4
#define EXIF_APP5_MARKER        0xFFE5   //!< Application Segement 5
#define EXIF_APP6_MARKER        0xFFE6   //!< Application Segement 6
#define EXIF_APP7_MARKER        0xFFE7   //!< Application Segement 7
#define EXIF_APP8_MARKER        0xFFE8   //!< Application Segement 8
#define EXIF_APP9_MARKER        0xFFE9   //!< Application Segement 9
#define EXIF_APP10_MARKER       0xFFEA   //!< Application Segement 10
#define EXIF_APP11_MARKER       0xFFEB   //!< Application Segement 11
#define EXIF_APP12_MARKER       0xFFEC   //!< Application Segement 12
#define EXIF_APP13_MARKER       0xFFED   //!< Application Segement 13
#define EXIF_APP14_MARKER       0xFFEE   //!< Application Segement 14
#define EXIF_APP15_MARKER       0xFFEF   //!< Application Segement 15
#define EXIF_LAST_APP_MARKER    0xFFEF   //!< Last possible App Segment
#define EXIF_DQT_MARKER         0xFFDB   //!< Quantization Table
#define EXIF_SOS_MARKER         0xFFDA   //!< Start of Scan
#define EXIF_SOF0_MARKER        0xFFC0
#define EXIF_SOF15_MARKER       0xFFCF
//@}

#define EXIF_MAINIFD 0  // used in location field of tagsxk

    
//! \name ExifIO flag attribute values
//@{
//! natural bit fill order for machine
#define	EXIF_FILLORDER		0x0003	
//! header must be written on close
#define	EXIF_DIRTYHEADER	0x0004	
//! current directory must be written 
#define	EXIF_DIRTYDIRECT	0x0008
//! data buffers setup 
#define	EXIF_BUFFERSETUP	0x0010	
//! encoder/decoder setup done 
#define	EXIF_CODERSETUP		0x0020	
//! written 1+ scanlines to file 
#define	EXIF_BEENWRITING	0x0040	
//! byte swap file information 
#define	EXIF_SWAB		0x0080	
//! inhibit bit reversal logic 
#define	EXIF_NOBITREV		0x0100	
//! my raw data buffer; free on close 
#define	EXIF_MYBUFFER		0x0200	
//! file is tile, not strip- based 
#define	EXIF_ISTILED		0x0400	
//! file is mapped into memory 
#define	EXIF_MAPPED		    0x0800	
//! Enable memory mapped files 
#define	EXIF_ENABLE_MEMMAP	0x1000	
//! need call to postencode routine 
#define	EXIF_POSTENCODE		0x2000	
//! library is doing data up-sampling  
#define	EXIF_UPSAMPLED		0x4000	
//@}


//#define MAX_COMP_THUMBNAIL_SIZE MAX_SEGMENT_SIZE
#define MAX_COMP_THUMBNAIL_SIZE 60000


/* NB: the exif_uint32 casts are to silence certain ANSI-C compilers */
#define	EXIFhowmany(x, y) ((((exif_uint32)(x))+(((exif_uint32)(y))-1))/((exif_uint32)(y)))
#define	EXIFroundup(x, y) (EXIFhowmany(x,y)*((exif_uint32)(y)))

#endif // _EXIF_DEFS_H_
