/*
 * jerror.h
 *
 * Copyright (C) 1994-1997, Thomas G. Lane.
 * This file is part of the Independent OE_JPEG Group's software.
 * For conditions of distribution and use, see the accompanying README file.
 *
 * This file defines the error and message codes for the OE_JPEG library.
 * Edit this file to add new codes, or to translate the message strings to
 * some other language.
 * A set of error-reporting macros are defined too.  Some applications using
 * the OE_JPEG library may wish to include this file to get the error codes
 * and/or the macros.
 */

/*
 * To define the enum list of message codes, include this file without
 * defining macro OE_JMESSAGE.  To create a message string table, include it
 * again with a suitable OE_JMESSAGE definition (see jerror.c for an example).
 */
#ifndef OE_JMESSAGE
#ifndef OE_JERROR_H
/* First time through, define the enum list */
#define OE_JMAKE_ENUM_LIST
#else
/* Repeated inclusions of this file are no-ops unless OE_JMESSAGE is defined */
#define OE_JMESSAGE(code,string)
#endif /* OE_JERROR_H */
#endif /* OE_JMESSAGE */

#ifdef OE_JMAKE_ENUM_LIST

typedef enum {

#define OE_JMESSAGE(code,string)	code ,

#endif /* OE_JMAKE_ENUM_LIST */

OE_JMESSAGE(OE_JMSG_NOMESSAGE, "Bogus message code %d") /* Must be first entry! */

/* For maintenance convenience, list is alphabetical by message code name */
OE_JMESSAGE(OE_JERR_ARITH_NOTIMPL,
	 "Sorry, there are legal restrictions on arithmetic coding")
OE_JMESSAGE(OE_JERR_BAD_ALIGN_TYPE, "ALIGN_TYPE is wrong, please fix")
OE_JMESSAGE(OE_JERR_BAD_ALLOC_CHUNK, "MAX_ALLOC_CHUNK is wrong, please fix")
OE_JMESSAGE(OE_JERR_BAD_BUFFER_MODE, "Bogus buffer control mode")
OE_JMESSAGE(OE_JERR_BAD_COMPONENT_ID, "Invalid component ID %d in SOS")
OE_JMESSAGE(OE_JERR_BAD_DCT_COEF, "DCT coefficient out of range")
OE_JMESSAGE(OE_JERR_BAD_DCTSIZE, "IDCT output block size %d not supported")
OE_JMESSAGE(OE_JERR_BAD_HUFF_TABLE, "Bogus Huffman table definition")
OE_JMESSAGE(OE_JERR_BAD_IN_COLORSPACE, "Bogus input colorspace")
OE_JMESSAGE(OE_JERR_BAD_J_COLORSPACE, "Bogus OE_JPEG colorspace")
OE_JMESSAGE(OE_JERR_BAD_LENGTH, "Bogus marker length")
OE_JMESSAGE(OE_JERR_BAD_LIB_VERSION,
	 "Wrong OE_JPEG library version: library is %d, caller expects %d")
OE_JMESSAGE(OE_JERR_BAD_MCU_SIZE, "Sampling factors too large for interleaved scan")
OE_JMESSAGE(OE_JERR_BAD_POOL_ID, "Invalid memory pool code %d")
OE_JMESSAGE(OE_JERR_BAD_PRECISION, "Unsupported OE_JPEG data precision %d")
OE_JMESSAGE(OE_JERR_BAD_PROGRESSION,
	 "Invalid progressive parameters Ss=%d Se=%d Ah=%d Al=%d")
OE_JMESSAGE(OE_JERR_BAD_PROG_SCRIPT,
	 "Invalid progressive parameters at scan script entry %d")
OE_JMESSAGE(OE_JERR_BAD_SAMPLING, "Bogus sampling factors")
OE_JMESSAGE(OE_JERR_BAD_SCAN_SCRIPT, "Invalid scan script at entry %d")
OE_JMESSAGE(OE_JERR_BAD_STATE, "Improper call to OE_JPEG library in state %d")
OE_JMESSAGE(OE_JERR_BAD_STRUCT_SIZE,
	 "JPEG parameter struct mismatch: library thinks size is %u, caller expects %u")
OE_JMESSAGE(OE_JERR_BAD_VIRTUAL_ACCESS, "Bogus virtual array access")
OE_JMESSAGE(OE_JERR_BUFFER_SIZE, "Buffer passed to OE_JPEG library is too small")
OE_JMESSAGE(OE_JERR_CANT_SUSPEND, "Suspension not allowed here")
OE_JMESSAGE(OE_JERR_CCIR601_NOTIMPL, "CCIR601 sampling not implemented yet")
OE_JMESSAGE(OE_JERR_COMPONENT_COUNT, "Too many color components: %d, max %d")
OE_JMESSAGE(OE_JERR_CONVERSION_NOTIMPL, "Unsupported color conversion request")
OE_JMESSAGE(OE_JERR_DAC_INDEX, "Bogus DAC index %d")
OE_JMESSAGE(OE_JERR_DAC_VALUE, "Bogus DAC value 0x%x")
OE_JMESSAGE(OE_JERR_DHT_INDEX, "Bogus DHT index %d")
OE_JMESSAGE(OE_JERR_DQT_INDEX, "Bogus DQT index %d")
OE_JMESSAGE(OE_JERR_EMPTY_IMAGE, "Empty OE_JPEG image (DNL not supported)")
OE_JMESSAGE(OE_JERR_EMS_READ, "Read from EMS failed")
OE_JMESSAGE(OE_JERR_EMS_WRITE, "Write to EMS failed")
OE_JMESSAGE(OE_JERR_EOI_EXPECTED, "Didn't expect more than one scan")
OE_JMESSAGE(OE_JERR_FILE_READ, "Input file read error")
OE_JMESSAGE(OE_JERR_FILE_WRITE, "Output file write error --- out of disk space?")
OE_JMESSAGE(OE_JERR_FRACT_SAMPLE_NOTIMPL, "Fractional sampling not implemented yet")
OE_JMESSAGE(OE_JERR_HUFF_CLEN_OVERFLOW, "Huffman code size table overflow")
OE_JMESSAGE(OE_JERR_HUFF_MISSING_CODE, "Missing Huffman code table entry")
OE_JMESSAGE(OE_JERR_IMAGE_TOO_BIG, "Maximum supported image dimension is %u pixels")
OE_JMESSAGE(OE_JERR_INPUT_EMPTY, "Empty input file")
OE_JMESSAGE(OE_JERR_INPUT_EOF, "Premature end of input file")
OE_JMESSAGE(OE_JERR_MISMATCHED_QUANT_TABLE,
	 "Cannot transcode due to multiple use of quantization table %d")
OE_JMESSAGE(OE_JERR_MISSING_DATA, "Scan script does not transmit all data")
OE_JMESSAGE(OE_JERR_MODE_CHANGE, "Invalid color quantization mode change")
OE_JMESSAGE(OE_JERR_NOTIMPL, "Not implemented yet")
OE_JMESSAGE(OE_JERR_NOT_COMPILED, "Requested feature was omitted at compile time")
OE_JMESSAGE(OE_JERR_NO_BACKING_STORE, "Backing store not supported")
OE_JMESSAGE(OE_JERR_NO_HUFF_TABLE, "Huffman table 0x%02x was not defined")
OE_JMESSAGE(OE_JERR_NO_IMAGE, "JPEG datastream contains no image")
OE_JMESSAGE(OE_JERR_NO_QUANT_TABLE, "Quantization table 0x%02x was not defined")
OE_JMESSAGE(OE_JERR_NO_SOI, "Not a OE_JPEG file: starts with 0x%02x 0x%02x")
OE_JMESSAGE(OE_JERR_OUT_OF_MEMORY, "Insufficient memory (case %d)")
OE_JMESSAGE(OE_JERR_QUANT_COMPONENTS,
	 "Cannot quantize more than %d color components")
OE_JMESSAGE(OE_JERR_QUANT_FEW_COLORS, "Cannot quantize to fewer than %d colors")
OE_JMESSAGE(OE_JERR_QUANT_MANY_COLORS, "Cannot quantize to more than %d colors")
OE_JMESSAGE(OE_JERR_SOF_DUPLICATE, "Invalid OE_JPEG file structure: two SOF markers")
OE_JMESSAGE(OE_JERR_SOF_NO_SOS, "Invalid OE_JPEG file structure: missing SOS marker")
OE_JMESSAGE(OE_JERR_SOF_UNSUPPORTED, "Unsupported OE_JPEG process: SOF type 0x%02x")
OE_JMESSAGE(OE_JERR_SOI_DUPLICATE, "Invalid OE_JPEG file structure: two SOI markers")
OE_JMESSAGE(OE_JERR_SOS_NO_SOF, "Invalid OE_JPEG file structure: SOS before SOF")
OE_JMESSAGE(OE_JERR_TFILE_CREATE, "Failed to create temporary file %s")
OE_JMESSAGE(OE_JERR_TFILE_READ, "Read failed on temporary file")
OE_JMESSAGE(OE_JERR_TFILE_SEEK, "Seek failed on temporary file")
OE_JMESSAGE(OE_JERR_TFILE_WRITE,
	 "Write failed on temporary file --- out of disk space?")
OE_JMESSAGE(OE_JERR_TOO_LITTLE_DATA, "Application transferred too few scanlines")
OE_JMESSAGE(OE_JERR_UNKNOWN_MARKER, "Unsupported marker type 0x%02x")
OE_JMESSAGE(OE_JERR_VIRTUAL_BUG, "Virtual array controller messed up")
OE_JMESSAGE(OE_JERR_WIDTH_OVERFLOW, "Image too wide for this implementation")
OE_JMESSAGE(OE_JERR_XMS_READ, "Read from XMS failed")
OE_JMESSAGE(OE_JERR_XMS_WRITE, "Write to XMS failed")
OE_JMESSAGE(OE_JMSG_COPYRIGHT, OE_JCOPYRIGHT)
OE_JMESSAGE(OE_JMSG_VERSION, OE_JVERSION)
OE_JMESSAGE(OE_JTRC_16BIT_TABLES,
	 "Caution: quantization tables are too coarse for baseline OE_JPEG")
OE_JMESSAGE(OE_JTRC_ADOBE,
	 "Adobe APP14 marker: version %d, flags 0x%04x 0x%04x, transform %d")
OE_JMESSAGE(OE_JTRC_APP0, "Unknown APP0 marker (not OE_JFIF), length %u")
OE_JMESSAGE(OE_JTRC_APP14, "Unknown APP14 marker (not Adobe), length %u")
OE_JMESSAGE(OE_JTRC_DAC, "Define Arithmetic Table 0x%02x: 0x%02x")
OE_JMESSAGE(OE_JTRC_DHT, "Define Huffman Table 0x%02x")
OE_JMESSAGE(OE_JTRC_DQT, "Define Quantization Table %d  precision %d")
OE_JMESSAGE(OE_JTRC_DRI, "Define Restart Interval %u")
OE_JMESSAGE(OE_JTRC_EMS_CLOSE, "Freed EMS handle %u")
OE_JMESSAGE(OE_JTRC_EMS_OPEN, "Obtained EMS handle %u")
OE_JMESSAGE(OE_JTRC_EOI, "End Of Image")
OE_JMESSAGE(OE_JTRC_HUFFBITS, "        %3d %3d %3d %3d %3d %3d %3d %3d")
OE_JMESSAGE(OE_JTRC_JFIF, "JFIF APP0 marker: version %d.%02d, density %dx%d  %d")
OE_JMESSAGE(OE_JTRC_JFIF_BADTHUMBNAILSIZE,
	 "Warning: thumbnail image size does not match data length %u")
OE_JMESSAGE(OE_JTRC_JFIF_EXTENSION,
	 "JFIF extension marker: type 0x%02x, length %u")
OE_JMESSAGE(OE_JTRC_JFIF_THUMBNAIL, "    with %d x %d thumbnail image")
OE_JMESSAGE(OE_JTRC_MISC_MARKER, "Miscellaneous marker 0x%02x, length %u")
OE_JMESSAGE(OE_JTRC_PARMLESS_MARKER, "Unexpected marker 0x%02x")
OE_JMESSAGE(OE_JTRC_QUANTVALS, "        %4u %4u %4u %4u %4u %4u %4u %4u")
OE_JMESSAGE(OE_JTRC_QUANT_3_NCOLORS, "Quantizing to %d = %d*%d*%d colors")
OE_JMESSAGE(OE_JTRC_QUANT_NCOLORS, "Quantizing to %d colors")
OE_JMESSAGE(OE_JTRC_QUANT_SELECTED, "Selected %d colors for quantization")
OE_JMESSAGE(OE_JTRC_RECOVERY_ACTION, "At marker 0x%02x, recovery action %d")
OE_JMESSAGE(OE_JTRC_RST, "RST%d")
OE_JMESSAGE(OE_JTRC_SMOOTH_NOTIMPL,
	 "Smoothing not supported with nonstandard sampling ratios")
OE_JMESSAGE(OE_JTRC_SOF, "Start Of Frame 0x%02x: width=%u, height=%u, components=%d")
OE_JMESSAGE(OE_JTRC_SOF_COMPONENT, "    Component %d: %dhx%dv q=%d")
OE_JMESSAGE(OE_JTRC_SOI, "Start of Image")
OE_JMESSAGE(OE_JTRC_SOS, "Start Of Scan: %d components")
OE_JMESSAGE(OE_JTRC_SOS_COMPONENT, "    Component %d: dc=%d ac=%d")
OE_JMESSAGE(OE_JTRC_SOS_PARAMS, "  Ss=%d, Se=%d, Ah=%d, Al=%d")
OE_JMESSAGE(OE_JTRC_TFILE_CLOSE, "Closed temporary file %s")
OE_JMESSAGE(OE_JTRC_TFILE_OPEN, "Opened temporary file %s")
OE_JMESSAGE(OE_JTRC_THUMB_JPEG,
	 "JFIF extension marker: OE_JPEG-compressed thumbnail image, length %u")
OE_JMESSAGE(OE_JTRC_THUMB_PALETTE,
	 "JFIF extension marker: palette thumbnail image, length %u")
OE_JMESSAGE(OE_JTRC_THUMB_RGB,
	 "JFIF extension marker: RGB thumbnail image, length %u")
OE_JMESSAGE(OE_JTRC_UNKNOWN_IDS,
	 "Unrecognized component IDs %d %d %d, assuming YCbCr")
OE_JMESSAGE(OE_JTRC_XMS_CLOSE, "Freed XMS handle %u")
OE_JMESSAGE(OE_JTRC_XMS_OPEN, "Obtained XMS handle %u")
OE_JMESSAGE(OE_JWRN_ADOBE_XFORM, "Unknown Adobe color transform code %d")
OE_JMESSAGE(OE_JWRN_BOGUS_PROGRESSION,
	 "Inconsistent progression sequence for component %d coefficient %d")
OE_JMESSAGE(OE_JWRN_EXTRANEOUS_DATA,
	 "Corrupt OE_JPEG data: %u extraneous bytes before marker 0x%02x")
OE_JMESSAGE(OE_JWRN_HIT_MARKER, "Corrupt OE_JPEG data: premature end of data segment")
OE_JMESSAGE(OE_JWRN_HUFF_BAD_CODE, "Corrupt OE_JPEG data: bad Huffman code")
OE_JMESSAGE(OE_JWRN_JFIF_MAJOR, "Warning: unknown OE_JFIF revision number %d.%02d")
OE_JMESSAGE(OE_JWRN_JPEG_EOF, "Premature end of OE_JPEG file")
OE_JMESSAGE(OE_JWRN_MUST_RESYNC,
	 "Corrupt OE_JPEG data: found marker 0x%02x instead of RST%d")
OE_JMESSAGE(OE_JWRN_NOT_SEQUENTIAL, "Invalid SOS parameters for sequential OE_JPEG")
OE_JMESSAGE(OE_JWRN_TOO_MUCH_DATA, "Application transferred too many scanlines")

#ifdef OE_JMAKE_ENUM_LIST

  OE_JMSG_LASTMSGCODE
} OE_J_MESSAGE_CODE;

