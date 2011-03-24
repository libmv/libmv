# Locate the Libmv libraries.
#
# Defines the following variables:
#
#   LIBMV_FOUND        - TRUE if the libmv headers and libs are found
#   LIBMV_INCLUDE_DIRS - The path to libmv headers
#
#   LIBMV_LIBRARIES    - All libmv libraries
#   LIBMV_LIBRARY_DIR  - The directory where the libraries are located
#
# Accepts the following variables as input:
#
#   LIBMV_ROOT - (as a CMake or environment variable)
#                The root directory of the libmv install prefix

MESSAGE(STATUS "Looking for Libmv.")

find_path(LIBMV_INCLUDE_DIR libmv/camera/camera.h
    HINTS
    $ENV{LIBMV_ROOT}/include
    ${LIBMV_ROOT}/include
    PATH_SUFFIXES
    libmv
)

IF(LIBMV_INCLUDE_DIR)
  MESSAGE(STATUS "Libmv headers found in ${LIBMV_INCLUDE_DIR}")
ENDIF (LIBMV_INCLUDE_DIR)

SET(LIBMV_LIBRARIES_NAMES camera 
                          correspondence
                          daisy
                          descriptor
                          detector
                          fast
                          flann
                          gflags
                          glog
                          image
                          jpeg
                          multiview 
                          numeric
                          OpenExif
                          pthread
                          png
                          reconstruction
                          tools
                          V3D
                          zlib)

FIND_LIBRARY(LIBMV_LIBRARY NAMES ${LIBMV_LIBRARIES_NAMES}
    HINTS
    $ENV{LIBMV_ROOT}/lib
    ${LIBMV_ROOT}/lib
    PATH_SUFFIXES
    libmv
)
GET_FILENAME_COMPONENT(LIBMV_LIBRARY_DIR "${LIBMV_LIBRARY}" PATH)

IF(LIBMV_LIBRARY)
  MESSAGE(STATUS "Libmv libraries found: ${LIBMV_LIBRARY}")
  MESSAGE(STATUS "Libmv libraries directories: ${LIBMV_LIBRARY_DIR}")
ENDIF (LIBMV_LIBRARY)

SET(LIBMV_LIBRARY "")
foreach(lib ${LIBMV_LIBRARIES_NAMES})
  LIST(APPEND LIBMV_LIBRARY ${lib})  
endforeach()


SET(LIBMV_LIBRARIES ${LIBMV_LIBRARY})
SET(LIBMV_INCLUDE_DIRS ${LIBMV_INCLUDE_DIR})

FIND_PACKAGE(Qt4)
IF(QT_FOUND)
  set(LIBMV_LIBRARIES ${LIBMV_LIBRARIES} ${QT_LIBRARIES})
  set(LIBMV_INCLUDE_DIRS ${LIBMV_INCLUDE_DIRS} ${QT_INCLUDE_DIR})
  set(LIBMV_LIBRARY_DIRS ${LIBMV_LIBRARY_DIR} ${QT_LIBRARY_DIR})
ENDIF(QT_FOUND)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBMV_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Libmv  DEFAULT_MSG
                                  LIBMV_LIBRARY LIBMV_INCLUDE_DIR)

mark_as_advanced(LIBMV_INCLUDE_DIR LIBMV_LIBRARY)

IF(LIBMV_FOUND)
  SET(LIBMV_INCLUDE_DIRS ${LIBMV_INCLUDE_DIRS}
                         ${LIBMV_INCLUDE_DIR}/third_party/daisy/include
                         ${LIBMV_INCLUDE_DIR}/third_party/gtest
                         ${LIBMV_INCLUDE_DIR}/third_party/gtest/include
                         ${LIBMV_INCLUDE_DIR}/third_party/eigen
                         ${LIBMV_INCLUDE_DIR}/third_party/ssba
                         ${LIBMV_INCLUDE_DIR}/third_party/glog/src
                         ${LIBMV_INCLUDE_DIR}/third_party/OpenExif/src
                         ${LIBMV_INCLUDE_DIR}/third_party/OpenExif/src/OpenExifJpeg
  )

  IF (WIN32)
    INCLUDE_DIRECTORIES(LIBMV_INCLUDE_DIRS ${LIBMV_INCLUDE_DIRS}
                        ${LIBMV_INCLUDE_DIR}/third_party/msinttypes
                        ${LIBMV_INCLUDE_DIR}/third_party/jpeg-7
                        ${LIBMV_INCLUDE_DIR}/third_party/zlib
                        ${LIBMV_INCLUDE_DIR}/third_party/pthreads-w32/include
                        ${LIBMV_INCLUDE_DIR}/third_party/png)
  ENDIF (WIN32)

  IF (APPLE)
    INCLUDE_DIRECTORIES(LIBMV_INCLUDE_DIRS ${LIBMV_INCLUDE_DIRS}
                        ${LIBMV_INCLUDE_DIR}/third_party/png
                        ${LIBMV_INCLUDE_DIR}/third_party/jpeg-7)
  ENDIF (APPLE)

  IF (UNIX)
    INCLUDE_DIRECTORIES(LIBMV_INCLUDE_DIRS ${LIBMV_INCLUDE_DIRS}
                        ${LIBMV_INCLUDE_DIR}/third_party/png
                        ${LIBMV_INCLUDE_DIR}/third_party/zlib)
  ENDIF (UNIX)
ENDIF(LIBMV_FOUND)

