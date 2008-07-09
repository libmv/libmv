
ADD_LIBRARY(flens
            blas.cc
	    complex_helper.cc
            id.cc
            lapack.cc
            range.cc
            refcounter.cc
            snapshot.cc
            sparse_blas.cc
            underscore.cc
)

ADD_EXECUTABLE(tinyvector_test tinyvector_test.cc)
TARGET_LINK_LIBRARIES(tinyvector_test flens gtest lapack atlas)
ADD_TEST(tinyvector_test tinyvector_test)
	              
