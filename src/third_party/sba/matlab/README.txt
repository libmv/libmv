    **************************************************************
            Matlab MEX interface to sba's simple drivers
                  Manolis Lourakis, November 2007
    **************************************************************

==================== GENERAL ====================
This directory contains a MEX-file interface for sba. This
interface allows sba's simple driver routines to be called
directly from matlab. Furthermore, the interface supports
arbitrary user-defined functions for computing the projection
function and its Jacobian; these functions can either be coded
in matlab or come from a shared (dynamic) library.

NOTE: This MEX-file supports the use of matlab-coded user
functions with the intention of providing a convenient
way to access sba's functions from within the matlab
enviroment. However, when execution time is of primary concern,
the use of matlab-coded user functions SHOULD BY ALL MEANS BE AVOIDED:
Due to the inherent overhead of making repeated calls back to
matlab from the MEX-file through mexCallMATLAB() (that are needed
for evaluating the projection function and its Jacobian), this
approach runs considerably slower (one to two orders of magnitude)
compared to an equivalent approach coded entirely in C. In an attempt
to remedy this, the MEX-file also provides the option of loading the
user functions from a shared (dynamic) library. In all cases,
best performance is achieved by avoiding matlab altogether and
implement BA entirely in C, similarly to the example in the ../demo/
directory.

Tested under matlab version 6.5 (R13) under linux and
version 7.4 (R2007) under Windows


==================== FILES ====================
sba.c:    SBA MEX-file
mkproj.m: symbolic matlab code for generating code for the projection
          function and its Jacobian
projRTS.m: projection function, generated with the aid of matlab's
           symbolic toolbox
jacprojRTS.m, jacprojRT.m, jacprojS.m: Jacobian with respect to
          motion & structure, motion only and structure only, resp.
          Generated with the aid of matlab's symbolic toolbox
projac.c: projection function and Jacobian, to be compiled in
          a shared (dynamic) library for use by the MEX-file; see
          also the corresponding comments in sba.m


==================== COMPILING ====================
 - On a Linux/Unix system, typing "make" will build the MEX object.

 - Under Windows, use the provided Makefile as a basis for creating your own makefile.


==================== TESTING ====================
At the command line, type
matlab < eucsbademo.m
