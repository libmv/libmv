#
# Makefile for Sparse Bundle Adjustment demo program
#
CC=gcc
CFLAGS=-I.. -O3 -Wall #-g -pg
OBJS=eucsbademo.o imgproj.o readparams.o
SRCS=eucsbademo.c imgproj.c readparams.c


LAPACKLIBS=-llapack -lblas -lf2c # On systems with a FORTRAN (not f2c'ed) version of LAPACK, -lf2c is
                                 # not necessary; on others -lf2c is equivalent to -lF77 -lI77

#LAPACKLIBS=-L/usr/local/atlas/lib -llapack -lcblas -lf77blas -latlas -lf2c # This works with the ATLAS updated lapack and Linux_P4SSE2
                                                                            # from http://www.netlib.org/atlas/archives/linux/

#LAPACKLIBS=-llapack -lgoto -lpthread -lf2c # This works with GotoBLAS
                                            # from http://www.tacc.utexas.edu/resources/software/

#LAPACKLIBS=-L/opt/intel/mkl/8.0.1/lib/32/ -lmkl_lapack -lmkl_ia32 -lguide -lf2c # This works with MKL 8.0.1 from
                                            # http://www.intel.com/cd/software/products/asmo-na/eng/perflib/mkl/index.htm

LIBS=-lsba $(LAPACKLIBS) -lm
LDFLAGS=-L..

eucsbademo: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o eucsbademo $(LIBS)

eucsbademo.o: eucsbademo.h readparams.h
readparams.o: readparams.h

clean:
	@rm -f $(OBJS)

realclean cleanall: clean
	@rm -f eucsbademo

depend:
	makedepend -f Makefile $(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
