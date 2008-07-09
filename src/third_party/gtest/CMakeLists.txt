
INCLUDE_DIRECTORIES( include
	             include/gtest/internal
		     .
)

ADD_LIBRARY( gtest
             src/gtest.cc
	     src/gtest-death-test.cc
	     src/gtest-filepath.cc
	     src/gtest_main.cc
	     src/gtest-port.cc
)

