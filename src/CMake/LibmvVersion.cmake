# Copyright (c) 2007-2011 libmv authors.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

SET(LIBMV_VERSION_MAJOR 0)
SET(LIBMV_VERSION_MINOR 1)
SET(LIBMV_VERSION_PATCH 0) #or the svn revision

# The standard GNU version numbering scheme is major.minor.revision
IF (EXISTS ${LIBMV_SOURCE_DIR}/.svn)
  # Probably a SVN workspace, determine revision level 
  FIND_PACKAGE(Subversion)
  IF(Subversion_FOUND)
    Subversion_WC_INFO(${LIBMV_SOURCE_DIR} ER)
    SET(SUBVERSION_REVISION ${ER_WC_REVISION})
    SET(LIBMV_VERSION_PATCH ${SUBVERSION_REVISION})
  ENDIF(Subversion_FOUND)
ENDIF (EXISTS ${LIBMV_SOURCE_DIR}/.svn)

# Built from svn
SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}.${LIBMV_VERSION_PATCH}")
# Alpha version [+number]
# SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}-a")
# Beta version [+number]
# SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}-b")
# Release candidate version [+number]
# SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}-rc")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/CMake/revision.h.in ${CMAKE_CURRENT_SOURCE_DIR}/libmv/tools/revision.h)