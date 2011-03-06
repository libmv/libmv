
SET(LIBMV_VERSION_MAJOR 0)
SET(LIBMV_VERSION_MINOR 0)
SET(LIBMV_VERSION_PATCH 9) #or the svn revision

# The standard GNU version numbering scheme is major.minor.revision
IF (EXISTS ${LIBMV_SOURCE_DIR}/.svn)
  # Probably a SVN workspace, determine revision level 
  FIND_PACKAGE(Subversion)
  IF(Subversion_FOUND)
    Subversion_WC_INFO(${LIBMV_SOURCE_DIR} ER)
    SET(SUBVERSION_REVISION ${ER_WC_REVISION})
    SET(LIBMV_VERSION_PATCH ${SUBVERSION_REVISION})
  ENDIF(Subversion_FOUND)
ENDIF (EXISTS ${LIBMV_SOURCE_DIR}/.svn)

# Built from svn
SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}.${LIBMV_VERSION_PATCH}")
# Alpha version [+number]
# SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}-a")
# Beta version [+number]
# SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}-b")
# Release candidate version [+number]
# SET(LIBMV_VERSION "${LIBMV_VERSION_MAJOR}.${LIBMV_VERSION_MINOR}-rc")

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/CMake/revision.h.in ${CMAKE_CURRENT_SOURCE_DIR}/libmv/tools/revision.h)