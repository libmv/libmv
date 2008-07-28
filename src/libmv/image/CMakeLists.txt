ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${LIBMV_SOURCE_DIR}/libmv/image\\")

ADD_LIBRARY(image image.cc image_io.cc convolve.cc image_pyramid.cc array_nd.cc)

ADD_EXECUTABLE(tuple_test tuple_test.cc)
TARGET_LINK_LIBRARIES(tuple_test gtest)
ADD_TEST(tuple_test tuple_test)

ADD_EXECUTABLE(array_nd_test array_nd_test.cc)
TARGET_LINK_LIBRARIES(array_nd_test gtest)
ADD_TEST(array_nd_test array_nd_test)

ADD_EXECUTABLE(image_test image_test.cc)
TARGET_LINK_LIBRARIES(image_test image gtest)
ADD_TEST(image_test image_test)

ADD_EXECUTABLE(image_io_test image_io_test.cc)
TARGET_LINK_LIBRARIES(image_io_test image gtest)
ADD_TEST(image_io_test image_io_test)

ADD_EXECUTABLE(convolve_test convolve_test.cc)
TARGET_LINK_LIBRARIES(convolve_test image flens gtest lapack atlas)
ADD_TEST(convolve_test convolve_test)

ADD_EXECUTABLE(image_pyramid_test image_pyramid_test.cc)
TARGET_LINK_LIBRARIES(image_pyramid_test image flens gtest lapack atlas)
ADD_TEST(image_pyramid_test image_pyramid_test)

ADD_EXECUTABLE(sample_test sample_test.cc)
TARGET_LINK_LIBRARIES(sample_test image flens gtest lapack atlas)
ADD_TEST(sample_test sample_test)

ADD_EXECUTABLE(lru_cache_test lru_cache_test.cc)
TARGET_LINK_LIBRARIES(lru_cache_test gtest)
ADD_TEST(lru_cache_test lru_cache_test)

