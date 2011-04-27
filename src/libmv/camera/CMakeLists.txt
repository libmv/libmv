# define the source files
SET(CAMERA_SRC camera.cc
               pinhole_camera.cc
               lens_distortion.cc)
               
# define the header files (make the headers appear in IDEs.)
FILE(GLOB CAMERA_HDRS *.h)

ADD_LIBRARY(camera ${CAMERA_SRC} ${CAMERA_HDRS})

TARGET_LINK_LIBRARIES(camera multiview numeric V3D colamd ldl)

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(camera PROPERTIES DEBUG_POSTFIX "_d")

# installation rules for the library
LIBMV_INSTALL_LIB(camera)

MACRO (CAMERA_TEST NAME)
  LIBMV_TEST(${NAME} "multiview_test_data;correspondence;camera;multiview;numeric")
ENDMACRO (CAMERA_TEST)

CAMERA_TEST(pinhole_camera)
CAMERA_TEST(lens_distortion)

