ADD_LIBRARY(reconstruction
            euclidean_reconstruction.cc
            export_blender.cc
            export_ply.cc
            image_selection.cc
            keyframe_selection.cc
            mapping.cc
            optimization.cc
            projective_reconstruction.cc
            reconstruction.cc
            tools.cc
            )

TARGET_LINK_LIBRARIES(reconstruction camera multiview numeric V3D colamd ldl glog)
LIBMV_INSTALL_LIB(reconstruction)

MACRO (RECONSTRUCTION_TEST NAME)
  LIBMV_TEST(${NAME} "reconstruction;multiview_test_data;camera;correspondence;multiview;numeric;glog")
ENDMACRO (RECONSTRUCTION_TEST)

RECONSTRUCTION_TEST(euclidean_reconstruction)