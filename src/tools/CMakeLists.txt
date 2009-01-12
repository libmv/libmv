ADD_EXECUTABLE(track track.cc)
TARGET_LINK_LIBRARIES(track image correspondence image gflags glog)

ADD_EXECUTABLE(experimental experimental.cc)
TARGET_LINK_LIBRARIES(experimental
                      multiview_test_data
                      multiview
                      numeric
                      pthread
                      gflags
                      glog
                      )
