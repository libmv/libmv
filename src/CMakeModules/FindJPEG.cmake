# - Find JPEG
# Find the native JPEG includes and library
# This module defines
#  JPEG_INCLUDE_DIR, where to find jpeglib.h, etc.
#  JPEG_LIBRARIES, the libraries needed to use JPEG.
#  JPEG_FOUND, If false, do not try to use JPEG.
# also defined, but not for general use are
#  JPEG_LIBRARY, where to find the JPEG library.

FIND_PATH(JPEG_INCLUDE_DIR jpeglib.h)

SET(JPEG_NAMES_RELEASE ${JPEG_NAMES_RELEASE} ${JPEG_NAMES} jpeg libjpeg)
FIND_LIBRARY(JPEG_LIBRARY_RELEASE NAMES ${JPEG_NAMES_RELEASE} )

SET(JPEG_NAMES_DEBUG ${JPEG_NAMES_DEBUG} jpegd libjpegd jpeg_d libjpeg_d)
FIND_LIBRARY(JPEG_LIBRARY_DEBUG NAMES ${JPEG_NAMES_DEBUG} )

IF(JPEG_FOUND)
  SET(JPEG_LIBRARIES ${JPEG_LIBRARY})
ENDIF(JPEG_FOUND)

