ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

ADD_LIBRARY(multiview fundamental.cc focal_from_fundamental.cc)

ADD_EXECUTABLE(fundamental_test fundamental_test.cc generated.cc)
TARGET_LINK_LIBRARIES(fundamental_test multiview numeric
                      flens gtest lapack atlas)
ADD_TEST(fundamental_test fundamental_test)

ADD_EXECUTABLE(focal_from_fundamental_test focal_from_fundamental_test.cc)
TARGET_LINK_LIBRARIES(focal_from_fundamental_test multiview numeric
                      flens gtest lapack atlas)
ADD_TEST(focal_from_fundamental_test focal_from_fundamental_test)

ADD_GENERATED_SOURCE(generated.cc generator.py)

