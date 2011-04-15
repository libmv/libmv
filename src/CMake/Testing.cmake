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

OPTION(BUILD_TESTS "Build the unit tests." ON)
MACRO (LIBMV_TEST NAME EXTRA_LIBS)
  IF (BUILD_TESTS)
    ADD_EXECUTABLE(${NAME}_test ${NAME}_test.cc)
    TARGET_LINK_LIBRARIES(${NAME}_test
                          ${EXTRA_LIBS} # Extra libs MUST be first.
                          libmv_test_main 
                          gtest 
                          glog
                          gflags
                          pthread)
     SET_TARGET_PROPERTIES(${NAME}_test PROPERTIES 
       RUNTIME_OUTPUT_DIRECTORY         ${LIBMV_TESTS_OUTPUT_DIR}
       RUNTIME_OUTPUT_DIRECTORY_RELEASE ${LIBMV_TESTS_OUTPUT_DIR}
       RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${LIBMV_TESTS_OUTPUT_DIR})
    ADD_TEST(${NAME}_test ${LIBMV_TESTS_OUTPUT_DIR}/${NAME}_test)
  ENDIF (BUILD_TESTS)
ENDMACRO (LIBMV_TEST)