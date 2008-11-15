ADD_LIBRARY(numeric numeric.cc)
ADD_LIBRARY(tinyvector tinyvector.cc)
LIBMV_TEST(numeric numeric)
LIBMV_TEST(tinyvector "")
