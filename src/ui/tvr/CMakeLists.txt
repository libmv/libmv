
SET(HEADERS main_window.h match_viewer.h)
SET(SOURCES main.cc main_window.cc match_viewer.cc)
QT4_WRAP_CPP(SOURCES ${HEADERS})

ADD_EXECUTABLE(tvr ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(tvr
                      ${QT_QTCORE_LIBRARY}
                      ${QT_QTGUI_LIBRARY}
                      ${QT_QTOPENGL_LIBRARY})

