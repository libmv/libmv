
INCLUDE_DIRECTORIES(algorithms util nn .)

SET(SOURCES flann.cpp util/Random.cpp nn/Testing.cpp algorithms/NNIndex.cpp algorithms/dist.cpp util/Logger.cpp)

ADD_LIBRARY(flann STATIC ${SOURCES})
