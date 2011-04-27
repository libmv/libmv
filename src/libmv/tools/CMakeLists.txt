# define the source files
SET(TOOLS_SRC ExifReader.cc)
               
# define the header files (make the headers appear in IDEs.)
FILE(GLOB TOOLS_HDRS *.h)

ADD_LIBRARY(tools ${TOOLS_SRC} ${TOOLS_HDRS})

TARGET_LINK_LIBRARIES(tools)

# make the name of debug libraries end in _d.
SET_TARGET_PROPERTIES(tools PROPERTIES DEBUG_POSTFIX "_d")

# installation rules for the library
LIBMV_INSTALL_LIB(tools)