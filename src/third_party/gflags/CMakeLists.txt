IF(WIN32)
  INCLUDE_DIRECTORIES(
    ../msinttypes
    ../pthreads-w32/include
  )
ENDIF(WIN32)

ADD_LIBRARY(gflags gflags.cc gflags_reporting.cc)

