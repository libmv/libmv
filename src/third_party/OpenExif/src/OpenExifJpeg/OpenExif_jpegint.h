/*
 * jpegint.h
 *
 * Copyright (C) 1991-1997, Thomas G. Lane.
 * This file is part of the Independent OE_JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file provides common declarations for the various OE_JPEG modules.
 * These declarations are considered internal to the OE_JPEG library; most
 * applications using the library shouldn't need to include this file.
 */


/* Declarations for both compression & decompression */

typedef enum {			/* Operating modes for buffer controllers */
	OE_JBUF_PASS_THRU,		/* Plain stripwise operation */
	/* Remaining modes require a full-image buffer to have been created */
	OE_JBUF_SAVE_SOURCE,	/* Run source subobject only, save output */
	OE_JBUF_CRANK_DEST,	/* Run dest subobject only, using saved data */
	OE_JBUF_SAVE_AND_PASS	/* Run both subobjects, save output */
} OE_J_BUF_MODE;

/* Values of global_state field (jdapi.c has some dependencies on ordering!) */
#define CSTATE_START	100	/* after create_compress */
#define CSTATE_SCANNING	101	/* start_compress done, write_scanlines OK */
#define CSTATE_RAW_OK	102	/* start_compress done, write_raw_data OK */
#define CSTATE_WRCOEFS	103	/* openexif_jpeg_write_coefficients done */
#define DSTATE_START	200	/* after create_decompress */
#define DSTATE_INHEADER	201	/* reading header markers, no SOS yet */
#define DSTATE_READY	202	/* found SOS, ready for start_decompress */
#define DSTATE_PRELOAD	203	/* reading multiscan file in start_decompress*/
#define DSTATE_PRESCAN	204	/* performing dummy pass for 2-pass quant */
#define DSTATE_SCANNING	205	/* start_decompress done, read_scanlines OK */
#define DSTATE_RAW_OK	206	/* start_decompress done, read_raw_data OK */
#define DSTATE_BUFIMAGE	207	/* expecting openexif_jpeg_start_output */
#define DSTATE_BUFPOST	208	/* looking for SOS/EOI in openexif_jpeg_finish_output */
#define DSTATE_RDCOEFS	209	/* reading file in openexif_jpeg_read_coefficients */
#define DSTATE_STOPPING	210	/* looking for EOI in openexif_jpeg_finish_decompress */


/* Declarations for compression modules */

/* Master control module */
struct openexif_jpeg_comp_master {
  OE_JMETHOD(void, prepare_for_pass, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, pass_startup, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, finish_pass, (oe_j_compress_ptr cinfo));

  /* State variables made visible to other modules */
  boolean call_pass_startup;	/* True if pass_startup must be called */
  boolean is_last_pass;		/* True during last pass */
};

/* Main buffer control (downsampled-data buffer) */
struct openexif_jpeg_c_main_controller {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo, OE_J_BUF_MODE pass_mode));
  OE_JMETHOD(void, process_data, (oe_j_compress_ptr cinfo,
			       OE_JSAMPARRAY input_buf, OE_JDIMENSION *in_row_ctr,
			       OE_JDIMENSION in_rows_avail));
};

/* Compression preprocessing (downsampling input buffer control) */
struct openexif_jpeg_c_prep_controller {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo, OE_J_BUF_MODE pass_mode));
  OE_JMETHOD(void, pre_process_data, (oe_j_compress_ptr cinfo,
				   OE_JSAMPARRAY input_buf,
				   OE_JDIMENSION *in_row_ctr,
				   OE_JDIMENSION in_rows_avail,
				   OE_JSAMPIMAGE output_buf,
				   OE_JDIMENSION *out_row_group_ctr,
				   OE_JDIMENSION out_row_groups_avail));
};

/* Coefficient buffer control */
struct openexif_jpeg_c_coef_controller {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo, OE_J_BUF_MODE pass_mode));
  OE_JMETHOD(boolean, compress_data, (oe_j_compress_ptr cinfo,
				   OE_JSAMPIMAGE input_buf));
};

/* Colorspace conversion */
struct openexif_jpeg_color_converter {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, color_convert, (oe_j_compress_ptr cinfo,
				OE_JSAMPARRAY input_buf, OE_JSAMPIMAGE output_buf,
				OE_JDIMENSION output_row, int num_rows));
};

/* Downsampling */
struct openexif_jpeg_downsampler {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, downsample, (oe_j_compress_ptr cinfo,
			     OE_JSAMPIMAGE input_buf, OE_JDIMENSION in_row_index,
			     OE_JSAMPIMAGE output_buf,
			     OE_JDIMENSION out_row_group_index));

  boolean need_context_rows;	/* TRUE if need rows above & below */
};

