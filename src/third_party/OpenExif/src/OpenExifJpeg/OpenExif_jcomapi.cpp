/*
 * jcomapi.c
 *
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent OE_JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains application interface routines that are used for both
 * compression and decompression.
 */

#define OE_JPEG_INTERNALS
#include "OpenExif_jinclude.h"
#include "OpenExifJpegLib.h"


/*
 * Abort processing of a OE_JPEG compression or decompression operation,
 * but don't destroy the object itself.
 *
 * For this, we merely clean up all the nonpermanent memory pools.
 * Note that temp files (virtual arrays) are not allowed to belong to
 * the permanent pool, so we will be able to close all temp files here.
 * Closing a data source or destination, if necessary, is the application's
 * responsibility.
 */

GLOBAL(void)
openexif_jpeg_abort (oe_j_common_ptr cinfo)
{
  int pool;

  /* Do nothing if called on a not-initialized or destroyed OE_JPEG object. */
  if (cinfo->mem == NULL)
    return;

  /* Releasing pools in reverse order might help avoid fragmentation
   * with some (brain-damaged) malloc libraries.
   */
  for (pool = OE_JPOOL_NUMPOOLS-1; pool > OE_JPOOL_PERMANENT; pool--) {
    (*cinfo->mem->free_pool) (cinfo, pool);
  }

  /* Reset overall state for possible reuse of object */
  if (cinfo->is_decompressor) {
    cinfo->global_state = DSTATE_START;
    /* Try to keep application from accessing now-deleted marker list.
     * A bit kludgy to do it here, but this is the most central place.
     */
    ((oe_j_decompress_ptr) cinfo)->marker_list = NULL;
  } else {
    cinfo->global_state = CSTATE_START;
  }
}


/*
 * Destruction of a OE_JPEG object.
 *
 * Everything gets deallocated except the master openexif_jpeg_compress_struct itself
 * and the error manager struct.  Both of these are supplied by the application
 * and must be freed, if necessary, by the application.  (Often they are on
 * the stack and so don't need to be freed anyway.)
 * Closing a data source or destination, if necessary, is the application's
 * responsibility.
 */

GLOBAL(void)
openexif_jpeg_destroy (oe_j_common_ptr cinfo)
{
  /* We need only tell the memory manager to release everything. */
  /* NB: mem pointer is NULL if memory mgr failed to initialize. */
  if (cinfo->mem != NULL)
    (*cinfo->mem->self_destruct) (cinfo);
  cinfo->mem = NULL;		/* be safe if openexif_jpeg_destroy is called twice */
  cinfo->global_state = 0;	/* mark it destroyed */
}


/*
 * Convenience routines for allocating quantization and Huffman tables.
 * (Would jutils.c be a more reasonable place to put these?)
 */

GLOBAL(OE_JQUANT_TBL *)
openexif_jpeg_alloc_quant_table (oe_j_common_ptr cinfo)
{
  OE_JQUANT_TBL *tbl;

  tbl = (OE_JQUANT_TBL *)
    (*cinfo->mem->alloc_small) (cinfo, OE_JPOOL_PERMANENT, SIZEOF(OE_JQUANT_TBL));
  tbl->sent_table = FALSE;	/* make sure this is false in any new table */
  return tbl;
}


GLOBAL(OE_JHUFF_TBL *)
openexif_jpeg_alloc_huff_table (oe_j_common_ptr cinfo)
{
  OE_JHUFF_TBL *tbl;

  tbl = (OE_JHUFF_TBL *)
    (*cinfo->mem->alloc_small) (cinfo, OE_JPOOL_PERMANENT, SIZEOF(OE_JHUFF_TBL));
  tbl->sent_table = FALSE;	/* make sure this is false in any new table */
  return tbl;
}
