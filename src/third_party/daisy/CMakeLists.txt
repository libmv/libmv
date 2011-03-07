project(daisy)

include_directories(include)

set(daisy_srcs
    include/kutility/convolution_default.h
    include/kutility/fileio.tcc
    include/kutility/image_io_jpeg.h
    include/kutility/kutility.def
    include/kutility/convolution.h
    include/kutility/general.h
    include/kutility/image.h
    include/kutility/image_io_bmp.h
    include/kutility/image_io_pnm.h
    include/kutility/image_io_png.h 
    include/kutility/math.h
    include/kutility/convolution_opencv.h
    include/kutility/progress_bar.h
    include/kutility/corecv.h
    include/kutility/image_manipulation.h
    include/kutility/fileio.h
    include/kutility/image_io.h
    include/kutility/interaction.h
    include/daisy/daisy.h
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

add_library(daisy ${daisy_srcs} )

LIBMV_INSTALL_THIRD_PARTY_LIB(daisy)