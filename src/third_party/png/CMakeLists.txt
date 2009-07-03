PROJECT(png)

# source files for png
SET(PNG_SRCS
	pngget.c  pngrio.c  pngwrite.c  png.c  pngmem.c  pngrtran.c  pngtrans.c  pngwtran.c
	pngerror.c  pngpread.c  pngrutil.c  pngvcrd.c  pngwutil.c  pnggccrd.c  pngread.c
	pngset.c  pngwio.c  pngconf.h  png.h
)

IF(WIN32)
  ADD_DEFINITIONS(-DPNG_STATIC)
ENDIF(WIN32)

ADD_LIBRARY(png ${PNG_SRCS} )

TARGET_LINK_LIBRARIES(png zlib)

INCLUDE_DIRECTORIES(
  ${ZLIB_SOURCE_DIR}
)
