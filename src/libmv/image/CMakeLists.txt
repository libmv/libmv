# TODO(julien) remove this line, what's that for?
ADD_DEFINITIONS(-DTHIS_SOURCE_DIR="\\"${CMAKE_CURRENT_SOURCE_DIR}\\"")

IF(WIN32)
  INCLUDE_DIRECTORIES(
    ${JPEG_SOURCE_DIR}
    ${PNG_SOURCE_DIR}
    ${ZLIB_SOURCE_DIR}
  )
ENDIF(WIN32)

# define the source files
SET(IMAGE_SRC image.cc image_io.cc convolve.cc image_pyramid.cc array_nd.cc
              image_sequence.cc image_sequence_io.cc image_sequence_filters.cc
              filtered_sequence.cc pyramid_sequence.cc image_transform_linear.cc)
               
# define the header files (make the headers appear in IDEs.)
FILE(GLOB IMAGE_HDRS *.h)

ADD_LIBRARY(image ${IMAGE_SRC} ${IMAGE_HDRS})

TARGET_LINK_LIBRARIES(image png jpeg glog gflags ${PTHREAD})

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(image PROPERTIES DEBUG_POSTFIX "_d")

# installation rules for the library
LIBMV_INSTALL_LIB(image)

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
IMAGE_TEST(image_drawing)
IMAGE_TEST(image_converter)
IMAGE_TEST(image_transform_linear)
IMAGE_TEST(integral_image)
IMAGE_TEST(lru_cache)
IMAGE_TEST(non_maximal_suppression)
IMAGE_TEST(pyramid_sequence)
IMAGE_TEST(sample)
IMAGE_TEST(surf)
IMAGE_TEST(tuple)
