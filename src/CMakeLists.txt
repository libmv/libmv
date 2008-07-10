
PROJECT(LIBMV)

ENABLE_TESTING()

SET(CMAKE_CXX_FLAGS "-Wall -W -Wfatal-errors -Werror")

INCLUDE_DIRECTORIES(third_party/gtest
	            third_party/gtest/include
		    third_party/
		    .
)

# Mac specific things.
IF(APPLE)
    # Tell gtest not to use mac framworks.
    ADD_DEFINITIONS('-DGTEST_NOT_MAC_FRAMEWORK_MODE')

    # Tell FLENS to use Veclib.
    ADD_DEFINITIONS('-DVECLIB')
ENDIF(APPLE)


ADD_SUBDIRECTORY(libmv)
ADD_SUBDIRECTORY(testing)
ADD_SUBDIRECTORY(third_party)
ADD_SUBDIRECTORY(ui)

