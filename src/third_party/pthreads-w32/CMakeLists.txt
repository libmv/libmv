PROJECT(pthread)

INCLUDE_DIRECTORIES(include)

# TODO(keir): This doesn't work for some reason.
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4996")

# This will build pthread.dll, which you probably want to put in
# C:\Windows\System. Unfortunately, statically linking doesn't work well
# because then you have to manually initialize the pthreads library (which
# normally happens via DLL import hooks).
ADD_LIBRARY(pthread SHARED src/pthread.c)
TARGET_LINK_LIBRARIES(pthread ws2_32)

LIBMV_INSTALL_THIRD_PARTY_LIB(pthread)