ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

ADD_LIBRARY(image image.cc image_io.cc convolve.cc image_pyramid.cc array_nd.cc
            image_sequence.cc image_sequence_io.cc image_sequence_filters.cc
            filtered_sequence.cc pyramid_sequence.cc)

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
