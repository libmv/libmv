
PROJECT(LIBMV)

ENABLE_TESTING()

SET(CMAKE_CXX_FLAGS "-Wall -W -Wfatal-errors -Werror")

INCLUDE_DIRECTORIES(third_party/gtest
	            third_party/gtest/include
		    third_party/
		    .
)

ADD_SUBDIRECTORY(libmv)
ADD_SUBDIRECTORY(testing)
ADD_SUBDIRECTORY(third_party)
ADD_SUBDIRECTORY(ui)

