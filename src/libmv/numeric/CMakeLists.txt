# define the source files
SET(NUMERIC_SRC numeric.cc 
                poly.cc)
               
# define the header files (make the headers appear in IDEs.)
FILE(GLOB NUMERIC_HDRS *.h)

ADD_LIBRARY(numeric ${NUMERIC_SRC} ${NUMERIC_HDRS})

TARGET_LINK_LIBRARIES(numeric)

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(numeric PROPERTIES DEBUG_POSTFIX "_d")

# installation rules for the library
LIBMV_INSTALL_LIB(numeric)

LIBMV_TEST(numeric numeric)
LIBMV_TEST(tinyvector numeric)
LIBMV_TEST(poly numeric)
LIBMV_TEST(function_derivative numeric)
LIBMV_TEST(levenberg_marquardt numeric)
LIBMV_TEST(dogleg numeric)
