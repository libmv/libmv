#########################
# Qt
FIND_PACKAGE(Qt4)
ADD_DEFINITIONS( ${QT_DEFINITIONS} )
INCLUDE_DIRECTORIES( ${QT_INCLUDES} )
LINK_DIRECTORIES( ${QT_LIBRARY_DIR} )
#########################

SET(imageviewer_SOURCES main.cc imageviewer.cc scrubber.cc)
QT_WRAP_CPP(LIBMV imageviewer_SOURCES imageviewer.h scrubber.h)
ADD_EXECUTABLE(imageviewer ${imageviewer_SOURCES})
TARGET_LINK_LIBRARIES(imageviewer image 
                      ${BLAS_LIBRARIES}
                      ${LAPACK_LIBRARIES}
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${QT_QTOPENGL_LIBRARY})

