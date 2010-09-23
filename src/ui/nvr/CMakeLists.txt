# Qt
FIND_PACKAGE(Qt4 REQUIRED)
#SET(QT_USE_QTOPENGL ON)
INCLUDE(${QT_USE_FILE})

SET(HEADERS nview.h)
SET(SOURCES nview.cpp)

QT4_WRAP_CPP(SOURCES ${HEADERS})
ADD_EXECUTABLE(nview ${SOURCES})
TARGET_LINK_LIBRARIES(nview
                      ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
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
