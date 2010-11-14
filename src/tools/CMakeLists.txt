# TODO(keir): Update this with the new API.
ADD_EXECUTABLE(track track.cc)
TARGET_LINK_LIBRARIES(track image correspondence image gflags glog)
LIBMV_INSTALL_EXE(track)


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
LIBMV_INSTALL_EXE(interest_points)

ADD_EXECUTABLE(experimental experimental.cc)
TARGET_LINK_LIBRARIES(experimental
                      multiview_test_data
                      multiview
                      numeric
                      pthread
                      gflags
                      glog
                      )
LIBMV_INSTALL_EXE(experimental)

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
LIBMV_INSTALL_EXE(points_detector)

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
LIBMV_INSTALL_EXE(homography_warping)

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
LIBMV_INSTALL_EXE(detector_repeatability)
                      
ADD_EXECUTABLE(extract_exif_data extractExifData.cc)
TARGET_LINK_LIBRARIES(extract_exif_data
                      OpenExif
                      tools
                      )
LIBMV_INSTALL_EXE(extract_exif_data)

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
LIBMV_INSTALL_EXE(tracker)

ADD_EXECUTABLE(nViewMatching nViewMatching.cc)
TARGET_LINK_LIBRARIES(nViewMatching
                      correspondence
                      image
                      gflags
                      glog
                      detector
                      fast
                      flann
                      descriptor
                      multiview
                      daisy
                      )
LIBMV_INSTALL_EXE(nViewMatching)