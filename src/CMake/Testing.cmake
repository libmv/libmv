
MACRO (LIBMV_TEST NAME EXTRA_LIBS)
  ADD_EXECUTABLE(${NAME}_test ${NAME}_test.cc)
  TARGET_LINK_LIBRARIES(${NAME}_test
                        ${EXTRA_LIBS} # Extra libs MUST be first.
                        libmv_test_main 
                        gtest 
                        glog
                        gflags
                        pthread)
  ADD_TEST(${NAME}_test ${NAME}_test)
ENDMACRO (LIBMV_TEST)