/* Forward DCT (also controls coefficient quantization) */
struct openexif_jpeg_forward_dct {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo));
  /* perhaps this should be an array??? */
  OE_JMETHOD(void, forward_DCT, (oe_j_compress_ptr cinfo,
			      openexif_jpeg_component_info * compptr,
			      OE_JSAMPARRAY sample_data, OE_JBLOCKROW coef_blocks,
			      OE_JDIMENSION start_row, OE_JDIMENSION start_col,
			      OE_JDIMENSION num_blocks));
};

/* Entropy encoding */
struct openexif_jpeg_entropy_encoder {
  OE_JMETHOD(void, start_pass, (oe_j_compress_ptr cinfo, boolean gather_statistics));
  OE_JMETHOD(boolean, encode_mcu, (oe_j_compress_ptr cinfo, OE_JBLOCKROW *MCU_data));
  OE_JMETHOD(void, finish_pass, (oe_j_compress_ptr cinfo));
};

/* Marker writing */
struct openexif_jpeg_marker_writer {
  OE_JMETHOD(void, write_file_header, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, write_frame_header, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, write_scan_header, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, write_file_trailer, (oe_j_compress_ptr cinfo));
  OE_JMETHOD(void, write_tables_only, (oe_j_compress_ptr cinfo));
  /* These routines are exported to allow insertion of extra markers */
  /* Probably only COM and APPn markers should be written this way */
  OE_JMETHOD(void, write_marker_header, (oe_j_compress_ptr cinfo, int marker,
				      unsigned int datalen));
  OE_JMETHOD(void, write_marker_byte, (oe_j_compress_ptr cinfo, int val));
};


/* Declarations for decompression modules */

/* Master control module */
struct openexif_jpeg_decomp_master {
  OE_JMETHOD(void, prepare_for_output_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, finish_output_pass, (oe_j_decompress_ptr cinfo));

  /* State variables made visible to other modules */
  boolean is_dummy_pass;	/* True during 1st pass for 2-pass quant */
};

/* Input control module */
struct openexif_jpeg_input_controller {
  OE_JMETHOD(int, consume_input, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, reset_input_controller, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, start_input_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, finish_input_pass, (oe_j_decompress_ptr cinfo));

  /* State variables made visible to other modules */
  boolean has_multiple_scans;	/* True if file has multiple scans */
  boolean eoi_reached;		/* True when EOI has been consumed */
};

/* Main buffer control (downsampled-data buffer) */
struct openexif_jpeg_d_main_controller {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo, OE_J_BUF_MODE pass_mode));
  OE_JMETHOD(void, process_data, (oe_j_decompress_ptr cinfo,
			       OE_JSAMPARRAY output_buf, OE_JDIMENSION *out_row_ctr,
			       OE_JDIMENSION out_rows_avail));
};

/* Coefficient buffer control */
struct openexif_jpeg_d_coef_controller {
  OE_JMETHOD(void, start_input_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(int, consume_data, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, start_output_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(int, decompress_data, (oe_j_decompress_ptr cinfo,
				 OE_JSAMPIMAGE output_buf));
  /* Pointer to array of coefficient virtual arrays, or NULL if none */
  oe_jvirt_barray_ptr *coef_arrays;
};

/* Decompression postprocessing (color quantization buffer control) */
struct openexif_jpeg_d_post_controller {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo, OE_J_BUF_MODE pass_mode));
  OE_JMETHOD(void, post_process_data, (oe_j_decompress_ptr cinfo,
				    OE_JSAMPIMAGE input_buf,
				    OE_JDIMENSION *in_row_group_ctr,
				    OE_JDIMENSION in_row_groups_avail,
				    OE_JSAMPARRAY output_buf,
				    OE_JDIMENSION *out_row_ctr,
				    OE_JDIMENSION out_rows_avail));
};

/* Marker reading & parsing */
struct openexif_jpeg_marker_reader {
  OE_JMETHOD(void, reset_marker_reader, (oe_j_decompress_ptr cinfo));
  /* Read markers until SOS or EOI.
   * Returns same codes as are defined for openexif_jpeg_consume_input:
   * OE_JPEG_SUSPENDED, OE_JPEG_REACHED_SOS, or OE_JPEG_REACHED_EOI.
   */
  OE_JMETHOD(int, read_markers, (oe_j_decompress_ptr cinfo));
  /* Read a restart marker --- exported for use by entropy decoder only */
  openexif_jpeg_marker_parser_method read_restart_marker;

  /* State of marker reader --- nominally internal, but applications
   * supplying COM or APPn handlers might like to know the state.
   */
  boolean saw_SOI;		/* found SOI? */
  boolean saw_SOF;		/* found SOF? */
  int next_restart_num;		/* next restart number expected (0-7) */
  unsigned int discarded_bytes;	/* # of bytes skipped looking for a marker */
};

