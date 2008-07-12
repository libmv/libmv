ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${LIBMV_SOURCE_DIR}/libmv/image\\")

ADD_LIBRARY(image image.cc image_io.cc convolve.cc)

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

ADD_EXECUTABLE(lru_cache_test lru_cache_test.cc)
TARGET_LINK_LIBRARIES(lru_cache_test gtest)
ADD_TEST(lru_cache_test lru_cache_test)

