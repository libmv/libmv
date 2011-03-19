#Install macro for libmv libraries
MACRO (LIBMV_INSTALL_LIB NAME)
  INSTALL(TARGETS ${NAME} ${NAME}
    LIBRARY DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
    ARCHIVE DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
  )
ENDMACRO (LIBMV_INSTALL_LIB)

# Install all libmv and third parties headers
MACRO (LIBMV_INSTALL_ALL_HEADERS)
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          DESTINATION ${LIBMV_HEADERS_OUTPUT_DIR}
          FILES_MATCHING PATTERN "*"
                         PATTERN ".svn" EXCLUDE
                         PATTERN "*.cpp" EXCLUDE
                         PATTERN "*.cc" EXCLUDE
                         PATTERN "*.c" EXCLUDE
                         PATTERN "*~" EXCLUDE
                         PATTERN "CMakeLists.txt" EXCLUDE
        )
ENDMACRO (LIBMV_INSTALL_ALL_HEADERS)

# Install macro for libmv binaries
MACRO (LIBMV_INSTALL_EXE NAME)
  # install libraries
  INSTALL(TARGETS ${NAME}
    RUNTIME DESTINATION ${LIBMV_EXECUTABLE_OUTPUT_DIR}
  )
ENDMACRO (LIBMV_INSTALL_EXE)

#Install macro for third parties libraries
MACRO (LIBMV_INSTALL_THIRD_PARTY_LIB NAME)
  INSTALL(TARGETS ${NAME} ${NAME}
    LIBRARY DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
    ARCHIVE DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
  )
ENDMACRO (LIBMV_INSTALL_THIRD_PARTY_LIB)

# Uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
    "${LIBMV_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${LIBMV_BINARY_DIR}/cmake_uninstall.cmake)


INSTALL(FILES CMake/modules/FindLibmv.cmake
        DESTINATION "${CMAKE_ROOT}/Modules/"
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
