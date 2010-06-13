# TODO(keir): Update this with the new API.
ADD_EXECUTABLE(track track.cc)
TARGET_LINK_LIBRARIES(track image correspondence image gflags glog)


ADD_EXECUTABLE(interest_points interest_points.cc)
TARGET_LINK_LIBRARIES(interest_points
                      numeric
                      correspondence
                      image
                      pthread
                      gflags
                      glog
                      detector
                      fast
                      )

ADD_EXECUTABLE(experimental experimental.cc)
TARGET_LINK_LIBRARIES(experimental
                      multiview_test_data
                      multiview
                      numeric
                      pthread
                      gflags
                      glog
                      )

ADD_EXECUTABLE(points_detector points_detector.cc)
TARGET_LINK_LIBRARIES(points_detector
                      correspondence
                      image
                      gflags
                      glog
                      detector
                      fast
                      descriptor
                      )

ADD_EXECUTABLE(homography_warping homography_warping.cc)
TARGET_LINK_LIBRARIES(homography_warping
                      correspondence
                      image
                      gflags
                      glog
                      detector
                      fast
                      flann
                      descriptor
                      multiview
                      )

ADD_EXECUTABLE(detector_repeatability detector_repeatability.cc)
TARGET_LINK_LIBRARIES(detector_repeatability
                      correspondence
                      image
                      gflags
                      glog
                      detector
                      fast
                      flann
                      descriptor
                      multiview
                      )
                      
ADD_EXECUTABLE(extract_exif_data extractExifData.cc)
TARGET_LINK_LIBRARIES(extract_exif_data
                      OpenExif
                      tools
                      )

ADD_EXECUTABLE(tracker tracker.cc)
TARGET_LINK_LIBRARIES(tracker
                      image
                      correspondence
                      numeric
                      multiview
                      flann
                      glog
                      gflags
                      detector
                      descriptor
                      fast
                      daisy
                      )
