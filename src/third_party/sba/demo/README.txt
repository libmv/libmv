==================== GENERAL ====================
This directory contains eucsbademo, an example of using sba for Euclidean bundle adjustment.
Refer to ICS/FORTH TR-340 for documentation on sba. eucsbademo implements two variants of
Euclidean BA: a) BA for camera poses and scene structure assuming intrinsics are fixed and
identical for all cameras and b) BA for camera intrinsics, poses and scene structure
assuming intrinsics vary among cameras.

** For a), eucsbademo accepts 3 file names as arguments:
They are the initial estimates for the camera motion (i.e. pose) parameters, the initial
estimates for the 3D point parameters along with the 3D points image projections and the
camera intrinsic calibration parameters. The file for the camera motion parameters has a
separate line for every camera (i.e. frame), each line containing 7 parameters (a 4
element quaternion for rotation and a 3 element vector for translation). The file for 3D
points and image projections is made up of lines of the form 

X Y Z  NFRAMES  FRAME0 x0 y0  FRAME1 x1 y1 ...

each corresponding to a single 3D point. X Y Z are the points' Euclidean 3D coordinates,
NFRAMES the total number of images in which the points' projections are available and each
of the NFRAMES subsequent triplets FRAME x y pecifies that the 3D point in question projects
to pixel x y in image FRAME. For example, the line

100.0 200.0 300.0 3  2 270.0 114.1 4 234.2 321.7 5 173.6 425.8

specifies the 3D point (100.0, 200.0, 300.0) projecting to the 3 points (270.0, 114.1),
(234.2, 321.7) and (173.6, 425.8) in images 2, 4 and 5 respectively. Camera and 3D point
indices count from 0.

** For b), eucsbademo accepts 2 file names as arguments, specifically the file defining the
inital estimates for the camera intrinsics and motion parameters and that defining the initial
estimates for the 3D point parameters along with the 3D points image projections.
The file for the camera motion parameters has a separate line for every camera, each line
containing 12 parameters (the five intrinsic parameters in the order focal length in x pixels,
principal point coordinates in pixels, aspect ratio [i.e. focalY/focalX] and skew factor,
plus a 4 element quaternion for rotation and a 3 element vector for translation).
The format of the file for 3D points and image projections is identical to that explained
for case a) above. The number of intrincic parameters to be kept fixed can also be selected
(e.g., fixed skew, fixed aspect ratio and skew, fixed aspect ratio, skew and principal point),
check the first few lines of sba_driver().

======================= POINT COVARIANCES =======================
Starting in ver. 1.5, sba supports the incorporation into BA of covariance information
for image points. To accept such covariances, eucsbademo extends the points file format
defined above to lines of the form

X Y Z  NFRAMES  FRAME0 x0 y0 covx0^2 covx0y0 covx0y0 covy0^2  FRAME1 x1 y1 covx1^2 covx1y1 covx1y1 covy1^2 ...

In other words, the covariance matrices simply follow the corresponding image coordinates.
It is also possible to slightly reduce the file size by specifying only the upper
triangular part of covariance matrices, e.g. covx0^2 covx0y0 covy0^2, etc

==================== ROTATION PARAMETRIZATION ====================
In both a) and b) cases above, the eucsbademo program parametrizes rotations internally using
the vector part of the supplied quaternions, i.e. 3 parameters per camera. To achieve this, the
input quaternions are normalized to unit norm and their scalar component is made positive; this
ensures that their scalar parts w can be determined uniquely from the vector parts (x, y, z)
as w=sqrt(1-x^2-y^2-z^2).

==================== FILES ====================
eucsbademo.c:  main demo program
readparams.c:  functions to read the initial motion and structure estimates from text files
imgproj.c:     functions to estimate the projection on a given camera of a certain 3D point. Also
               includes code for evaluating the corresponding jacobian
eucsbademo.h:  function prototypes
readparams.h:  function prototypes

calib.txt:     intrinsic calibration matrix K for the employed camera (used only in the fixed K test cases)