/* Entropy decoding */
struct openexif_jpeg_entropy_decoder {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(boolean, decode_mcu, (oe_j_decompress_ptr cinfo,
				OE_JBLOCKROW *MCU_data));

  /* This is here to share code between baseline and progressive decoders; */
  /* other modules probably should not use it */
  boolean insufficient_data;	/* set TRUE after emitting warning */
};

/* Inverse DCT (also performs dequantization) */
typedef OE_JMETHOD(void, inverse_DCT_method_ptr,
		(oe_j_decompress_ptr cinfo, openexif_jpeg_component_info * compptr,
		 OE_JCOEFPTR coef_block,
		 OE_JSAMPARRAY output_buf, OE_JDIMENSION output_col));

struct openexif_jpeg_inverse_dct {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo));
  /* It is useful to allow each component to have a separate IDCT method. */
  inverse_DCT_method_ptr inverse_DCT[MAX_COMPONENTS];
};

/* Upsampling (note that upsampler must also call color converter) */
struct openexif_jpeg_upsampler {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, upsample, (oe_j_decompress_ptr cinfo,
			   OE_JSAMPIMAGE input_buf,
			   OE_JDIMENSION *in_row_group_ctr,
			   OE_JDIMENSION in_row_groups_avail,
			   OE_JSAMPARRAY output_buf,
			   OE_JDIMENSION *out_row_ctr,
			   OE_JDIMENSION out_rows_avail));

  boolean need_context_rows;	/* TRUE if need rows above & below */
};

/* Colorspace conversion */
struct openexif_jpeg_color_deconverter {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, color_convert, (oe_j_decompress_ptr cinfo,
				OE_JSAMPIMAGE input_buf, OE_JDIMENSION input_row,
				OE_JSAMPARRAY output_buf, int num_rows));
};

/* Color quantization or color precision reduction */
struct openexif_jpeg_color_quantizer {
  OE_JMETHOD(void, start_pass, (oe_j_decompress_ptr cinfo, boolean is_pre_scan));
  OE_JMETHOD(void, color_quantize, (oe_j_decompress_ptr cinfo,
				 OE_JSAMPARRAY input_buf, OE_JSAMPARRAY output_buf,
				 int num_rows));
  OE_JMETHOD(void, finish_pass, (oe_j_decompress_ptr cinfo));
  OE_JMETHOD(void, new_color_map, (oe_j_decompress_ptr cinfo));
};


/* Miscellaneous useful macros */

#undef MAX
#define MAX(a,b)	((a) > (b) ? (a) : (b))
#undef MIN
#define MIN(a,b)	((a) < (b) ? (a) : (b))


/* We assume that right shift corresponds to signed division by 2 with
 * rounding towards minus infinity.  This is correct for typical "arithmetic
 * shift" instructions that shift in copies of the sign bit.  But some
 * C compilers implement >> with an unsigned shift.  For these machines you
 * must define RIGHT_SHIFT_IS_UNSIGNED.
 * RIGHT_SHIFT provides a proper signed right shift of an INT32 quantity.
 * It is only applied with constant shift counts.  SHIFT_TEMPS must be
 * included in the variables of any routine using RIGHT_SHIFT.
 */

#ifdef RIGHT_SHIFT_IS_UNSIGNED
#define SHIFT_TEMPS	INT32 shift_temp;
#define RIGHT_SHIFT(x,shft)  \
	((shift_temp = (x)) < 0 ? \
	 (shift_temp >> (shft)) | ((~((INT32) 0)) << (32-(shft))) : \
	 (shift_temp >> (shft)))
#else
#define SHIFT_TEMPS
#define RIGHT_SHIFT(x,shft)	((x) >> (shft))
#endif


/* Short forms of external names for systems with brain-damaged linkers. */

