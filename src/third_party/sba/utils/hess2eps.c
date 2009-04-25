/////////////////////////////////////////////////////////////////////////////////
//// 
////  Visualization of the nonzero pattern of JtJ in EPS format.
////  This is part of the sba package,
////  Copyright (C) 2004  Manolis Lourakis (lourakis@ics.forth.gr)
////  Institute of Computer Science, Foundation for Research & Technology - Hellas
////  Heraklion, Crete, Greece.
////
////  This program is free software; you can redistribute it and/or modify
////  it under the terms of the GNU General Public License as published by
////  the Free Software Foundation; either version 2 of the License, or
////  (at your option) any later version.
////
////  This program is distributed in the hope that it will be useful,
////  but WITHOUT ANY WARRANTY; without even the implied warranty of
////  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
////  GNU General Public License for more details.
////
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "sba.h"

#if 0
#define HORZ_OFFSET 3 // horizontal offset from left edge of paper in PostScript points (~ 1 mm)
#define VERT_OFFSET 6 // vertical offset from lower edge of paper in PostScript points (~ 2 mm)
#else
#define HORZ_OFFSET 0
#define VERT_OFFSET 0
#endif

#define A4_WIDTH    595 // A4 paper width in PostScript points (210 mm, 1 pt = 25.4/72 mm)
//#define A4_HEIGHT   842 // A4 paper height in PostScript points (297 mm)

/* 1.1x1.1 looks better than 1.0x1.0 */
#define DOT_WIDTH   1.1
#define DOT_HEIGHT  1.1

/* saves the nonzero pattern of JtJ in EPS format. If mcon!=0, then all U_j and W_ij with j<mcon
 * are assumed to be zero and are therefore clipped off.
 */
void sba_hessian2eps(int n, int m, int mcon, int cnp, int pnp, struct sba_crsm *idxij, int *rcidxs, int *rcsubs,
               double *U, double *V, double *W, char *fname)
{
register int i, j, ii, jj;
int i0, j0;
int mmcon, nvars, Usz, Vsz, Wsz, nnz, bbox=1, denseblocks=0;
register double *ptr1, *ptr2;
FILE *fp;
time_t tim;

  mmcon=m-mcon;
  nvars=mmcon*cnp + n*pnp;
  Usz=cnp * cnp; Vsz=pnp * pnp; Wsz=cnp * pnp; 

  if((fp=fopen(fname, "w"))==NULL){
    fprintf(stderr, "JtJ2EPS(): failed to open file %s for writing\n", fname);
    exit(1);
  }

  /* print EPS preamble */
  time(&tim);
  fprintf(fp, "%%!PS-Adobe-2.0 EPSF-2.0\n%%%%Title: %s\n%%%%Creator: sba ver. %s\n", fname, SBA_VERSION);
  fprintf(fp, "%%%%CreationDate: %s", ctime(&tim));
  fprintf(fp, "%%%%BoundingBox: 0 0 %d %d\n%%%%Magnification: 1.0000\n%%%%Page: 1 1\n%%%%EndComments\n\n", A4_WIDTH, A4_WIDTH+VERT_OFFSET);
  fprintf(fp, "/origstate save def\ngsave\n0 setgray\n");
  if(bbox) fprintf(fp, "0 0 %d %d rectstroke\n", A4_WIDTH, A4_WIDTH+VERT_OFFSET);

  /* move the coordinate system to the upper left corner with axes pointing as shown below:
   * +------> y
   * |
   * |
   * |
   * v x
   */
  fprintf(fp, "%d %d translate\n-90 rotate\n", HORZ_OFFSET, A4_WIDTH+VERT_OFFSET);
  /* scale the coordinate system so that the hessian pattern fits in the narrowest page dimension */
  fprintf(fp, "%.4lf %.4lf scale\n", ((double)(A4_WIDTH-HORZ_OFFSET))/nvars, ((double)(A4_WIDTH-HORZ_OFFSET))/nvars);

  /* define dot dimensions */
  fprintf(fp, "/w %g def\n/h %g def\n", DOT_WIDTH, DOT_HEIGHT);
  /* define shorthand for rectfill */
  fprintf(fp, "/R { rectfill } bind def\n\n");

  /* process block row j: [0, ..., 0, U_j, 0, ..., 0, W1j, ..., Wnj] */
  for(j=mcon; j<m; ++j){
    ptr1=U + j*Usz; // set ptr1 to point to U_j
    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero W_ij, i=0...n-1 */

    i0=(j-mcon)*cnp;
    for(ii=0; ii<cnp; ++ii){
      j0=(j-mcon)*cnp;
      for(jj=0; jj<cnp; ++jj){ // row ii of U_j
        if(denseblocks || (!denseblocks && ptr1[ii*cnp+jj]!=0.0))
          fprintf(fp, "%d %d w h R\n", i0+ii, j0+jj);
      }

      for(i=0; i<nnz; ++i){
        /* set ptr2 to point to W_ij, actual row number in rcsubs[i] */
        ptr2=W + idxij->val[rcidxs[i]]*Wsz;
        j0=mmcon*cnp+rcsubs[i]*pnp;
        for(jj=0; jj<pnp; ++jj){ // row ii of W_ij
          if(denseblocks || (!denseblocks && ptr2[ii*pnp+jj]!=0.0))
            fprintf(fp, "%d %d w h R\n", i0+ii, j0+jj);
        }
      }
    }
  }

  /* process block row mmcon+i: [W_i1^T, ..., W_im^T, 0, ..., 0, V_i, 0, ..., 0] */
  for(i=0; i<n; ++i){
    nnz=sba_crsm_row_elmidxs(idxij, i, rcidxs, rcsubs); /* find nonzero W_ij, j=0...m-1 */
    ptr1=V + i*Vsz; // set ptr1 to point to V_i

    i0=cnp*mmcon+i*pnp;
    for(ii=0; ii<pnp; ++ii){
      for(j=0; j<nnz; ++j){
        /* set ptr2 to point to W_ij, actual column number in rcsubs[j] */
        if(rcsubs[j]<mcon) continue; /* W_ij is zero */

        ptr2=W + idxij->val[rcidxs[j]]*Wsz;

        j0=(rcsubs[j]-mcon)*cnp;
        for(jj=0; jj<cnp; ++jj){ // row ii of W_ij^T
           if(denseblocks || (!denseblocks && ptr2[jj*pnp+ii]!=0.0))
             fprintf(fp, "%d %d w h R\n", i0+ii, j0+jj);
        }
      }

      j0=cnp*mmcon+i*pnp;
      for(jj=0; jj<pnp; ++jj){ // row ii of V_i
        if(denseblocks || (!denseblocks && ptr1[ii*pnp+jj]!=0.0))
          fprintf(fp, "%d %d w h R\n", i0+ii, j0+jj);
      }
    }
  }

  fprintf(fp, "grestore\norigstate restore\n\n%%%%Trailer");
  fclose(fp);
}
