
ADD_LIBRARY( gtest
             src/gtest.cc
             src/gtest-death-test.cc
             src/gtest-filepath.cc
             src/gtest-port.cc
             src/gtest-printers.cc
             src/gtest-test-part.cc
             src/gtest-typed-test.cc
)

LIBMV_INSTALL_THIRD_PARTY_LIB(gtest)
