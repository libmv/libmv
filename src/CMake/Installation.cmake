#Install macro for libmv libraries
MACRO (LIBMV_INSTALL_LIB NAME)
  # install headers
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          DESTINATION ${LIBMV_HEADERS_OUTPUT_DIR}/libmv
          FILES_MATCHING PATTERN "*.h"
                         PATTERN "*.hpp"
                         PATTERN ".svn" EXCLUDE
        )
  # install libraries
  INSTALL(TARGETS ${NAME} ${NAME}
    LIBRARY DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
    ARCHIVE DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
  )
ENDMACRO (LIBMV_INSTALL_LIB)

#Install macro for libmv binaries
MACRO (LIBMV_INSTALL_EXE NAME)
  # install libraries
  INSTALL(TARGETS ${NAME}
    RUNTIME DESTINATION ${LIBMV_EXECUTABLE_OUTPUT_DIR}
  )
ENDMACRO (LIBMV_INSTALL_EXE)

#Install macro for third parties libraries
MACRO (LIBMV_INSTALL_THIRD_PARTY_LIB NAME)
  # install libraries
  INSTALL(TARGETS ${NAME} ${NAME}
    LIBRARY DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
    ARCHIVE DESTINATION ${LIBMV_LIBRARY_OUTPUT_DIR}
  )
ENDMACRO (LIBMV_INSTALL_THIRD_PARTY_LIB)

#Install macro for third parties headers
MACRO (LIBMV_INSTALL_THIRD_PARTY_HEADERS)
  # install headers
  INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
          DESTINATION ${LIBMV_HEADERS_OUTPUT_DIR}/third_party
          FILES_MATCHING PATTERN "*"
                         PATTERN ".svn" EXCLUDE
                         PATTERN "*.cpp" EXCLUDE
                         PATTERN "*.cc" EXCLUDE
                         PATTERN "*.c" EXCLUDE
                         PATTERN "CMakeLists.txt" EXCLUDE
        )
ENDMACRO (LIBMV_INSTALL_THIRD_PARTY_HEADERS)


# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/CMake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/../cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/../cmake_uninstall.cmake)
