include_directories(${V3D_INCLUDE_DIRS} ${EXTRA_INC_DIRS})
link_directories(${V3D_DIR} ${EXTRA_LIB_DIRS})
link_libraries (V3D ${EXTRA_LIBRARIES} glut)

if (V3DLIB_ENABLE_GPGPU)
  add_subdirectory(GL)
endif (V3DLIB_ENABLE_GPGPU)

add_v3d_executable (bundle_common bundle_common.cpp)
add_v3d_executable (bundle_varying bundle_varying.cpp)
