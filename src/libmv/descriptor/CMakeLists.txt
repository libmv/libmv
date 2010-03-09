ADD_LIBRARY(descriptor daisy_descriptor.cc simpliest_descriptor.cc surf_descriptor.cc)
LIBMV_TEST(daisy_descriptor "descriptor;image;daisy")
