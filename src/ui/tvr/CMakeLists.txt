
SET(MOC_HEADERS main_window.h match_viewer.h 3D_viewer.h)
SET(SOURCES main.cc main_window.cc match_viewer.cc 3D_viewer.cc features.cc)
QT4_WRAP_CPP(SOURCES ${MOC_HEADERS})

#ADD_EXECUTABLE(tvr ${SOURCES})
#TARGET_LINK_LIBRARIES(tvr
#                      image
#                      correspondence
#                      numeric
#                      multiview
#                      glog
#                      gflags
#                      ${QT_QTCORE_LIBRARY}
#                      ${QT_QTGUI_LIBRARY}
#                      ${QT_QTOPENGL_LIBRARY}
#                      ${OPENGL_gl_LIBRARY}
#                      ${OPENGL_glu_LIBRARY})

