# Copyright (c) 2007, 2008 libmv authors.
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

# -Wno-deprecated needed for gcc-4.3 on debian/testing;
# needed due to flens/refcounter.h including deprecated hash_map

IF(WIN32)
  SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "-Wall -W -Wfatal-errors -Werror -Wno-deprecated -Wno-sign-compare -Wno-strict-aliasing")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

SET(CMAKE_MODULE_PATH ${LIBMV_SOURCE_DIR}/CMakeModules)
MESSAGE("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")
IF (NOT CMAKE_MODULE_PATH)
  MESSAGE(FATAL_ERROR
          "ERROR: export CMAKE_MODULE_PATH as your CMakeModules repository.")
ELSE (NOT CMAKE_MODULE_PATH)
  # Check for one of the find scripts for sanity.
  FIND_FILE(LIBMV_CMAKE_MODULE_PATH_OK FindJPEG.cmake ${CMAKE_MODULE_PATH}
            DOC "testing CMAKE_MODULE_PATH")
  IF (NOT LIBMV_CMAKE_MODULE_PATH_OK)
    MESSAGE(FATAL_ERROR "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} invalid: FindJPEG.cmake not found.")
  ENDIF (NOT LIBMV_CMAKE_MODULE_PATH_OK)
ENDIF (NOT CMAKE_MODULE_PATH)
MARK_AS_ADVANCED(LIBMV_CMAKE_MODULE_PATH_OK)

INCLUDE_DIRECTORIES(
  third_party/gtest
  third_party/glog/src
  third_party/gtest/include
  third_party/eigen
  third_party/ssba
  third_party
  ${LIBMV_SOURCE_DIR}
  ${LIBMV_BINARY_DIR}
)

IF (WIN32)
  INCLUDE_DIRECTORIES(
    third_party/pthreads-w32/include
    third_party/msinttypes
  )
ENDIF (WIN32)

IF (APPLE)
  # Tell gtest not to use mac framworks.
  ADD_DEFINITIONS('-DGTEST_NOT_MAC_FRAMEWORK_MODE')
ENDIF (APPLE)

MACRO (ADD_GENERATED_SOURCE generated_file python_generator)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${generated_file}
       COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/${python_generator}
       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${python_generator})
ENDMACRO (ADD_GENERATED_SOURCE)

MACRO (LIBMV_TEST NAME EXTRA_LIBS)
  ADD_EXECUTABLE(${NAME}_test ${NAME}_test.cc)
  TARGET_LINK_LIBRARIES(${NAME}_test
                        ${EXTRA_LIBS} # Extra libs MUST be first.
                        gtest 
                        gflags
                        glog
                        pthread)
  ADD_TEST(${NAME}_test ${NAME}_test)
ENDMACRO (LIBMV_TEST)

ADD_SUBDIRECTORY(libmv)
ADD_SUBDIRECTORY(testing)
ADD_SUBDIRECTORY(third_party)
ADD_SUBDIRECTORY(ui)
ADD_SUBDIRECTORY(tools)
