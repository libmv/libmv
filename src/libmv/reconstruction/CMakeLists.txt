# define the source files
SET(RECONSTRUCTION_SRC euclidean_reconstruction.cc
              export_blender.cc
              export_ply.cc
              image_selection.cc
              keyframe_selection.cc
              mapping.cc
              optimization.cc
              projective_reconstruction.cc
              reconstruction.cc
              tools.cc)
               
# define the header files (make the headers appear in IDEs.)
FILE(GLOB RECONSTRUCTION_HDRS *.h)

ADD_LIBRARY(reconstruction ${RECONSTRUCTION_SRC} ${RECONSTRUCTION_HDRS})

TARGET_LINK_LIBRARIES(reconstruction camera multiview numeric V3D colamd ldl glog)

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(reconstruction PROPERTIES DEBUG_POSTFIX "_d")

# installation rules for the library
LIBMV_INSTALL_LIB(reconstruction)

MACRO (RECONSTRUCTION_TEST NAME)
  LIBMV_TEST(${NAME} "reconstruction;multiview_test_data;camera;correspondence;multiview;numeric;glog")
ENDMACRO (RECONSTRUCTION_TEST)

RECONSTRUCTION_TEST(euclidean_reconstruction)