PROJECT(fast)

SET(FAST_SRCS
fast.h
fast.c
fast_9.c
fast_10.c
fast_11.c
fast_12.c
nonmax.c
)

ADD_LIBRARY(fast ${FAST_SRCS} )

