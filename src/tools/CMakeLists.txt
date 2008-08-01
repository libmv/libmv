ADD_EXECUTABLE(track track.cc)
TARGET_LINK_LIBRARIES(track image correspondence image flens blas lapack gflags)
