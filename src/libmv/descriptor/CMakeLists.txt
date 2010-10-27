ADD_LIBRARY(descriptor
			daisy_descriptor.cc
			simpliest_descriptor.cc
			surf_descriptor.cc
			dipole_descriptor.cc
			descriptor_factory.cc)
LIBMV_INSTALL_LIB(descriptor)
LIBMV_TEST(daisy_descriptor "descriptor;image;daisy")
