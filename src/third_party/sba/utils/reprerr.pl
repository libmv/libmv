#!/usr/bin/perl

#################################################################################
## 
##  Perl script for computing the reprojection error corresponding to a
##  given reconstruction. Currently, projective and quaternion-based euclidean
##  reconstructions are supported. More reconstruction types can be added by
##  supplying appropriate camera matrix generation routines (i.e. CamMat_Generate)
##  Copyright (C) 2005  Manolis Lourakis (lourakis@ics.forth.gr)
##  Institute of Computer Science, Foundation for Research & Technology - Hellas
##  Heraklion, Crete, Greece.
##
##  This program is free software; you can redistribute it and/or modify
##  it under the terms of the GNU General Public License as published by
##  the Free Software Foundation; either version 2 of the License, or
##  (at your option) any later version.
##
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##
#################################################################################

#################################################################################
# Initializations

$usage="Usage is $0 -e|-i|-p [-s,-h] <cams file> <pts file> [<calib file>]";
$help="-e specifies a quaternion-based Euclidean reconstruction with fixed intrinsics,\n-i a Euclidean reconstruction with varying intrinsics and -p a projective one.\n"
."-s computes the average *squared* reprojection error.";
use constant EUCBA => 0; # Euclidean BA, fixed intrinsics
use constant EUCIBA => 1; # Euclidean BA, varying intrinsics
use constant PROJBA => 2; # Projective BA
$cnp=$pnp=0;
$camsfile=$ptsfile=$calfile="";
$CamMat_Generate=\&dont_know;

#################################################################################
# Basic arguments parsing

use Getopt::Std;
getopts("eipsh", \%opt) or die "$usage\n";
die "$0 help: Compute the average reprojection error for some reconstruction.\n$usage\n$help\n" if($opt{h});

if($opt{e}+$opt{i}+$opt{p}!=1){
    die "$0: Only one of -e, -p can be specified!\n";
} elsif($opt{e}){
    $batype=EUCBA;
} elsif($opt{i}){
    $batype=EUCIBA;
} elsif($opt{p}){
    $batype=PROJBA;
}
$squared=$opt{s}? 1 : 0;

#################################################################################
# Initializations depending on reconstruction type
if($batype==EUCBA){
    $cnp=7; $pnp=3;
    die "$0: Cameras, points, or calibration file is missing!\n$usage" if(@ARGV<3);
    die "$0: Too many arguments!\n$usage" if(@ARGV>3);
    $camsfile=$ARGV[0];
    $ptsfile=$ARGV[1];
    $calfile=$ARGV[2];
    $CamMat_Generate=\&PfromRtK;
}
elsif($batype==EUCIBA){
    $cnp=7+5; $pnp=3;
    die "$0: Cameras or points file is missing!\n$usage" if(@ARGV<2);
    die "$0: Too many arguments!\n$usage" if(@ARGV>2);
    $camsfile=$ARGV[0];
    $ptsfile=$ARGV[1];
    $CamMat_Generate=\&PfromRtVarK;
}
elsif($batype==PROJBA){ 
    $cnp=12; $pnp=4;
    die "$0: Cameras or points file is missing!\n$usage" if(@ARGV<2);
    die "$0: Too many arguments!\n$usage" if(@ARGV>2);
    $camsfile=$ARGV[0];
    $ptsfile=$ARGV[1];
    $CamMat_Generate=\&nop;
}
else{
    die "Unknown BA type \"$batype\" specified!\n";
}

die "$0: Do not know how to handle $pnp parameters per point!\n" if($pnp!=3 && $pnp!=4);


#################################################################################
# Main code for computing the reprojection error.

# NOTE: all 2D arrays are stored in row-major order as vectors
@camPoses=(); # array of arrays storing each camera's pose; each element is of size $cnp

@threeDpts=(); # array of arrays storing the reconstructed 3D points; each element is of size $pnp
@twoDtrajs=(); # array of hashes storing the 2D trajectory correponding to reconstructed 3D points.
               # The hash key is the frame number
@trajsFrames=(); # array of arrays storing the frame numbers corresponding to each trajectory.
                 # The first number is the total number of frames, then follow the individual frame
                 # numbers: [nframes, fr_i, fr_j, ..., fr_k]
@camCal=();    # 3x3 array for storing the camera intrinsic calibration


