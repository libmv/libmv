ADD_LIBRARY(camera
            camera.cc
            pinhole_camera.cc
            lens_distortion.cc
            )

TARGET_LINK_LIBRARIES(camera multiview numeric V3D colamd ldl)
LIBMV_INSTALL_LIB(camera)

MACRO (CAMERA_TEST NAME)
  LIBMV_TEST(${NAME} "multiview_test_data;correspondence;camera;multiview;numeric")
ENDMACRO (CAMERA_TEST)

CAMERA_TEST(camera)
CAMERA_TEST(lens_distortion)