#undef OE_JMAKE_ENUM_LIST
#endif /* OE_JMAKE_ENUM_LIST */

/* Zap OE_JMESSAGE macro so that future re-inclusions do nothing by default */
#undef OE_JMESSAGE


#ifndef OE_JERROR_H
#define OE_JERROR_H

/* Macros to simplify using the error and trace message stuff */
/* The first parameter is either type of cinfo pointer */

/* Fatal errors (print message and exit) */
#define ERREXIT(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->error_exit) ((oe_j_common_ptr) (cinfo)))
#define ERREXIT1(cinfo,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->error_exit) ((oe_j_common_ptr) (cinfo)))
#define ERREXIT2(cinfo,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->error_exit) ((oe_j_common_ptr) (cinfo)))
#define ERREXIT3(cinfo,code,p1,p2,p3)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (cinfo)->err->msg_parm.i[2] = (p3), \
   (*(cinfo)->err->error_exit) ((oe_j_common_ptr) (cinfo)))
#define ERREXIT4(cinfo,code,p1,p2,p3,p4)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (cinfo)->err->msg_parm.i[2] = (p3), \
   (cinfo)->err->msg_parm.i[3] = (p4), \
   (*(cinfo)->err->error_exit) ((oe_j_common_ptr) (cinfo)))
