ADD_LIBRARY(correspondence 
            klt.cc 
            feature.cc 
            matches.cc 
            feature_matching.cc
            feature_matching_FLANN.cc
            tracker.cc
            robust_tracker.cc
            nRobustViewMatching.cc)
LIBMV_INSTALL_LIB(correspondence)
            
LIBMV_TEST(klt "correspondence;image;numeric")
LIBMV_TEST(bipartite_graph "")
LIBMV_TEST(kdtree "")
LIBMV_TEST(feature_set "correspondence;image;numeric")
LIBMV_TEST(matches "correspondence;image;numeric")
LIBMV_TEST(Array_Matcher "correspondence;numeric;flann")
