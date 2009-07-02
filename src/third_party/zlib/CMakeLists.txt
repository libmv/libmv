PROJECT(ZLIB)

ADD_LIBRARY(zlib STATIC ${ZLIB_SRCS} 
  adler32.c    compress.c   crc32.c      deflate.c    gzio.c       infback.c    inffast.c    inflate.c
  inftrees.c   minigzip.c   trees.c      uncompr.c    zutil.c
  crc32.h      deflate.h    inffast.h    inffixed.h   inflate.h    inftrees.h   trees.h      zconf.h      zconf.in.h
  zlib.h       zutil.h
)
