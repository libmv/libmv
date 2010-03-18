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
 * Dan Graham dan.graham@kodak.com
 *
 * Contributor(s): 
 * Ricardo Rosario ricardo.rosario@kodak.com
 */ 


#ifndef EXIF_JPEG_WRAPPER_
#define EXIF_JPEG_WRAPPER_

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

#ifndef OPENEXIF_NO_IJG
#include "OpenExifJpegLib.h"
#else
typedef enum {OE_JDCT_ISLOW,OE_JDCT_IFAST,OE_JDCT_FLOAT} OE_J_DCT_METHOD;
#ifndef DCTSIZE2
#define DCTSIZE2 64
#endif
#endif

#include "ExifJpegExtn.h"
#include "ExifTypeDefs.h"
#include "ExifHuffmanTable.h"

class EXIF_DECL ExifJpegCompress
{

public:
    //! Default constructor
	ExifJpegCompress(void);
    
    //! Default constructor
	virtual ~ExifJpegCompress(void);

	JpegState getState(void) {return gJpegState;}

	JpegStatus startCompress(void);

	JpegStatus stopCompress(void);

	JpegStatus setFileOutput(FILE *fp);

	JpegStatus setBufferOutput(unsigned char *buf, exif_uint32 bufSize);

	JpegStatus setImageInfo(exif_uint32 width, exif_uint32 height,
						unsigned short numComps, ExifColorSpace cs);

	JpegStatus setInputColorSpace(ExifColorSpace inputCS)
    {
        gInputColorSpace = inputCS; 
        return JPEG_OK;
    }

	JpegStatus setJpegQuality(unsigned short qualityFactor) 
    {
        gQualityFactor = qualityFactor;
        return JPEG_OK;
    }
    
    // 1..100 (0 for no smoothing)
    JpegStatus setSmoothingFactor(unsigned short smoothingFactor)
    {
        gSmoothingFactor = smoothingFactor;
        return JPEG_OK;
    }
    
    // true or false
    JpegStatus setOptimization(bool optimize)
    {
        gOptimizeEncoding = optimize;
        return JPEG_OK;
    }
    
    // true or false
    JpegStatus setProgressive(bool progressive)
    {
        gProgressive = progressive;
        return JPEG_OK;
    }

	JpegStatus setJpegDCTMethod(OE_J_DCT_METHOD dctMethod) 
    {
        gDCTMethod = dctMethod;
        return JPEG_OK;
    }

	JpegStatus setJpegQuantTable(
				ExifJpegQTable*  Q0,
              	ExifJpegQTable*  Q1,
              	ExifJpegQTable*  Q2,
              	ExifJpegQTable*  Q3,
             	ExifJpegHUFFTable*   Huff_DC,
             	ExifJpegHUFFTable*   Huff_AC,
              	ExifJpegHUFFTable*   Huff_DC_Chroma = NULL,
              	ExifJpegHUFFTable*   Huff_AC_Chroma = NULL,
				ExifJpegTableOrder  tableOrder = JPEG_ZIG_ZAG); 

	JpegStatus setJpegQuantTable(
				unsigned char slot,
				unsigned short *qt,
				unsigned short qF = 0);


	JpegStatus writeScanLines(uint8 *buf, exif_uint32 bufLineStride, 
                                  exif_uint32 numLines, exif_uint32 *numLinesWritten,
                                  exif_uint32 *nextLine);

	exif_uint32 getCompSize(void);

	void writeApp0(void) {gWriteApp0 = true;}

	JpegStatus setICCProfile(unsigned char *iccBuf, exif_uint32 iccSize);

	void setDPI(unsigned short dpi_x, unsigned short dpi_y,
        unsigned short resUnit)
	{
            gXRes = dpi_x;
            gYRes = dpi_y;
            gResUnit = resUnit;
            gDpiSet = true;
        }

	//void setHSamplingFactor(uint8 componentID, uint8 value);
	//void setVSamplingFactor(uint8 componentID, uint8 value);
        void set422Sampling() { g422Sampling=true; } 
	
protected:
#endif
	JpegState gJpegState;
	unsigned short gQualityFactor;
    unsigned short gSmoothingFactor;
    bool gOptimizeEncoding;
    bool gProgressive;
	OE_J_DCT_METHOD gDCTMethod;
	unsigned char *gCompressBuf;
	exif_uint32 gCompressBufSize;
	FILE *gCompressFile;
	char *gCompressFileName;
	exif_uint32 gWidth;
	exif_uint32 gHeight;
	unsigned short gNumComps;
	exif_uint32 gLastLineWritten;
	ExifColorSpace gInputColorSpace;
	ExifColorSpace gJpegColorSpace;
	bool gWriteApp0;
	bool tableScaleFactorSet;
	unsigned short tableScaleFactor;
	bool g422Sampling;

	struct JPEGTableHolder
        {
            ExifJpegQTable* Q0;
            ExifJpegQTable* Q1;
            ExifJpegQTable*  Q2;
            ExifJpegQTable*  Q3;
            ExifJpegHUFFTable*   Huff_DC;
            ExifJpegHUFFTable*   Huff_AC;
            ExifJpegHUFFTable*   Huff_DC_Chroma;
            ExifJpegHUFFTable*   Huff_AC_Chroma;
            ~JPEGTableHolder() {   delete Q0;
                                                delete Q1;
                                                delete Q2;
                                                delete Q3;
                                                delete Huff_DC;
                                                delete Huff_AC;
                                                delete Huff_DC_Chroma;
                                                delete Huff_AC_Chroma;
                                                };
            JPEGTableHolder() 
              : Q0(NULL), Q1(NULL), Q2(NULL), Q3(NULL), Huff_DC(NULL),
                Huff_AC(NULL), Huff_DC_Chroma(NULL), Huff_AC_Chroma(NULL){};
        };

