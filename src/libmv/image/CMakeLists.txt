ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

IF(WIN32)
  # TODO: Remove next line completely when libjpeg included into CMake, instead add ${libjpeg_BINARY_DIR} to INCLUDE_DIRECTORIES
  CONFIGURE_FILE(../../third_party/jpeg-7/jconfig.vc ${PROJECT_BINARY_DIR}/jconfig.h) 

  INCLUDE_DIRECTORIES(
    # TODO: replace next 2 lines with '${libjpeg_SOURCE_DIR}' and '${libjpeg_BINARY_DIR}' when libjpeg is included into build
    # Platform-specific jconfig.h is copied into ${libjpeg_BINARY_DIR} during make
      ../../third_party/jpeg-7  
      ${PROJECT_BINARY_DIR} 
    # TODO: replace next line with '${png_SOURCE_DIR}' when png is included into build (maybe PNG_SOURCE_DIR, not sure about caps)
      ../../third_party/png
    # TODO: replace next line with '${zlib_SOURCE_DIR}' when zlib is included
      ../../third_party/zlib
  )
ENDIF(WIN32)

ADD_LIBRARY(image image.cc image_io.cc convolve.cc image_pyramid.cc array_nd.cc
            image_sequence.cc image_sequence_io.cc image_sequence_filters.cc
            filtered_sequence.cc pyramid_sequence.cc)

TARGET_LINK_LIBRARIES(image png jpeg glog gflags pthread)

MACRO (IMAGE_TEST NAME)
  LIBMV_TEST(${NAME} image)
ENDMACRO (IMAGE_TEST)

IMAGE_TEST(array_nd)
IMAGE_TEST(blob_response)
IMAGE_TEST(convolve)
IMAGE_TEST(derivative)
IMAGE_TEST(filtered_sequence)
IMAGE_TEST(image)
IMAGE_TEST(image_io)
IMAGE_TEST(image_pyramid)
IMAGE_TEST(image_sequence_filters)
IMAGE_TEST(image_sequence_io)
IMAGE_TEST(integral_image)
IMAGE_TEST(lru_cache)
IMAGE_TEST(non_maximal_suppression)
IMAGE_TEST(pyramid_sequence)
IMAGE_TEST(sample)
IMAGE_TEST(surf)
IMAGE_TEST(tuple)
