# Copyright (c) 2007, 2008, 2009 libmv authors.
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

CMAKE_MINIMUM_REQUIRED(VERSION 2.2)

IF (COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
  cmake_policy(SET CMP0005 OLD)
ENDIF (COMMAND cmake_policy)

PROJECT(LIBMV C CXX)

ENABLE_TESTING()

set(CMAKE_MODULE_PATH  
  "${LIBMV_SOURCE_DIR}/CMake"
  "${LIBMV_SOURCE_DIR}/CMake/modules")

INCLUDE(LibmvVersion)

set(LIBMV_SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})
set(LIBMV_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
SET(LIBMV_BINARY_DIR  ${PROJECT_BINARY_DIR}/..)
SET(LIBMV_LIBRARY_DIR ${LIBMV_BINARY_DIR}/lib)

set(LIBMV_HEADERS_OUTPUT_DIR    ${CMAKE_INSTALL_PREFIX}/include/libmv)
set(LIBMV_LIBRARY_OUTPUT_DIR    ${CMAKE_INSTALL_PREFIX}/lib)
set(LIBMV_EXECUTABLE_OUTPUT_DIR ${CMAKE_INSTALL_PREFIX}/bin)

IF(UNIX)
  SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBMV_LIBRARY_DIR})
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBMV_LIBRARY_DIR})
ENDIF(UNIX)

IF(MSVC)
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${LIBMV_LIBRARY_DIR})
  SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIBMV_LIBRARY_DIR})
ENDIF(MSVC)
# SET(LIBRARY_OUTPUT_PATH    ${LIBMV_LIBRARY_OUTPUT_DIR})
# SET(EXECUTABLE_OUTPUT_PATH ${LIBMV_EXECUTABLE_OUTPUT_DIR})

# To build shared libs
#SET(BUILD_SHARED_LIBS ON)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIBMV_LIBRARY_OUTPUT_DIR})


INCLUDE(ConfigureBuild)

INCLUDE_DIRECTORIES(
  .
  third_party/daisy/include
  third_party/gtest
  third_party/gtest/include
  third_party/eigen
  third_party/ssba
  third_party/glog/src
  third_party/OpenExif/src
  third_party/OpenExif/src/OpenExifJpeg
)

IF (WIN32)
  INCLUDE_DIRECTORIES(
    third_party/msinttypes
    third_party/jpeg-7
    third_party/zlib
    third_party/pthreads-w32/include
    third_party/png)
ENDIF (WIN32)

IF (APPLE)
  INCLUDE_DIRECTORIES(
    third_party/png
    third_party/jpeg-7
    ${LIBMV_BINARY_DIR}/third_party/jpeg-7
  )
ENDIF (APPLE)

IF (UNIX)
  INCLUDE_DIRECTORIES(
    third_party/png
    third_party/zlib)
ENDIF (UNIX)


# INCLUDE(Documentation)
INCLUDE(Installation)
INCLUDE(Packaging)
INCLUDE(Testing)

ADD_SUBDIRECTORY(libmv)
ADD_SUBDIRECTORY(testing)
ADD_SUBDIRECTORY(third_party)

IF (QT4_FOUND)
  MESSAGE(STATUS "Qt4 found: GUI enabled.")
  ADD_SUBDIRECTORY(ui)
ELSE (QT4_FOUND)
  MESSAGE(STATUS "Qt4 not found: GUI disabled.")
  MESSAGE(STATUS "If you have installed Qt4, check that qmake.exe is accessible\
 from your PATH.")
ENDIF (QT4_FOUND)

ADD_SUBDIRECTORY(tools)
