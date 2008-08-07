cmake_minimum_required(VERSION 2.2)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
  cmake_policy(SET CMP0005 OLD)
endif(COMMAND cmake_policy)


PROJECT(LIBMV)

ENABLE_TESTING()

#no-deprecated needed for gcc-4.3 on debian/testing
# needed due to flens/refcounter.h including deprecated hash_map
SET(CMAKE_CXX_FLAGS "-Wall -W -Wfatal-errors -Werror -Wno-deprecated")

INCLUDE_DIRECTORIES(third_party/gtest
	            third_party/gtest/include
		    third_party/
		    ${LIBMV_SOURCE_DIR}
		    ${LIBMV_BINARY_DIR}
)

# Mac specific things.
IF(APPLE)
    # Tell gtest not to use mac framworks.
    ADD_DEFINITIONS('-DGTEST_NOT_MAC_FRAMEWORK_MODE')

    # Tell FLENS to use Veclib.
    ADD_DEFINITIONS('-DVECLIB')
ENDIF(APPLE)


MACRO(ADD_GENERATED_SOURCE generated_file python_generator)
    ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${generated_file}
	       COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/${python_generator}
	       DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${python_generator})
ENDMACRO(ADD_GENERATED_SOURCE)


ADD_SUBDIRECTORY(libmv)
ADD_SUBDIRECTORY(testing)
ADD_SUBDIRECTORY(third_party)
ADD_SUBDIRECTORY(ui)
ADD_SUBDIRECTORY(tools)
