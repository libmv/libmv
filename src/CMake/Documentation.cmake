# TODO(julien) make it work!!!

# check if doxygen is even installed
FIND_PACKAGE(Doxygen)
IF (NOT DOXYGEN_FOUND STREQUAL "NO")

# add doxygen as target
ADD_CUSTOM_TARGET(doxygen ${DOXYGEN_EXECUTABLE} ${LIBMV_SOURCE_DIR}/../Doxyfile)

# cleanup $build/api-doc on "make clean"
SET_PROPERTY(DIRECTORY APPEND PROPERTY
       ADDITIONAL_MAKE_CLEAN_FILES doc)

# add doxygen as dependency to doc-target
GET_TARGET_PROPERTY(DOC_TARGET doc TYPE)
IF(NOT DOC_TARGET)
  ADD_CUSTOM_TARGET(doc)
ENDIF()
ADD_DEPENDENCIES(doc doxygen)

# install HTML API documentation and manual pages
SET(DOC_PATH "share/doc/${CPACK_PACKAGE_NAME}-${VERSION}")

INSTALL(DIRECTORY ${LIBMV_SOURCE_DIR}/../doc/html
         DESTINATION ${DOC_PATH}
       )

# install man pages into packages, scope is now project root..
INSTALL(DIRECTORY ${LIBMV_SOURCE_DIR}/../doc/man/man3
         DESTINATION share/man/man3/
       )

ENDIF(NOT DOXYGEN_FOUND STREQUAL "NO")