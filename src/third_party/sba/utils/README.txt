==================== GENERAL ====================
This directory contains utilities related to sba. 

==================== FILES ====================
hess2eps.c: C source of a function for visualizing the sparseness pattern
            of JtJ in EPS format.
reprerr.pl: Perl script for computing the reprojection error corresponding to a 3D
            reconstruction. The format of datafiles is that explained in ../demo/README.txt

==================== SAMPLE RUNS ====================
Consult http://www.ics.forth.gr/~lourakis/sba/bt_pattern.pdf
for sample output generated with sba_hessian2eps() for Oxford's
corridor sequence.


The command
  ./reprerr.pl -e ../demo/7cams.txt ../demo/7pts.txt ../demo/calib.txt
produces the following output:

Read 7 cameras
Read 465 3D points & trajectories
Mean error for camera 0 [207 projections] is 0.420131
Mean error for camera 1 [244 projections] is 0.46823
Mean error for camera 2 [302 projections] is 0.727632
Mean error for camera 3 [352 projections] is 1.04241
Mean error for camera 4 [351 projections] is 1.14576
Mean error for camera 5 [274 projections] is 1.68392
Mean error for camera 6 [186 projections] is 12.4378

Mean error for the whole sequence [1916 projections]  is 2.06935



Send your comments/questions to lourakis@ics.forth.gr
