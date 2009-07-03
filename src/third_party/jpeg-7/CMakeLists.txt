CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(libjpeg)

SET(LIBJPEG_SYSDEP jmemnobs.c)

SET(LIBJPEG_COMMON 
  jaricom.c jcomapi.c jutils.c jerror.c jmemmgr.c
)

SET(LIBJPEG_CLIB
  jcapimin.c jcapistd.c jcarith.c jctrans.c jcparam.c 
  jdatadst.c jcinit.c jcmaster.c jcmarker.c jcmainct.c 
  jcprepct.c jccoefct.c jccolor.c jcsample.c jchuff.c 
  jcdctmgr.c jfdctfst.c jfdctflt.c jfdctint.c
)

SET(LIBJPEG_DLIB
  jdapimin.c jdapistd.c jdarith.c jdtrans.c jdatasrc.c 
  jdmaster.c jdinput.c jdmarker.c jdhuff.c jdmainct.c 
  jdcoefct.c jdpostct.c jddctmgr.c jidctfst.c jidctflt.c 
  jidctint.c jdsample.c jdcolor.c jquant1.c jquant2.c 
  jdmerge.c
)

IF(WIN32)
  CONFIGURE_FILE(${libjpeg_SOURCE_DIR}/jconfig.vc
                 ${PROJECT_BINARY_DIR}/jconfig.h)
  ADD_DEFINITIONS( -DJPEGSTATIC -DUSE_WINDOWS_MESSAGEBOX )
ENDIF(WIN32)

IF(APPLE)
  CONFIGURE_FILE(${libjpeg_SOURCE_DIR}/jconfig.mac
                 ${PROJECT_BINARY_DIR}/jconfig.h)
ENDIF(APPLE)

INCLUDE_DIRECTORIES(
  ${libjpeg_BINARY_DIR} 
  # Platform-dependent jconfig.h is copied there, to avoid modifying source tree
  # all libjpeg dependent libs would need to include this dir
)

FILE(GLOB LIBJPEG_H *.h )

SET(LIBJPEG_NAME jpeg)
IF(WIN32)
  SET(LIBJPEG_NAME libjpeg)
ENDIF(WIN32)

ADD_LIBRARY(${LIBJPEG_NAME} ${LIBJPEG_SYSDEP} ${LIBJPEG_COMMON} ${LIBJPEG_CLIB} ${LIBJPEG_DLIB} ${LIBJPEG_H} )