7cams.txt:     initial motion parameters for a test case involving 7 cameras (fixed K)
7camsvarK.txt: initial intrinsic & motion parameters for the 7 cameras test case (varying K)
7pts.txt:      initial structure parameters for the 7 cameras test case

9cams.txt:     initial motion parameters for a test case involving 9 cameras (fixed K)
9camsvarK.txt: initial intrinsic & motion parameters for the 9 cameras test case (varying K)
9pts.txt:      initial structure parameters for the 9 cameras test case

54cams.txt:     initial motion parameters for a test case involving 54 cameras (fixed K)
54camsvarK.txt: initial intrinsic & motion parameters for the 54 cameras test case (varying K)
54pts.txt:      initial structure parameters for the 54 cameras test case

==================== COMPILING ====================
The demo program is built during sba's compilation

==================== SAMPLE RUNS ====================
The command
  eucsbademo 7cams.txt 7pts.txt calib.txt
produces the following output:
  Starting BA with fixed intrinsic parameters
  SBA using 465 3D pts, 7 frames and 1916 image projections, 1437 variables

  Method BA_MOTSTRUCT, expert driver, analytic jacobian, fixed intrinsics

  SBA returned 19 in 19 iter, reason 2, error 0.675396 [initial 19.0947], 26/19 func/fjac evals, 26 lin. systems
  Elapsed time: 0.33 seconds, 330.00 msecs
whereas command
  eucsbademo 7camsvarK.txt 7pts.txt
produces the following output:
  Starting BA with varying intrinsic parameters
  SBA using 465 3D pts, 7 frames and 1916 image projections, 1472 variables

  Method BA_MOTSTRUCT, expert driver, analytic jacobian, variable intrinsics (1 fixed)

  SBA returned 109 in 109 iter, reason 2, error 0.655777 [initial 19.0947], 117/109 func/fjac evals, 119 lin. systems
  Elapsed time: 2.96 seconds, 2960.00 msecs


For the 9 cameras case,
  eucsbademo 9cams.txt 9pts.txt calib.txt
produces
  Starting BA with fixed intrinsic parameters
  SBA using 559 3D pts, 9 frames and 2422 image projections, 1731 variables

  Method BA_MOTSTRUCT, expert driver, analytic jacobian, fixed intrinsics

  SBA returned 21 in 21 iter, reason 2, error 0.619559 [initial 8.17604], 31/21 func/fjac evals, 31 lin. systems
  Elapsed time: 0.50 seconds, 500.00 msecs
and
  eucsbademo 9camsvarK.txt 9pts.txt
produces
  Starting BA with varying intrinsic parameters
  SBA using 559 3D pts, 9 frames and 2422 image projections, 1776 variables

  Method BA_MOTSTRUCT, expert driver, analytic jacobian, variable intrinsics (1 fixed)

  SBA returned 150 in 150 iter, reason 3, error 0.597561 [initial 8.17603], 162/150 func/fjac evals, 169 lin. systems
  Elapsed time: 5.64 seconds, 5640.00 msecs


For the 54 cameras case,
  eucsbademo 54cams.txt 54pts.txt calib.txt
produces
  Starting BA with fixed intrinsic parameters
  SBA using 5207 3D pts, 54 frames and 24609 image projections, 15945 variables

  Method BA_MOTSTRUCT, expert driver, analytic jacobian, fixed intrinsics

  SBA returned 25 in 25 iter, reason 2, error 0.176473 [initial 2.14707], 34/25 func/fjac evals, 34 lin. systems
  Elapsed time: 9.02 seconds, 9020.00 msecs
and
  eucsbademo 54camsvarK.txt 54pts.txt
produces
  Starting BA with varying intrinsic parameters
  SBA using 5207 3D pts, 54 frames and 24609 image projections, 16215 variables

  Method BA_MOTSTRUCT, expert driver, analytic jacobian, variable intrinsics (1 fixed)

  SBA returned 109 in 109 iter, reason 2, error 0.134325 [initial 2.14707], 117/109 func/fjac evals, 123 lin. systems
  Elapsed time: 75.53 seconds, 75530.00 msecs


Send your comments/questions to lourakis@ics.forth.gr
