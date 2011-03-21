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
IF (NOT DOXYGEN_FOUND STREQUAL "NO" AND BUILD_DOC STREQUAL ON)
  # add doxygen as target
  ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_EXECUTABLE} ${LIBMV_SOURCE_DIR}/../Doxyfile)

  # cleanup $build/api-doc on "make clean"
  SET_PROPERTY(DIRECTORY APPEND PROPERTY
         ADDITIONAL_MAKE_CLEAN_FILES doc)

  # add doxygen as dependency to doc-target
  GET_TARGET_PROPERTY(DOC_TARGET doc TYPE)
  IF(NOT DOC_TARGET)
    ADD_CUSTOM_TARGET(doc)
  ENDIF()
  ADD_DEPENDENCIES(doc doxygen)

  # install HTML API documentation and manual pages
  SET(DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}-${VERSION}")

  INSTALL(DIRECTORY ${LIBMV_SOURCE_DIR}/../doc/html
           DESTINATION ${DOC_PATH}
         )

  # install man pages into packages, scope is now project root..
  INSTALL(DIRECTORY ${LIBMV_SOURCE_DIR}/../doc/man/man3
           DESTINATION share/man/man3/
         )
ENDIF(NOT DOXYGEN_FOUND STREQUAL "NO"  AND BUILD_DOC STREQUAL ON)