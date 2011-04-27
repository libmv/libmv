# define the source files
SET(DETECTOR_SRC fast_detector.cc
                 surf_detector.cc
                 star_detector.cc
                 fast_detector_limited.cc
                 mser_detector.cc
                 detector_factory.cc)
               
# define the header files (make the headers appear in IDEs.)
FILE(GLOB DETECTOR_HDRS *.h)

ADD_LIBRARY(detector ${DETECTOR_SRC} ${DETECTOR_HDRS})

TARGET_LINK_LIBRARIES(detector)

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(detector PROPERTIES DEBUG_POSTFIX "_d")

LIBMV_INSTALL_LIB(detector)
LIBMV_TEST(fast_detector "detector;image;correspondence;fast")
LIBMV_TEST(orientation_detector "detector;image;correspondence")
