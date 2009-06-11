cmake_minimum_required(VERSION 2.4)

project(V3D)

set (V3D_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (V3D_INCLUDE_DIRS ${V3D_DIR}/.)

include (Config/v3d_macros.cmake)

include (Config/local_config.cmake)

include_directories(${V3D_INCLUDE_DIRS} ${EXTRA_INC_DIRS})

source_group("Base" REGULAR_EXPRESSION Base/.*cpp|Base.*h)
set (BASE_SRC
)

source_group("Math" REGULAR_EXPRESSION Math/.*cpp|Math.*h)
set (MATH_SRC
    Math/v3d_linear.h
    Math/v3d_mathutilities.h
    Math/v3d_optimization.h
    Math/v3d_optimization.cpp
)

source_group("Geometry" REGULAR_EXPRESSION Geometry/.*cpp|Geometry.*h)
set (GEOMETRY_SRC
    Geometry/v3d_cameramatrix.h
    Geometry/v3d_metricbundle.h
    Geometry/v3d_metricbundle.cpp
)

source_group("GL" REGULAR_EXPRESSION GL/.*cpp|GL.*h)
set (GL_SRC
)

source_group("GL\\Shaders" REGULAR_EXPRESSION GL/Shaders/.*cg)
set (GL_SHADERS_SRC
)

set (ALL_SRC
    ${BASE_SRC} ${MATH_SRC} ${GEOMETRY_SRC} ${GL_SRC} ${GL_SHADERS_SRC}
)

SET(CMAKE_CXX_FLAGS "")

add_library(V3D ${ALL_SRC})
add_subdirectory(Apps)
