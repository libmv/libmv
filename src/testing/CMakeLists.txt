ADD_LIBRARY(libmv_test_main testing_main.cc)
# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(libmv_test_main PROPERTIES DEBUG_POSTFIX "_d")
