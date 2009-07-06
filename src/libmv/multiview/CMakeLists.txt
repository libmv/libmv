ADD_DEFINITIONS(-DTHIS_SOURCE_DIR=\\"${CMAKE_CURRENT_SOURCE_DIR}\\")

ADD_LIBRARY(multiview
            projection.cc
            fundamental.cc
            robust_fundamental.cc
            homography.cc
            robust_estimation.cc
            focal_from_fundamental.cc
            sixpointnview.cc
            triangulation.cc
            bundle.cc
            autocalibration.cc)

TARGET_LINK_LIBRARIES(multiview numeric
                                V3D colamd ldl)

ADD_LIBRARY(multiview_test_data
            test_data_sets.cc)

MACRO (MULTIVIEW_TEST NAME)
  LIBMV_TEST(${NAME} "multiview_test_data;multiview;numeric")
ENDMACRO (MULTIVIEW_TEST)

MULTIVIEW_TEST(projection)
MULTIVIEW_TEST(triangulation)
MULTIVIEW_TEST(fundamental)
MULTIVIEW_TEST(fundamental_parameterization)
MULTIVIEW_TEST(homography)
MULTIVIEW_TEST(focal_from_fundamental)
MULTIVIEW_TEST(nviewtriangulation)
MULTIVIEW_TEST(resection)
MULTIVIEW_TEST(robust_fundamental)
MULTIVIEW_TEST(robust_estimation)
MULTIVIEW_TEST(sixpointnview)
MULTIVIEW_TEST(bundle)
MULTIVIEW_TEST(autocalibration)

# TODO(keir): Make tests that depend on generated.cc to use generated sources.
#ADD_GENERATED_SOURCE(generated.cc generator.py)

