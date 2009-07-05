IF(WIN32)
  INCLUDE_DIRECTORIES(
    windows   # for unistd.h implementation
  )
ENDIF(WIN32)

IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused")
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

ADD_LIBRARY(glog
            demangle.cc
            logging.cc
            raw_logging.cc
            signalhandler.cc
            symbolize.cc
            utilities.cc
            vlog_is_on.cc
)

MACRO (GLOG_TEST NAME)
  ADD_EXECUTABLE(${NAME} ${NAME}.cc googletest.cc)
  TARGET_LINK_LIBRARIES(${NAME} glog gflags pthread)
  ADD_TEST(${NAME} ${NAME})
ENDMACRO (GLOG_TEST)

GLOG_TEST(demangle_unittest)
# These require extra magic...
#GLOG_TEST(logging_striptest10)
#GLOG_TEST(logging_striptest2)
# I don't think logging_unittest can really be called a unittest, because it
# benchmarks the logging. Commented out to keep tests fast.
#GLOG_TEST(logging_unittest)
GLOG_TEST(signalhandler_unittest)
GLOG_TEST(stacktrace_unittest)
GLOG_TEST(stl_logging_unittest)
GLOG_TEST(symbolize_unittest)
GLOG_TEST(utilities_unittest)
