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

OPTION(BUILD_DOC "Build the documentation. (Doxygen needed)" OFF)

# check if doxygen is even installed
FIND_PACKAGE(Doxygen)
IF (DOXYGEN_FOUND AND BUILD_DOC)

  CONFIGURE_FILE(${LIBMV_SOURCE_DIR}/CMake/Doxyfile.cmake
                 ${PROJECT_BINARY_DIR}/Doxyfile
                 )
  # add doxygen as target
  ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)

  # cleanup $build/api-doc on "make clean"
  SET_PROPERTY(DIRECTORY APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES doc)

  # add doxygen as dependency to doc-target
  GET_TARGET_PROPERTY(DOC_TARGET doc TYPE)
  IF(NOT DOC_TARGET)
    ADD_CUSTOM_TARGET(doc)
  ENDIF()
  ADD_DEPENDENCIES(doc doxygen)
ENDIF(DOXYGEN_FOUND AND BUILD_DOC)

OPTION(INSTALL_DOCUMENTATION "Install the documentation" OFF)
IF (INSTALL_DOCUMENTATION)
# install HTML API documentation and manual pages
SET(DOC_PATH "${LIBMV_SHARE_OUTPUT_DIR}/../doc/libmv-${LIBMV_VERSION}")
INSTALL(DIRECTORY ${LIBMV_SOURCE_DIR}/../doc/doxygen/html
		DESTINATION ${DOC_PATH}
		COMPONENT documentation)
# install man pages into packages, scope is now project root..
#IF (UNIX)
#  INSTALL(DIRECTORY ${LIBMV_SOURCE_DIR}/../doc/man/man3
#          DESTINATION ${LIBMV_SHARE_OUTPUT_DIR}/../man/man3/
#          COMPONENT documentation)
#ENDIF (UNIX)
ENDIF (INSTALL_DOCUMENTATION)