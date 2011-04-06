ADD_LIBRARY(multiview
            projection.cc
            fundamental.cc
            fundamental_kernel.cc
            panography_kernel.cc
            robust_fundamental.cc
            homography_kernel.cc
            robust_estimation.cc
            focal_from_fundamental.cc
            sixpointnview.cc
            triangulation.cc
            bundle.cc
            autocalibration.cc
            five_point.cc
            affine_2d.cc
            affine_2d_kernel.cc
            robust_affine_2d.cc
            robust_homography.cc
            euclidean_resection.cc
            twoviewtriangulation.cc
            structure.cc
            conditioning.cc
            robust_euclidean_resection.cc
            robust_resection.cc
            essential_kernel.cc
            )

TARGET_LINK_LIBRARIES(multiview numeric V3D colamd ldl)
LIBMV_INSTALL_LIB(multiview)

IF (BUILD_TESTS)
ADD_LIBRARY(multiview_test_data
            test_data_sets.cc)
ENDIF (BUILD_TESTS)

MACRO (MULTIVIEW_TEST NAME)
  LIBMV_TEST(${NAME} "multiview_test_data;multiview;numeric")
ENDMACRO (MULTIVIEW_TEST)

MULTIVIEW_TEST(projection)
MULTIVIEW_TEST(triangulation)
MULTIVIEW_TEST(fundamental)
MULTIVIEW_TEST(fundamental_kernel)
MULTIVIEW_TEST(fundamental_parameterization)
MULTIVIEW_TEST(homography_kernel)
MULTIVIEW_TEST(panography)
MULTIVIEW_TEST(focal_from_fundamental)
MULTIVIEW_TEST(nviewtriangulation)
MULTIVIEW_TEST(resection)
MULTIVIEW_TEST(resection_kernel)
MULTIVIEW_TEST(robust_homography)
MULTIVIEW_TEST(robust_fundamental)
MULTIVIEW_TEST(robust_estimation)
MULTIVIEW_TEST(sixpointnview)
MULTIVIEW_TEST(bundle)
MULTIVIEW_TEST(autocalibration)
MULTIVIEW_TEST(five_point)
MULTIVIEW_TEST(five_point_kernel)
MULTIVIEW_TEST(essential_kernel)
MULTIVIEW_TEST(affine_2d)
MULTIVIEW_TEST(euclidean_resection)
MULTIVIEW_TEST(euclidean_resection_kernel)
MULTIVIEW_TEST(robust_euclidean_resection)
MULTIVIEW_TEST(twoviewtriangulation)
MULTIVIEW_TEST(robust_resection)

# TODO(keir): Make tests that depend on generated.cc to use generated sources.
#ADD_GENERATED_SOURCE(generated.cc generator.py)

