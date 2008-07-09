
ADD_LIBRARY(correspondence klt.cc)
TARGET_LINK_LIBRARIES(correspondence lapack atlas)

ADD_EXECUTABLE(klt_test klt_test.cc)
TARGET_LINK_LIBRARIES(klt_test correspondence image flens gtest lapack atlas)
ADD_TEST(klt_test klt_test)

