ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

ADD_LIBRARY(multiview
            projection.cc
            fundamental.cc
            focal_from_fundamental.cc
            triangulation.cc)

ADD_LIBRARY(multiview_test_data
            test_data_sets.cc)

MACRO (MULTIVIEW_TEST NAME)
  LIBMV_TEST(${NAME} "multiview_test_data;multiview;numeric")
ENDMACRO (MULTIVIEW_TEST)

MULTIVIEW_TEST(projection)
MULTIVIEW_TEST(triangulation)
MULTIVIEW_TEST(fundamental)
MULTIVIEW_TEST(focal_from_fundamental)

# TODO(keir): Make tests that depend on generated.cc to use generated sources.
#ADD_GENERATED_SOURCE(generated.cc generator.py)

