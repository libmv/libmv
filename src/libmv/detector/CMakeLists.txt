ADD_LIBRARY(detector fast_detector.cc surf_detector.cc star_detector.cc)
LIBMV_TEST(fast_detector "detector;image;correspondence;fast")
LIBMV_TEST(orientation_detector "detector;image;correspondence")
