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

LIBMV_TEST(tinyvector "")
