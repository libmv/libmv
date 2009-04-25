    **************************************************************
                                 SBA 
                              version 1.5
                          By Manolis Lourakis

                     Institute of Computer Science
            Foundation for Research and Technology - Hellas
                       Heraklion, Crete, Greece
    **************************************************************


==================== GENERAL ====================
This is sba, a copylefted C/C++ implementation of generic bundle adjustment
based on the sparse Levenberg-Marquardt algorithm. sba can support a wide
range of manifestations/parameterizations of the multiple view reconstruction
problem such as arbitrary projective cameras, partially or fully intrinsically
calibrated cameras, exterior orientation (i.e. pose) estimation from fixed 3D
points, etc. sba can be downloaded from http://www.ics.forth.gr/~lourakis/sba

sba relies on lapack for solving the augmented normal equations arising in the
course of the Levenberg-Marquardt algorithm. if you don't already have lapack,
I suggest getting clapack from http://www.netlib.org/clapack.
Directory demo contains eucsbademo, a working example of using sba for Euclidean
bundle adjustment.

More details regarding sba can be found in ICS/FORTH Technical Report No. 340
entitled "The Design and Implementation of a Generic Sparse Bundle Adjustment
Software Package Based on the Levenberg-Marquardt Algorithm", by M.I.A. Lourakis
and A.A. Argyros (available from http://www.ics.forth.gr/~lourakis/sba)

In case that you use sba in your published work, please include a reference to
the above TR:

@techreport{lourakis04,
    author={M.I.A. Lourakis and A.A. Argyros},
    title={The Design and Implementation of a Generic Sparse Bundle Adjustment Software Package
           Based on the Levenberg-Marquardt Algorithm}
    institution={Institute of Computer Science - FORTH},
    address={Heraklion, Crete, Greece},
    number={340},
    year={2004},
    month={Aug.},
    note={Available from \verb+http://www.ics.forth.gr/~lourakis/sba+}
}

==================== FILES ====================
sba_levmar.c: SBA expert driver routines
sba_levmar_wrap.c: simple wrappers around the routines in sba_levmar.c
sba_lapack.c: LAPACK-based linear system solvers (LU, QR, SVD, Cholesky, Bunch-Kaufman)
sba_crsm.c: CRS sparse matrix manipulation routines
sba_chkjac.c: routines for verifying the correctness of user-supplied jacobians
sba.h: Function prototypes & related data structures
demo/*: Euclidean BA demo; see demo/README.txt for more details
matlab/*: sba MEX-file interface; see matlab/README.txt for more details
utils/*: Various utilities; see utils/README.txt for more details

==================== COMPILING ====================
 - On a Linux/Unix system, typing "make" will build both sba and the demo program.

 - Under Windows and if Visual C is installed & configured for command line use,
   type "nmake /f Makefile.vc" in a cmd window to build sba and the demo program.
   In case of trouble, read the comments on top of Makefile.vc


Send your comments/bug reports to lourakis@ics.forth.gr
