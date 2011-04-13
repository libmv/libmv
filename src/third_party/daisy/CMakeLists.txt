project(daisy)

check_symbol_exists(mmap64 "sys/mman.h" HAVE_MMAP64)
if(${HAVE_MMAP64})
    message("System has large file support.")
    add_definitions(-DHAVE_LARGE_FILE_SUPPORT)
else()
    message("System hasn't large file support.")
endif()

include_directories(include)

# Make the headers appear in IDEs.
file(GLOB daisy_hdrs include/kutility/*.h include/daisy/*.h)

set(daisy_srcs
    src/daisy.cpp
    src/corecv.cpp
    src/image_io_bmp.cpp
    src/image_io_pnm.cpp
    src/image_io_png.cpp
    src/image_io_jpeg.cpp
    src/image_manipulation.cpp
    src/progress_bar.cpp
    src/general.cpp
    src/interaction.cpp
)

add_library(daisy ${daisy_srcs} ${daisy_hdrs})

LIBMV_INSTALL_THIRD_PARTY_LIB(daisy)
