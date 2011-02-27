# TODO(julien) make it work!!!
# TODO(julien) also make windows/mac OS archives

SET(CPACK_PACKAGE_NAME "libmv")
SET(CPACK_PACKAGE_VERSION ${LIBMV_VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${LIBMV_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${LIBMV_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${LIBMV_VERSION_PATCH})

# The libmv binary package must contain: bin/ lib/*.so(dll)
SET(CPACK_GENERATOR "TBZ2;DEB;RPM;NSIS")
SET(CPACK_IGNORE_FILES  ".svn;build/;bin-opt/;bin-dbg/;doc/;include/;src/;lib/*.a;lib/*.lib;contrib/;extras/;~$;${CPACK_SOURCE_IGNORE_FILES}")
SET(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${LIBMV_VERSION}")#-${CMAKE_SYSTEM_PROCESSOR}

# The libmv source package must contain: binary + src/ lib/*.a(lib) contrib/ extras/ doc/ ?
SET(CPACK_SOURCE_GENERATOR "TBZ2;DEB;RPM;NSIS")
SET(CPACK_SOURCE_IGNORE_FILES  ".svn/;build/;bin-opt/;bin-dbg/;bin/;~$;${CPACK_SOURCE_IGNORE_FILES}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${LIBMV_VERSION}-dev")

# SET(CPACK_INSTALL_CMAKE_PROJECTS "${LIBMV_BINARY_DIR}" "libmv" "ALL" "/")

SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.3.6), libgcc1 (>= 1:4.1)")
# TODO(julien) add third_parties: libpthread-stubs0, libjpeg62, (zlib?, png?) 

SET(CPACK_PACKAGE_INSTALL_DIRECTORY "libmv")
SET(CPACK_PACKAGE_DESCRIPTION "A Structure from Motion library")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Structure from Motion library")
SET(CPACK_RESOURCE_FILE_LICENSE "${LIBMV_SOURCE_DIR}/../LICENSE")
SET(CPACK_PACKAGE_CONTACT "Libmv Team. <http://code.google.com/p/libmv>")

SET(CMAKE_INSTALL_PREFIX /usr)
SET(CPACK_SET_DESTDIR TRUE)

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} package_source)

INCLUDE(CPack)

# #set(CPACK_PACKAGE_ICON "${LIBMV_SOURCE_DIR}\\\\libmv-logo.png")

# TODO(julien) NSIS (windows)
# set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "WriteRegStr \\\${WriteEnvStr_RegKey} \\\"LIBMV_HOME\\\" $INSTDIR")
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/revision.h.in ${CMAKE_CURRENT_BINARY_DIR}/revision.h)

# TODO(julien) Bundle (Mac OS)

# see http://www.cmake.org/Wiki/CMake:CPackPackageGenerators
