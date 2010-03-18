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

#ifndef _EXIF_JPEG_EXTN_H_
#define _EXIF_JPEG_EXTN_H_

#ifndef OPENEXIF_NO_IJG

#include "OpenExifJpegLib.h"
#define SIZEOF(object)       ((size_t) sizeof(object))

typedef struct {
        int bytes_in_buffer ;
        size_t size_buffer ;
} openexif_jpeg_mem_client;

typedef openexif_jpeg_mem_client* openexif_jpeg_mem_client_ptr;

typedef struct {
  struct openexif_jpeg_destination_mgr pub; /* public fields */

  OE_JOCTET *NIFbuffer;
  OE_JOCTET *buffer;
} nif_destination_mgr;

typedef nif_destination_mgr *nif_dest_ptr;


/* and similarly for an in memory version for NIFTY. */
typedef struct {
  struct openexif_jpeg_source_mgr pub;   /* public fields */

  int    bufSize;
  OE_JOCTET *NIFbuffer;
  OE_JOCTET *buffer;
  boolean start_of_file;
  int   offset;
} nif_source_mgr;

typedef nif_source_mgr *nif_src_ptr;

/* and one for macintosh FSSpec */
#ifdef macintosh
typedef struct {
  struct jpeg_source_mgr pub;   /* public fields */
 
  short refNum;
  OE_JOCTET * buffer;      /* start of buffer */
  boolean start_of_file;    /* have we gotten any data yet? */
} nif_ref_source_mgr;
typedef nif_ref_source_mgr *nif_ref_src_ptr;
#endif  /* macintosh */


EXTERN(void) openexif_jpeg_mem_dest OE_JPP((oe_j_compress_ptr cinfo, OE_JOCTET *buffer));
EXTERN(void) openexif_jpeg_mem_src OE_JPP((oe_j_decompress_ptr cinfo, OE_JOCTET *buffer,
                              int bufsize));
#ifdef macintosh
EXTERN(void) jpeg_ref_src OE_JPP((oe_j_decompress_ptr cinfo, short refNum));
#endif

#ifdef JPEG_INTERNET_ENABLED
#ifdef WIN32
#ifdef INTERNET_REQUIRED
EXTERN(void) oe_jpeg_internet_dest OE_JPP((j_compress_ptr cinfo, HINTERNET outfile));
EXTERN(void) oe_jpeg_internet_src OE_JPP((j_decompress_ptr cinfo, HINTERNET infile));
#endif // INTERNET_REQUIRED
#endif
#endif

#endif

#endif