#define ERREXITS(cinfo,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), OE_JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->error_exit) ((oe_j_common_ptr) (cinfo)))

#define MAKESTMT(stuff)		do { stuff } while (0)

/* Nonfatal errors (we can keep going, but the data is probably corrupt) */
#define WARNMS(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), -1))
#define WARNMS1(cinfo,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), -1))
#define WARNMS2(cinfo,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), -1))

/* Informational/debugging messages */
#define TRACEMS(cinfo,lvl,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)))
#define TRACEMS1(cinfo,lvl,code,p1)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)))
#define TRACEMS2(cinfo,lvl,code,p1,p2)  \
  ((cinfo)->err->msg_code = (code), \
   (cinfo)->err->msg_parm.i[0] = (p1), \
   (cinfo)->err->msg_parm.i[1] = (p2), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)))
#define TRACEMS3(cinfo,lvl,code,p1,p2,p3)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS4(cinfo,lvl,code,p1,p2,p3,p4)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS5(cinfo,lvl,code,p1,p2,p3,p4,p5)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   _mp[4] = (p5); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)); )
#define TRACEMS8(cinfo,lvl,code,p1,p2,p3,p4,p5,p6,p7,p8)  \
  MAKESTMT(int * _mp = (cinfo)->err->msg_parm.i; \
	   _mp[0] = (p1); _mp[1] = (p2); _mp[2] = (p3); _mp[3] = (p4); \
	   _mp[4] = (p5); _mp[5] = (p6); _mp[6] = (p7); _mp[7] = (p8); \
	   (cinfo)->err->msg_code = (code); \
	   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)); )
#define TRACEMSS(cinfo,lvl,code,str)  \
  ((cinfo)->err->msg_code = (code), \
   strncpy((cinfo)->err->msg_parm.s, (str), OE_JMSG_STR_PARM_MAX), \
   (*(cinfo)->err->emit_message) ((oe_j_common_ptr) (cinfo), (lvl)))

#endif /* OE_JERROR_H */