# read calibration file, if there is one
  if(length($calfile)>0){
    if(not open(CAL, $calfile)){
	    print STDERR "cannot open file $calfile: $!\n";
	    exit(1);
    }
    for($i=0; $i<3; ){ # $i gets incremented at the bottom of the loop
      $line=<CAL>;
      if($line=~/\r\n$/){ # CR+LF
        chop($line); chop($line);
      }
      else{
        chomp($line);
      }

      next if($line=~/^#.+/); # skip comments

      @columns=split(" ", $line);
      die "line \"$line\" in $calfile does not contain exactly 3 numbers [$#columns+1]!\n" if($#columns+1!=3);
      $camCal[$i*3]=$columns[0]; $camCal[$i*3+1]=$columns[1]; $camCal[$i*3+2]=$columns[2];
      $i++;
    }
    close(CAL);
  }

# read cameras file
  if(not open(CAMS, $camsfile)){
	  print STDERR "cannot open file $camsfile: $!\n";
	  exit(1);
  }
  $ncams=0;
  while($line=<CAMS>){
    if($line=~/\r\n$/){ # CR+LF
      chop($line); chop($line);
    }
    else{
      chomp($line);
    }

    next if($line=~/^#.+/); # skip comments

    @columns=split(" ", $line);
    #next if($#columns==-1); # skip empty lines

    die "line \"$line\" in $camsfile does not contain exactly $cnp numbers [$#columns+1]!\n" if($cnp!=$#columns+1);
    @pose=();
    for($i=0; $i<$cnp; $i++){
      $pose[$i]=$columns[$i];
    }
    $camPoses[$ncams]=$CamMat_Generate->($ncams, [@pose], [@camCal]);
    $ncams++;
  }
  close(CAMS);

  printf "Read %d cameras\n", scalar(@camPoses);

# read points file
  if(not open(PTS, $ptsfile)){
	  print STDERR "cannot open file $ptsfile: $!\n";
	  exit(1);
  }

  $npts=0;
  $trajno=0;
  while($line=<PTS>){
	  $npts++;
    if($line=~/\r\n$/){ # CR+LF
      chop($line); chop($line);
    }
    else{
      chomp($line);
    }

    next if($line=~/^#.+/); # skip comments
    @columns=split(" ", $line);

    die "line \"$line\" in $ptsfile contains less than $pnp numbers [$#columns+1]!\n" if($pnp>$#columns+1);
    @recpt=();
    for($i=0; $i<$pnp; $i++){
      $recpt[$i]=$columns[$i];
    }

    $nframes=$columns[$pnp];
    $i=$pnp+1+$nframes*3; # 3 numbers per image projection: (i.e. imgid, x, y)
    if($i!=$#columns+1){
      die "line \"$line\" in $ptsfile does not contain exactly the $i numbers required for a 3D point with $nframes 2D projections [$#columns+1]!\n";
    }

    %traj=();
    @theframes=($nframes);
    for($i=0, $j=$pnp+1; $i<$nframes; $i++, $j+=3){
      $traj{$columns[$j]}=[$columns[$j+1], $columns[$j+2]];
      push @theframes, $columns[$j];

#     printf "%d: %d %.6g %.6g\n", $j, $columns[$j], $columns[$j+1], $columns[$j+2];
    }
    $threeDpts[$trajno]=[@recpt];
    $twoDtrajs[$trajno]={%traj};
    $trajsFrames[$trajno++]=[@theframes];
  }
  close(PTS);

  printf "Read %d 3D points \& trajectories\n", scalar(@threeDpts);

# Data file has now been read. Following fragment shows how it can be printed
if(0){
  for($i=0; $i<scalar(@threeDpts); $i++){
    for($j=0; $j<$pnp; $j++){
      printf "%.6g ", $threeDpts[$i][$j];
    }

    printf "%d ", $trajsFrames[$i][0];
    for($j=0; $j<$trajsFrames[$i][0]; $j++){
      $fr=$trajsFrames[$i][$j+1];
      if(defined($twoDtrajs[$i]{$fr})){
        printf "%d %.6g %.6g ", $fr, $twoDtrajs[$i]{$fr}[0], $twoDtrajs[$i]{$fr}[1];
      }
    }
    print "\n";
  }
}

# compute reprojection error
  unless ($batype==EUCBA || $batype==EUCIBA || $batype==PROJBA){
    die "current implementation of reprError() cannot handle supplied reconstruction data!\n";
  }

  $toterr=0.0;
  $totprojs=0.0;
  @error=();
  for($fr=0; $fr<$ncams; $fr++){
    $error[$fr]=0.0;
    for($i=$j=0; $i<scalar(@threeDpts); $i++){
      if(defined($twoDtrajs[$i]{$fr})){
        $theerr=&reprError($twoDtrajs[$i]{$fr}, @camPoses[$fr], @threeDpts[$i], $pnp);
        $theerr=sqrt($theerr) if(!$squared);
        $error[$fr]+=$theerr;
#        printf "@@@ point %d, camera %d: %g\n", $i, $fr, $theerr;
        $j++;
      }
    }
    printf "Mean error for camera %d [%d projections] is %g\n", $fr, $j, $error[$fr]/$j;
    $toterr+=$error[$fr];
    $totprojs+=$j;
  }
  printf "\nMean error for the whole sequence [%d projections]  is %g\n", $totprojs, $toterr/$totprojs;



#################################################################################
# Misc routines

# compute the SQUARED reprojection error |x-xx|^2  with xx=P*X
sub reprError{

  my ($x, $P, $X, $pnp)=@_;

  # error checking
  unless (@_==4 && ref($x) eq 'ARRAY' && ref($P) eq 'ARRAY' && ref($X) eq 'ARRAY'){
    die "usage: reprError ARRAYREF1 ARRAYREF2 ARRAYREF3 pnp";
  }

  my @xx=();
  my $k;

  # compute the projection in xx
  for($k=0; $k<3; $k++){
    $xx[$k]=$P->[$k*4]*$X->[0] + $P->[$k*4+1]*$X->[1] + $P->[$k*4+2]*$X->[2] + $P->[$k*4+3]*(($pnp==4)? $X->[3] : 1.0);
  }
  $xx[0]/=$xx[2];
  $xx[1]/=$xx[2];

# printf "[%g %g -- %g %g] ", $x->[0], $x->[1], $xx[0], $xx[1];

  return ($x->[0]-$xx[0])*($x->[0]-$xx[0]) + ($x->[1]-$xx[1])*($x->[1]-$xx[1]);
}

#################################################################################
# Camera matrix generation routines

sub dont_know {
  my ($camid, $camparms)=@_;

  die "Don't know how to generate a projection matrix for camera $camid from the supplied camera parameters!\n";
  return $camparms;
}

# Return as is
sub nop {
  my ($camid, $camparms)=@_;

  return $camparms;
}

# Compute P as K[R|t]. R is specified by the first 4 elements of $camparms, while t corresponds to the last 3 ones
sub PfromRtK {
  my ($camid, $camparms, $calparams)=@_;

  my $x, $y, $z, $w, $xx, $xy, $xz, $xw, $yy, $yz, $yw, $zz, $zw, $ww, $i, $j, $k;
  my @R=(), @P=(); # 3x3 & 3x4 resp.

# compute the rotation matrix for q=(x, y, z, w);
# see also http://www.gamedev.net/reference/articles/article1095.asp (but note that q=(w, x, y, z) there!)

  $x=$camparms->[0]; $y=$camparms->[1];
  $z=$camparms->[2]; $w=$camparms->[3];
  $xx=$x*$x; $xy=$x*$y; $xz=$x*$z; $xw=$x*$w;
  $yy=$y*$y; $yz=$y*$z; $yw=$y*$w;
  $zz=$z*$z; $zw=$z*$w; $ww=$w*$w;
  $R[0]=$xx+$yy - ($zz+$ww); $R[1]=2.0*($yz-$xw);       $R[2]=2.0*($yw+$xz);
  $R[3]=2.0*($yz+$xw);       $R[4]=$xx+$zz - ($yy+$ww); $R[5]=2.0*($zw-$xy);
  $R[6]=2.0*($yw-$xz);       $R[7]=2.0*($zw+$xy);       $R[8]=$xx+$ww - ($yy+$zz);

#print "@R\n\n";
# compute the matrix-matrix & matrix-vector products
  for($i=0; $i<3; $i++){
    for($j=0; $j<3; $j++){
      for($k=0, $sum=0.0; $k<3; $k++){
        $sum+=$calparams->[$i*3+$k]*$R[$k*3+$j];
      }
      $P[$i*4+$j]=$sum;
    }
    for($j=0, $sum=0.0; $j<3; $j++){
      $sum+=$calparams->[$i*3+$j]*$camparms->[4+$j];
    }
    $P[$i*4+3]=$sum;
  }

  return [@P];
}

# Compute P as K[R|t]. K is specified by the first 5 elements of $camparms, while R and t correspond to the next 4 & 3 elements, respectively
sub PfromRtVarK {
  my ($camid, $camparms)=@_;
  my @K=(), @poseparams=(), $size, $i;

  # setup the intrinsics matrix from the 5 first elements
  $K[0]=$camparms->[0]; $K[1]=$camparms->[4];                $K[2]=$camparms->[1];
  $K[3]=0.0;            $K[4]=$camparms->[3]*$camparms->[0]; $K[5]=$camparms->[2];
  $K[6]=0.0;            $K[7]=0.0;                           $K[8]=1.0;

  $size=scalar(@$camparms);
  for($i=5; $i<$size; $i++){
    $poseparams[$i-5]=$camparms->[$i];
  }

  &PfromRtK($camid, [@poseparams], [@K]);
}
