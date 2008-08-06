
ADD_LIBRARY(numeric numeric.cc)

ADD_EXECUTABLE(numeric_test numeric_test.cc)
TARGET_LINK_LIBRARIES(numeric_test numeric flens gtest lapack atlas)
ADD_TEST(numeric_test numeric_test)

