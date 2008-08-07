ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

ADD_LIBRARY(correspondence klt.cc feature.cc correspondence.cc)

ADD_EXECUTABLE(klt_test klt_test.cc)
TARGET_LINK_LIBRARIES(klt_test correspondence image flens gtest lapack atlas)
ADD_TEST(klt_test klt_test)

ADD_EXECUTABLE(bipartite_graph_test bipartite_graph_test.cc)
TARGET_LINK_LIBRARIES(bipartite_graph_test gtest)
ADD_TEST(bipartite_graph_test bipartite_graph_test)

ADD_EXECUTABLE(correspondence_test correspondence_test.cc)
TARGET_LINK_LIBRARIES(correspondence_test  correspondence gtest)
ADD_TEST(correspondence_test correspondence_test)

