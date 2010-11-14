# Qt
FIND_PACKAGE(Qt4 REQUIRED)
FIND_PACKAGE(OpenGL REQUIRED)
SET(QT_USE_QTOPENGL ON)
INCLUDE(${QT_USE_FILE})

SET(HEADERS nview.h)
SET(SOURCES nview.cpp 
            glwidget.cc)

QT4_WRAP_CPP(SOURCES ${HEADERS})
ADD_EXECUTABLE(nview ${SOURCES})
TARGET_LINK_LIBRARIES(nview
                      ${QT_QTCORE_LIBRARY} 
                      ${QT_QTGUI_LIBRARY}
                      ${QT_QTOPENGL_LIBRARY}
                      ${OPENGL_gl_LIBRARY}
                      ${OPENGL_glu_LIBRARY}
                      correspondence
                      image
                      gflags
                      glog
                      detector
                      fast
                      flann
                      descriptor
                      multiview
                      daisy )

LIBMV_INSTALL_EXE(nview)