#ifdef NEED_SHORT_EXTERNAL_NAMES
#define jinit_compress_master	oe_jICompress
#define jinit_c_master_control	oe_jICMaster
#define jinit_c_main_controller	oe_jICMainC
#define jinit_c_prep_controller	oe_jICPrepC
#define jinit_c_coef_controller	oe_jICCoefC
#define jinit_color_converter	oe_jICColor
#define jinit_downsampler	oe_jIDownsampler
#define jinit_forward_dct	oe_jIFDCT
#define jinit_huff_encoder	oe_jIHEncoder
#define jinit_phuff_encoder	oe_jIPHEncoder
#define jinit_marker_writer	oe_jIMWriter
#define jinit_master_decompress	oe_jIDMaster
#define jinit_d_main_controller	oe_jIDMainC
#define jinit_d_coef_controller	oe_jIDCoefC
#define jinit_d_post_controller	oe_jIDPostC
#define jinit_input_controller	oe_jIInCtlr
#define jinit_marker_reader	oe_jIMReader
#define jinit_huff_decoder	oe_jIHDecoder
#define jinit_phuff_decoder	oe_jIPHDecoder
#define jinit_inverse_dct	oe_jIIDCT
#define jinit_upsampler		oe_jIUpsampler
#define jinit_color_deconverter	oe_jIDColor
#define jinit_1pass_quantizer	oe_jI1Quant
#define jinit_2pass_quantizer	oe_jI2Quant
#define jinit_merged_upsampler	oe_jIMUpsampler
#define jinit_memory_mgr	oe_jIMemMgr
#define jdiv_round_up		oe_jDivRound
#define jround_up		oe_jRound
#define jcopy_sample_rows	oe_jCopySamples
#define jcopy_block_row		oe_jCopyBlocks
#define jzero_far		oe_jZeroFar
#define openexif_jpeg_zigzag_order	oe_jZIGTable
#define openexif_jpeg_natural_order	oe_jZAGTable
#endif /* NEED_SHORT_EXTERNAL_NAMES */


/* Compression module initialization routines */
EXTERN(void) jinit_compress_master OE_JPP((oe_j_compress_ptr cinfo));
EXTERN(void) jinit_c_master_control OE_JPP((oe_j_compress_ptr cinfo,
					 boolean transcode_only));
EXTERN(void) jinit_c_main_controller OE_JPP((oe_j_compress_ptr cinfo,
					  boolean need_full_buffer));
EXTERN(void) jinit_c_prep_controller OE_JPP((oe_j_compress_ptr cinfo,
					  boolean need_full_buffer));
EXTERN(void) jinit_c_coef_controller OE_JPP((oe_j_compress_ptr cinfo,
					  boolean need_full_buffer));
EXTERN(void) jinit_color_converter OE_JPP((oe_j_compress_ptr cinfo));
EXTERN(void) jinit_downsampler OE_JPP((oe_j_compress_ptr cinfo));
EXTERN(void) jinit_forward_dct OE_JPP((oe_j_compress_ptr cinfo));
EXTERN(void) jinit_huff_encoder OE_JPP((oe_j_compress_ptr cinfo));
EXTERN(void) jinit_phuff_encoder OE_JPP((oe_j_compress_ptr cinfo));
EXTERN(void) jinit_marker_writer OE_JPP((oe_j_compress_ptr cinfo));
/* Decompression module initialization routines */
EXTERN(void) jinit_master_decompress OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_d_main_controller OE_JPP((oe_j_decompress_ptr cinfo,
					  boolean need_full_buffer));
EXTERN(void) jinit_d_coef_controller OE_JPP((oe_j_decompress_ptr cinfo,
					  boolean need_full_buffer));
EXTERN(void) jinit_d_post_controller OE_JPP((oe_j_decompress_ptr cinfo,
					  boolean need_full_buffer));
EXTERN(void) jinit_input_controller OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_marker_reader OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_huff_decoder OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_phuff_decoder OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_inverse_dct OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_upsampler OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_color_deconverter OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_1pass_quantizer OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_2pass_quantizer OE_JPP((oe_j_decompress_ptr cinfo));
EXTERN(void) jinit_merged_upsampler OE_JPP((oe_j_decompress_ptr cinfo));
/* Memory manager initialization */
EXTERN(void) openexif_jinit_memory_mgr OE_JPP((oe_j_common_ptr cinfo));

/* Utility routines in jutils.c */
EXTERN(long) openexif_jdiv_round_up OE_JPP((long a, long b));
EXTERN(long) openexif_jround_up OE_JPP((long a, long b));
EXTERN(void) openexif_jcopy_sample_rows OE_JPP((OE_JSAMPARRAY input_array, int source_row,
				    OE_JSAMPARRAY output_array, int dest_row,
				    int num_rows, OE_JDIMENSION num_cols));
EXTERN(void) openexif_jcopy_block_row OE_JPP((OE_JBLOCKROW input_row, OE_JBLOCKROW output_row,
				  OE_JDIMENSION num_blocks));
EXTERN(void) openexif_jzero_far OE_JPP((void * target, size_t bytestozero));
/* Constant tables in jutils.c */
#if 0				/* This table is not actually needed in v6a */
extern const int openexif_jpeg_zigzag_order[]; /* natural coef order to zigzag order */
#endif
extern const int openexif_jpeg_natural_order[]; /* zigzag coef order to natural order */

/* Suppress undefined-structure complaints if necessary. */

#ifdef INCOMPLETE_TYPES_BROKEN
#ifndef AM_MEMORY_MANAGER	/* only jmemmgr.c defines these */
struct jvirt_sarray_control { long dummy; };
struct jvirt_barray_control { long dummy; };
#endif
#endif /* INCOMPLETE_TYPES_BROKEN */
