ADD_SUBDIRECTORY(colamd)
ADD_SUBDIRECTORY(glog)
ADD_SUBDIRECTORY(gtest)
ADD_SUBDIRECTORY(gflags)
ADD_SUBDIRECTORY(ldl)
ADD_SUBDIRECTORY(ssba)
# TODO(keir): Add Eigen2's cmake stuff once TAUCS or SuperLU is integrated.

# Inactive dependencies for future
IF(WIN32)
  ADD_SUBDIRECTORY(jpeg-7)
  ADD_SUBDIRECTORY(zlib)
  ADD_SUBDIRECTORY(png)
ENDIF(WIN32)
