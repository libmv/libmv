/*
 * jdapistd.c
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent OE_JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file contains application interface code for the decompression half
 * of the OE_JPEG library.  These are the "standard" API routines that are
 * used in the normal full-decompression case.  They are not used by a
 * transcoding-only application.  Note that if an application links in
 * openexif_jpeg_start_decompress, it will end up linking in the entire decompressor.
 * We thus must separate this file from jdapimin.c to avoid linking the
 * whole decompression library into a transcoder.
 */

#define OE_JPEG_INTERNALS
#include "OpenExif_jinclude.h"
#include "OpenExifJpegLib.h"


/* Forward declarations */
LOCAL(boolean) output_pass_setup OE_JPP((oe_j_decompress_ptr cinfo));


/*
 * Decompression initialization.
 * openexif_jpeg_read_header must be completed before calling this.
 *
 * If a multipass operating mode was selected, this will do all but the
 * last pass, and thus may take a great deal of time.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */

GLOBAL(boolean)
openexif_jpeg_start_decompress (oe_j_decompress_ptr cinfo)
{
  if (cinfo->global_state == DSTATE_READY) {
    /* First call: initialize master control, select active modules */
    jinit_master_decompress(cinfo);
    if (cinfo->buffered_image) {
      /* No more work here; expecting openexif_jpeg_start_output next */
      cinfo->global_state = DSTATE_BUFIMAGE;
      return TRUE;
    }
    cinfo->global_state = DSTATE_PRELOAD;
  }
  if (cinfo->global_state == DSTATE_PRELOAD) {
    /* If file has multiple scans, absorb them all into the coef buffer */
    if (cinfo->inputctl->has_multiple_scans) {
#ifdef D_MULTISCAN_FILES_SUPPORTED
      for (;;) {
	int retcode;
	/* Call progress monitor hook if present */
	if (cinfo->progress != NULL)
	  (*cinfo->progress->progress_monitor) ((oe_j_common_ptr) cinfo);
	/* Absorb some more input */
	retcode = (*cinfo->inputctl->consume_input) (cinfo);
	if (retcode == OE_JPEG_SUSPENDED)
	  return FALSE;
	if (retcode == OE_JPEG_REACHED_EOI)
	  break;
	/* Advance progress counter if appropriate */
	if (cinfo->progress != NULL &&
	    (retcode == OE_JPEG_ROW_COMPLETED || retcode == OE_JPEG_REACHED_SOS)) {
	  if (++cinfo->progress->pass_counter >= cinfo->progress->pass_limit) {
	    /* jdmaster underestimated number of scans; ratchet up one scan */
	    cinfo->progress->pass_limit += (long) cinfo->total_iMCU_rows;
	  }
	}
      }
#else
      ERREXIT(cinfo, OE_JERR_NOT_COMPILED);
#endif /* D_MULTISCAN_FILES_SUPPORTED */
    }
    cinfo->output_scan_number = cinfo->input_scan_number;
  } else if (cinfo->global_state != DSTATE_PRESCAN)
    ERREXIT1(cinfo, OE_JERR_BAD_STATE, cinfo->global_state);
  /* Perform any dummy output passes, and set up for the final pass */
  return output_pass_setup(cinfo);
}


/*
 * Set up for an output pass, and perform any dummy pass(es) needed.
 * Common subroutine for openexif_jpeg_start_decompress and openexif_jpeg_start_output.
 * Entry: global_state = DSTATE_PRESCAN only if previously suspended.
 * Exit: If done, returns TRUE and sets global_state for proper output mode.
 *       If suspended, returns FALSE and sets global_state = DSTATE_PRESCAN.
 */

LOCAL(boolean)
output_pass_setup (oe_j_decompress_ptr cinfo)
{
  if (cinfo->global_state != DSTATE_PRESCAN) {
    /* First call: do pass setup */
    (*cinfo->master->prepare_for_output_pass) (cinfo);
    cinfo->output_scanline = 0;
    cinfo->global_state = DSTATE_PRESCAN;
  }
  /* Loop over any required dummy passes */
  while (cinfo->master->is_dummy_pass) {
#ifdef QUANT_2PASS_SUPPORTED
    /* Crank through the dummy pass */
    while (cinfo->output_scanline < cinfo->output_height) {
      OE_JDIMENSION last_scanline;
      /* Call progress monitor hook if present */
      if (cinfo->progress != NULL) {
	cinfo->progress->pass_counter = (long) cinfo->output_scanline;
	cinfo->progress->pass_limit = (long) cinfo->output_height;
	(*cinfo->progress->progress_monitor) ((oe_j_common_ptr) cinfo);
      }
      /* Process some data */
      last_scanline = cinfo->output_scanline;
      (*cinfo->main->process_data) (cinfo, (OE_JSAMPARRAY) NULL,
				    &cinfo->output_scanline, (OE_JDIMENSION) 0);
      if (cinfo->output_scanline == last_scanline)
	return FALSE;		/* No progress made, must suspend */
    }
    /* Finish up dummy pass, and set up for another one */
    (*cinfo->master->finish_output_pass) (cinfo);
    (*cinfo->master->prepare_for_output_pass) (cinfo);
    cinfo->output_scanline = 0;
#else
    ERREXIT(cinfo, OE_JERR_NOT_COMPILED);
#endif /* QUANT_2PASS_SUPPORTED */
  }
  /* Ready for application to drive output pass through
   * openexif_jpeg_read_scanlines or openexif_jpeg_read_raw_data.
   */
  cinfo->global_state = cinfo->raw_data_out ? DSTATE_RAW_OK : DSTATE_SCANNING;
  return TRUE;
}


