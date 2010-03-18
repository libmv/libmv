/*
 * jmemansi.c
 *
 * Copyright (C) 1992-1996, Thomas G. Lane.
 * This file is part of the Independent OE_JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file provides a simple generic implementation of the system-
 * dependent portion of the OE_JPEG memory manager.  This implementation
 * assumes that you have the ANSI-standard library routine tmpfile().
 * Also, the problem of determining the amount of memory available
 * is shoved onto the user.
 */

#define OE_JPEG_INTERNALS
#include "OpenExif_jinclude.h"
#include "OpenExifJpegLib.h"
#include "OpenExif_jmemsys.h"		/* import the system-dependent declarations */

#ifndef HAVE_STDLIB_H		/* <stdlib.h> should declare malloc(),free() */
extern void * malloc OE_JPP((size_t size));
extern void free OE_JPP((void *ptr));
#endif

#ifndef SEEK_SET		/* pre-ANSI systems may not define this; */
#define SEEK_SET  0		/* if not, assume 0 is correct */
#endif


/*
 * Memory allocation and freeing are controlled by the regular library
 * routines malloc() and free().
 */

GLOBAL(void *)
openexif_jpeg_get_small (oe_j_common_ptr cinfo, size_t sizeofobject)
{
  (void)cinfo;
  return (void *) malloc(sizeofobject);
}

GLOBAL(void)
openexif_jpeg_free_small (oe_j_common_ptr cinfo, void * object, size_t sizeofobject)
{
  (void)cinfo;
  (void)sizeofobject;
  free(object);
}


/*
 * "Large" objects are treated the same as "small" ones.
 * NB: although we include FAR keywords in the routine declarations,
 * this file won't actually work in 80x86 small/medium model; at least,
 * you probably won't be able to process useful-size images in only 64KB.
 */

GLOBAL(void *)
openexif_jpeg_get_large (oe_j_common_ptr cinfo, size_t sizeofobject)
{
  (void)cinfo;
  return (void *) malloc(sizeofobject);
}

GLOBAL(void)
openexif_jpeg_free_large (oe_j_common_ptr cinfo, void * object, size_t sizeofobject)
{
  (void)cinfo;
  (void)sizeofobject;
  free(object);
}


/*
 * This routine computes the total memory space available for allocation.
 * It's impossible to do this in a portable way; our current solution is
 * to make the user tell us (with a default value set at compile time).
 * If you can actually get the available space, it's a good idea to subtract
 * a slop factor of 5% or so.
 */

#ifndef DEFAULT_MAX_MEM		/* so can override from makefile */
#define DEFAULT_MAX_MEM		1000000L /* default: one megabyte */
#endif

GLOBAL(long)
openexif_jpeg_mem_available (oe_j_common_ptr cinfo, long min_bytes_needed,
		    long max_bytes_needed, long already_allocated)
{
  (void)cinfo;
  (void)min_bytes_needed;
  (void)max_bytes_needed;
  return cinfo->mem->max_memory_to_use - already_allocated;
}


/*
 * Backing store (temporary file) management.
 * Backing store objects are only used when the value returned by
 * openexif_jpeg_mem_available is less than the total space needed.  You can dispense
 * with these routines if you have plenty of virtual memory; see jmemnobs.c.
 */


METHODDEF(void)
read_backing_store (oe_j_common_ptr cinfo, backing_store_ptr info,
		    void * buffer_address,
		    long file_offset, long byte_count)
{
  if (fseek(info->temp_file, file_offset, SEEK_SET))
    ERREXIT(cinfo, OE_JERR_TFILE_SEEK);
  if (OE_JFREAD(info->temp_file, buffer_address, byte_count)
      != (size_t) byte_count)
    ERREXIT(cinfo, OE_JERR_TFILE_READ);
}


METHODDEF(void)
write_backing_store (oe_j_common_ptr cinfo, backing_store_ptr info,
		     void * buffer_address,
		     long file_offset, long byte_count)
{
  if (fseek(info->temp_file, file_offset, SEEK_SET))
    ERREXIT(cinfo, OE_JERR_TFILE_SEEK);
  if (OE_JFWRITE(info->temp_file, buffer_address, byte_count)
      != (size_t) byte_count)
    ERREXIT(cinfo, OE_JERR_TFILE_WRITE);
}


METHODDEF(void)
close_backing_store (oe_j_common_ptr cinfo, backing_store_ptr info)
{
  (void)cinfo;
  fclose(info->temp_file);
  /* Since this implementation uses tmpfile() to create the file,
   * no explicit file deletion is needed.
   */
}


/*
 * Initial opening of a backing-store object.
 *
 * This version uses tmpfile(), which constructs a suitable file name
 * behind the scenes.  We don't have to use info->temp_name[] at all;
 * indeed, we can't even find out the actual name of the temp file.
 */

GLOBAL(void)
openexif_jpeg_open_backing_store (oe_j_common_ptr cinfo, backing_store_ptr info,
			 long total_bytes_needed)
{
  (void)total_bytes_needed;
  if ((info->temp_file = tmpfile()) == NULL)
    ERREXITS(cinfo, OE_JERR_TFILE_CREATE, "");
  info->read_backing_store = read_backing_store;
  info->write_backing_store = write_backing_store;
  info->close_backing_store = close_backing_store;
}


/*
 * These routines take care of any system-dependent initialization and
 * cleanup required.
 */

GLOBAL(long)
openexif_jpeg_mem_init (oe_j_common_ptr cinfo)
{
  (void)cinfo;
  return DEFAULT_MAX_MEM;	/* default for max_memory_to_use */
}

GLOBAL(void)
openexif_jpeg_mem_term (oe_j_common_ptr cinfo)
{
  (void)cinfo;
  /* no work */
}