	JPEGTableHolder *gJpegTables;
	ExifJpegTableOrder jpegTableOrder;
	bool gUseCustomTables;

	JpegStatus setupCompress();

#ifndef OPENEXIF_NO_IJG
	// Used for IJG
	openexif_jpeg_compress_struct *gIJGCompInfo;
	struct my_error_mgr {
        /* "public" fields of a normal IJG error object. */
        struct openexif_jpeg_error_mgr pub;

        jmp_buf setjmp_buffer;  /* for return to caller */
        JpegStatus jpegStatus;
        // a buffer for IJG error messages.
        char jpegMessageBuffer[OE_JMSG_LENGTH_MAX];
    };

	my_error_mgr myErrorMgr;

	static void errorExitHandler(oe_j_common_ptr cinfo);
 
    static void emitMessageHandler(oe_j_common_ptr cinfo, int msgLevel);
#endif

	JpegStatus writeICCProfile(void);
	unsigned char* gICCProfBuf;
	exif_uint32 gICCProfSize;
	unsigned short gXRes;
	unsigned short gYRes;
	unsigned short gResUnit;
	bool gDpiSet;

};


class EXIF_DECL ExifJpegDecompress
{

public:
	ExifJpegDecompress(void);
    
	virtual ~ExifJpegDecompress(void);
    
	JpegState getState(void)
    {
        return gJpegState;
    }
    
	JpegStatus startDecompress(void);
    
	JpegStatus stopDecompress(void);
    
	JpegStatus resetDecompress(void);
    
	JpegStatus setFileInput(FILE *fp);
    
#ifdef WIN32
#ifdef INTERNET_REQUIRED
	JpegStatus setFileInput(HINTERNET internet);
#endif // INTERNET_REQUIRED
#endif // WIN32

	JpegStatus setBufferInput(unsigned char *buf, exif_uint32 bufSize);
    
	JpegStatus setOutputColorSpace(ExifColorSpace outputCS);
			
	ExifColorSpace getOutputColorSpace(void) { return gOutputColorSpace; }
	
	virtual JpegStatus getImageInfo(exif_uint32 *width, exif_uint32 *height,
						unsigned short *numComps);
	virtual	JpegStatus readScanLines(uint8 *readBuf, exif_uint32 numLines,
						exif_int32 lineStride,
						exif_uint32 *numRead, exif_uint32 *nextLine);
	

	JpegStatus setJpegDCTMethod(OE_J_DCT_METHOD dctMethod) 
    {
        gDCTMethod = dctMethod;
        return JPEG_OK;
    }
    
	JpegStatus setScaling(ExifJpegScaleFactor scaleFactor);
    
	JpegStatus setScaling(unsigned short scaleFactor);
    
    JpegStatus getJpegScaleSize(ExifJpegScaleFactor sF,
				exif_uint32 *width, exif_uint32* height);
                
	exif_uint32 outputWidth(void)
    {
        return gScaledWidth;
    }
    
	exif_uint32 outputHeight(void)
    {
        return gScaledHeight;
    }
    
	unsigned short outputComps(void)
    {
        return gNumComps;
    }
    
	unsigned short outputPrecision(void)
    {
        return gPrecision;
    }

	JpegStatus getJpegQuantTable(uint8 qIndex, uint16 *qt);

    uint8 getHSamplingFactor(uint8 componentID);
    
    uint8 getVSamplingFactor(uint8 componentID);

protected:
	JpegState gJpegState;
	OE_J_DCT_METHOD gDCTMethod;
	unsigned char *gDecompressBuf;
	exif_uint32 gDecompressBufSize;
	FILE *gDecompressFile;
	char *gDecompressFileName;
#ifdef WIN32
#ifdef INTERNET_REQUIRED
	HINTERNET gInternetHandle;
#endif // INTERNET_REQUIRED
#endif // WIN32
	exif_uint32 gWidth;
	exif_uint32 gHeight;
	exif_uint32 gScaledWidth;
	exif_uint32 gScaledHeight;
	uint16 gNumComps;
	uint16 gPrecision;

	bool gJpegHeaderRead;
	ExifJpegScaleFactor gScaleFactor;
	ExifColorSpace gOutputColorSpace;

#ifndef OPENEXIF_NO_IJG
	openexif_jpeg_decompress_struct *gIJGDecompInfo;

	JpegStatus setupDecompress(void);

	struct my_error_mgr {
        /* "public" fields of a normal IJG error object. */
        struct openexif_jpeg_error_mgr pub;

        jmp_buf setjmp_buffer;  /* for return to caller */
        JpegStatus jpegStatus;
        // a buffer for IJG error messages.
        char jpegMessageBuffer[OE_JMSG_LENGTH_MAX];
    };

	my_error_mgr myErrorMgr;

	static void errorExitHandler(oe_j_common_ptr cinfo);
 
    static void emitMessageHandler(oe_j_common_ptr cinfo, int msgLevel);
#endif

	exif_uint32 gLastLineRead;

};