/*
 * Read some scanlines of data from the OE_JPEG decompressor.
 *
 * The return value will be the number of lines actually read.
 * This may be less than the number requested in several cases,
 * including bottom of image, data source suspension, and operating
 * modes that emit multiple scanlines at a time.
 *
 * Note: we warn about excess calls to openexif_jpeg_read_scanlines() since
 * this likely signals an application programmer error.  However,
 * an oversize buffer (max_lines > scanlines remaining) is not an error.
 */

GLOBAL(OE_JDIMENSION)
openexif_jpeg_read_scanlines (oe_j_decompress_ptr cinfo, OE_JSAMPARRAY scanlines,
		     OE_JDIMENSION max_lines)
{
  OE_JDIMENSION row_ctr;

  if (cinfo->global_state != DSTATE_SCANNING)
    ERREXIT1(cinfo, OE_JERR_BAD_STATE, cinfo->global_state);
  if (cinfo->output_scanline >= cinfo->output_height) {
    WARNMS(cinfo, OE_JWRN_TOO_MUCH_DATA);
    return 0;
  }

  /* Call progress monitor hook if present */
  if (cinfo->progress != NULL) {
    cinfo->progress->pass_counter = (long) cinfo->output_scanline;
    cinfo->progress->pass_limit = (long) cinfo->output_height;
    (*cinfo->progress->progress_monitor) ((oe_j_common_ptr) cinfo);
  }

  /* Process some data */
  row_ctr = 0;
  (*cinfo->main->process_data) (cinfo, scanlines, &row_ctr, max_lines);
  cinfo->output_scanline += row_ctr;
  return row_ctr;
}


/*
 * Alternate entry point to read raw data.
 * Processes exactly one iMCU row per call, unless suspended.
 */

GLOBAL(OE_JDIMENSION)
openexif_jpeg_read_raw_data (oe_j_decompress_ptr cinfo, OE_JSAMPIMAGE data,
		    OE_JDIMENSION max_lines)
{
  OE_JDIMENSION lines_per_iMCU_row;

  if (cinfo->global_state != DSTATE_RAW_OK)
    ERREXIT1(cinfo, OE_JERR_BAD_STATE, cinfo->global_state);
  if (cinfo->output_scanline >= cinfo->output_height) {
    WARNMS(cinfo, OE_JWRN_TOO_MUCH_DATA);
    return 0;
  }

  /* Call progress monitor hook if present */
  if (cinfo->progress != NULL) {
    cinfo->progress->pass_counter = (long) cinfo->output_scanline;
    cinfo->progress->pass_limit = (long) cinfo->output_height;
    (*cinfo->progress->progress_monitor) ((oe_j_common_ptr) cinfo);
  }

  /* Verify that at least one iMCU row can be returned. */
  lines_per_iMCU_row = cinfo->max_v_samp_factor * cinfo->min_DCT_scaled_size;
  if (max_lines < lines_per_iMCU_row)
    ERREXIT(cinfo, OE_JERR_BUFFER_SIZE);

  /* Decompress directly into user's buffer. */
  if (! (*cinfo->coef->decompress_data) (cinfo, data))
    return 0;			/* suspension forced, can do nothing more */

  /* OK, we processed one iMCU row. */
  cinfo->output_scanline += lines_per_iMCU_row;
  return lines_per_iMCU_row;
}


/* Additional entry points for buffered-image mode. */

#ifdef D_MULTISCAN_FILES_SUPPORTED

/*
 * Initialize for an output pass in buffered-image mode.
 */

GLOBAL(boolean)
openexif_jpeg_start_output (oe_j_decompress_ptr cinfo, int scan_number)
{
  if (cinfo->global_state != DSTATE_BUFIMAGE &&
      cinfo->global_state != DSTATE_PRESCAN)
    ERREXIT1(cinfo, OE_JERR_BAD_STATE, cinfo->global_state);
  /* Limit scan number to valid range */
  if (scan_number <= 0)
    scan_number = 1;
  if (cinfo->inputctl->eoi_reached &&
      scan_number > cinfo->input_scan_number)
    scan_number = cinfo->input_scan_number;
  cinfo->output_scan_number = scan_number;
  /* Perform any dummy output passes, and set up for the real pass */
  return output_pass_setup(cinfo);
}


/*
 * Finish up after an output pass in buffered-image mode.
 *
 * Returns FALSE if suspended.  The return value need be inspected only if
 * a suspending data source is used.
 */

GLOBAL(boolean)
openexif_jpeg_finish_output (oe_j_decompress_ptr cinfo)
{
  if ((cinfo->global_state == DSTATE_SCANNING ||
       cinfo->global_state == DSTATE_RAW_OK) && cinfo->buffered_image) {
    /* Terminate this pass. */
    /* We do not require the whole pass to have been completed. */
    (*cinfo->master->finish_output_pass) (cinfo);
    cinfo->global_state = DSTATE_BUFPOST;
  } else if (cinfo->global_state != DSTATE_BUFPOST) {
    /* BUFPOST = repeat call after a suspension, anything else is error */
    ERREXIT1(cinfo, OE_JERR_BAD_STATE, cinfo->global_state);
  }
  /* Read markers looking for SOS or EOI */
  while (cinfo->input_scan_number <= cinfo->output_scan_number &&
	 ! cinfo->inputctl->eoi_reached) {
    if ((*cinfo->inputctl->consume_input) (cinfo) == OE_JPEG_SUSPENDED)
      return FALSE;		/* Suspend, come back later */
  }
  cinfo->global_state = DSTATE_BUFIMAGE;
  return TRUE;
}

#endif /* D_MULTISCAN_FILES_SUPPORTED */
