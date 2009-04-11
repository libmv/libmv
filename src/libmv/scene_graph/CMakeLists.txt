ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

ADD_LIBRARY(scene_graph scene_graph.cc)

TARGET_LINK_LIBRARIES(scene_graph numeric glog gflags pthread)

LIBMV_TEST(scene_graph scene_graph)
