
ADD_DEFINITIONS(
  -DTHIS_SOURCE_DIR=\\"${LIBMV_SOURCE_DIR}/libmv/multiview\\"
)

ADD_LIBRARY(multiview fundamental.cc)

ADD_EXECUTABLE(fundamental_test fundamental_test.cc generated.cc)
TARGET_LINK_LIBRARIES(fundamental_test multiview numeric
                      flens gtest lapack atlas)
ADD_TEST(fundamental_test fundamental_test)

ADD_GENERATED_SOURCE(generated.cc generator.py)

