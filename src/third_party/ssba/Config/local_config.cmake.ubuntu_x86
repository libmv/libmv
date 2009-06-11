# BIAS related dirs and settings. Note that BIAS is always required.
#enable_feature (V3DLIB_ENABLE_BIAS)
#enable_feature_inc_path (V3DLIB_ENABLE_BIAS /usr/local/include/BIAS)
#enable_feature_lib_path (V3DLIB_ENABLE_BIAS /usr/local/lib/shared)
#enable_feature_libraries (V3DLIB_ENABLE_BIAS BIASGeometry BIASMathAlgo BIASMathBase gsl)
#enable_feature_libraries (V3DLIB_ENABLE_BIAS BIASGeometry BIASMathAlgo BIASMathBase)

# Other optional libraries
#enable_feature (V3DLIB_ENABLE_OPENCV)
#enable_feature (V3DLIB_ENABLE_FFMPEG)
#enable_feature (V3DLIB_ENABLE_GPGPU)
#enable_conditional_feature (V3DLIB_GPGPU_ENABLE_CG V3DLIB_ENABLE_GPGPU)
#enable_feature_libraries (V3DLIB_ENABLE_GPGPU GLEW GLU GL)
#enable_feature_libraries (V3DLIB_GPGPU_ENABLE_CG Cg CgGL pthread)
#enable_feature_libraries (V3DLIB_ENABLE_OPENCV highgui cvaux cv)
#enable_feature_libraries (V3DLIB_ENABLE_FFMPEG avcodec avformat avcodec avutil)

enable_feature (V3DLIB_ENABLE_SUITESPARSE)
enable_feature_inc_path (V3DLIB_ENABLE_SUITESPARSE /usr/include/suitesparse)
#enable_feature_lib_path (V3DLIB_ENABLE_SUITESPARSE /usr/local/src/Devel/SuiteSparse/Lib)
enable_feature_libraries (V3DLIB_ENABLE_SUITESPARSE colamd ldl)

# Debug/release mode selection
set (CMAKE_BUILD_TYPE Release)
#set (CMAKE_BUILD_TYPE Debug)
