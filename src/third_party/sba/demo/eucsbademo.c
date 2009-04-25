/* Euclidean bundle adjustment demo using the sba package */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <sba.h>
#include "eucsbademo.h"
#include "readparams.h"

#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000.0)

#define MAXITER         100

/* pointers to additional data, used for computed image projections and their jacobians */
struct globs_{
	double *intrcalib; /* the 5 intrinsic calibration parameters in the order [fu, u0, v0, ar, skew],
                      * where ar is the aspect ratio fv/fu.
                      * Used only when calibration is fixed for all cameras;
                      * otherwise, it is null and the intrinsic parameters are
                      * included in the set of motion parameters for each camera
                      */
  int nccalib; /* number of calibration parameters that must be kept constant.
                * 0: all paremeters are free 
                * 1: skew is fixed to its initial value, all other parameters vary (i.e. fu, u0, v0, ar) 
                * 2: skew and aspect ratio are fixed to their initial values, all other parameters vary (i.e. fu, u0, v0)
                * 3: meaningless
                * 4: skew, aspect ratio and principal point are fixed to their initial values, only the focal length varies (i.e. fu)
                * >=5: meaningless
                * Used only when calibration varies among cameras
                */

  int cnp, pnp, mnp; /* dimensions */

	double *ptparams; /* needed only when bundle adjusting for camera parameters only */
	double *camparams; /* needed only when bundle adjusting for structure parameters only */
} globs;


/* Routines to estimate the estimated measurement vector (i.e. "func") and
 * its sparse jacobian (i.e. "fjac") needed in BA. Code below makes use of the
 * routines calcImgProj() and calcImgProjJacXXX() which
 * compute the predicted projection & jacobian of a SINGLE 3D point (see imgproj.c).
 * In the terminology of TR-340, these routines compute Q and its jacobians A=dQ/da, B=dQ/db.
 * Notice also that what follows is two pairs of "func" and corresponding "fjac" routines.
 * The first is to be used in full (i.e. motion + structure) BA, the second in 
 * motion only BA.
 */

/****************************************************************************************/
/* MEASUREMENT VECTOR AND JACOBIAN COMPUTATION FOR VARYING CAMERA POSE AND 3D STRUCTURE */
/****************************************************************************************/

/*** MEASUREMENT VECTOR AND JACOBIAN COMPUTATION FOR THE SIMPLE DRIVERS ***/

/* FULL BUNDLE ADJUSTMENT, I.E. SIMULTANEOUS ESTIMATION OF CAMERA AND STRUCTURE PARAMETERS */

/* Given the parameter vectors aj and bi of camera j and point i, computes in xij the
 * predicted projection of point i on image j
 */
static void img_projRTS(int j, int i, double *aj, double *bi, double *xij, void *adata)
{
  double *Kparms;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  Kparms=gl->intrcalib;

  calcImgProj(Kparms, aj, aj+3, bi, xij); // 3 is the quaternion's vector part length
}

/* Given the parameter vectors aj and bi of camera j and point i, computes in Aij, Bij the
 * jacobian of the predicted projection of point i on image j
 */
static void img_projRTS_jac(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata)
{
  double *Kparms;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  Kparms=gl->intrcalib;

  calcImgProjJacRTS(Kparms, aj, aj+3, bi, (double (*)[6])Aij, (double (*)[3])Bij); // 3 is the quaternion's vector part length
}

/* BUNDLE ADJUSTMENT FOR CAMERA PARAMETERS ONLY */

/* Given the parameter vector aj of camera j, computes in xij the
 * predicted projection of point i on image j
 */
static void img_projRT(int j, int i, double *aj, double *xij, void *adata)
{
  int pnp;

  double *Kparms, *ptparams;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  pnp=gl->pnp;
  Kparms=gl->intrcalib;
  ptparams=gl->ptparams;

  calcImgProj(Kparms, aj, aj+3, ptparams+i*pnp, xij); // 3 is the quaternion's vector part length
}

/* Given the parameter vector aj of camera j, computes in Aij
 * the jacobian of the predicted projection of point i on image j
 */
static void img_projRT_jac(int j, int i, double *aj, double *Aij, void *adata)
{
  int pnp;

  double *Kparms, *ptparams;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  pnp=gl->pnp;
  Kparms=gl->intrcalib;
  ptparams=gl->ptparams;

  calcImgProjJacRT(Kparms, aj, aj+3, ptparams+i*pnp, (double (*)[6])Aij); // 3 is the quaternion's vector part length
}

/* BUNDLE ADJUSTMENT FOR STRUCTURE PARAMETERS ONLY */

/* Given the parameter vector bi of point i, computes in xij the
 * predicted projection of point i on image j
 */
static void img_projS(int j, int i, double *bi, double *xij, void *adata)
{
  int cnp;

  double *Kparms, *camparams, *aj;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp;
  Kparms=gl->intrcalib;
  camparams=gl->camparams;
  aj=camparams+j*cnp;

  calcImgProj(Kparms, aj, aj+3, bi, xij); // 3 is the quaternion's vector part length
}

/* Given the parameter vector bi of point i, computes in Bij
 * the jacobian of the predicted projection of point i on image j
 */
static void img_projS_jac(int j, int i, double *bi, double *Bij, void *adata)
{
  int cnp;

  double *Kparms, *camparams, *aj;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp;
  Kparms=gl->intrcalib;
  camparams=gl->camparams;
  aj=camparams+j*cnp;

  calcImgProjJacS(Kparms, aj, aj+3, bi, (double (*)[3])Bij); // 3 is the quaternion's vector part length
}

/*** MEASUREMENT VECTOR AND JACOBIAN COMPUTATION FOR THE EXPERT DRIVERS ***/

/* FULL BUNDLE ADJUSTMENT, I.E. SIMULTANEOUS ESTIMATION OF CAMERA AND STRUCTURE PARAMETERS */

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * hx the prediction of the measurements, i.e. the projections of 3D points in the m images. The measurements
 * are returned in the order (hx_11^T, .. hx_1m^T, ..., hx_n1^T, .. hx_nm^T)^T, where hx_ij is the predicted
 * projection of the i-th point on the j-th camera.
 * Notice that depending on idxij, some of the hx_ij might be missing
 *
 */
static void img_projsRTS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pa, *pb, *pqr, *pt, *ppt, *pmeas, *Kparms;
  //int n;
  int m, nnz;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  Kparms=gl->intrcalib;

  //n=idxij->nr;
  m=idxij->nc;
  pa=p; pb=p+m*cnp;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pqr=pa+j*cnp;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=pb + rcsubs[i]*pnp;
      pmeas=hx + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

      calcImgProj(Kparms, pqr, pt, ppt, pmeas); // evaluate Q in pmeas
    }
  }
}

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * jac the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is returned in the order (A_11, ..., A_1m, ..., A_n1, ..., A_nm, B_11, ..., B_1m, ..., B_n1, ..., B_nm),
 * where A_ij=dx_ij/db_j and B_ij=dx_ij/db_i (see HZ).
 * Notice that depending on idxij, some of the A_ij, B_ij might be missing
 *
 */
static void img_projsRTS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pa, *pb, *pqr, *pt, *ppt, *pA, *pB, *Kparms;
  //int n;
  int m, nnz, Asz, Bsz, ABsz, idx;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  Kparms=gl->intrcalib;

  //n=idxij->nr;
  m=idxij->nc;
  pa=p; pb=p+m*cnp;
  Asz=mnp*cnp; Bsz=mnp*pnp; ABsz=Asz+Bsz;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pqr=pa+j*cnp;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=pb + rcsubs[i]*pnp;
      idx=idxij->val[rcidxs[i]];
      pA=jac + idx*ABsz; // set pA to point to A_ij
      pB=pA  + Asz; // set pB to point to B_ij

      calcImgProjJacRTS(Kparms, pqr, pt, ppt, (double (*)[6])pA, (double (*)[3])pB); // evaluate dQ/da, dQ/db in pA, pB
    }
  }
}

/* BUNDLE ADJUSTMENT FOR CAMERA PARAMETERS ONLY */

/* Given a parameter vector p made up of the parameters of m cameras, compute in
 * hx the prediction of the measurements, i.e. the projections of 3D points in the m images.
 * The measurements are returned in the order (hx_11^T, .. hx_1m^T, ..., hx_n1^T, .. hx_nm^T)^T,
 * where hx_ij is the predicted projection of the i-th point on the j-th camera.
 * Notice that depending on idxij, some of the hx_ij might be missing
 *
 */
static void img_projsRT_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pmeas, *Kparms, *ptparams;
  //int n;
  int m, nnz;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  Kparms=gl->intrcalib;
  ptparams=gl->ptparams;

  //n=idxij->nr;
  m=idxij->nc;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pqr=p+j*cnp;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
	    ppt=ptparams + rcsubs[i]*pnp;
      pmeas=hx + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

      calcImgProj(Kparms, pqr, pt, ppt, pmeas); // evaluate Q in pmeas
    }
  }
}

/* Given a parameter vector p made up of the parameters of m cameras, compute in jac
 * the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is returned in the order (A_11, ..., A_1m, ..., A_n1, ..., A_nm),
 * where A_ij=dx_ij/db_j (see HZ).
 * Notice that depending on idxij, some of the A_ij might be missing
 *
 */
static void img_projsRT_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pA, *Kparms, *ptparams;
  //int n;
  int m, nnz, Asz, idx;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  Kparms=gl->intrcalib;
  ptparams=gl->ptparams;

  //n=idxij->nr;
  m=idxij->nc;
  Asz=mnp*cnp;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pqr=p+j*cnp;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=ptparams + rcsubs[i]*pnp;
      idx=idxij->val[rcidxs[i]];
      pA=jac + idx*Asz; // set pA to point to A_ij

      calcImgProjJacRT(Kparms, pqr, pt, ppt, (double (*)[6])pA); // evaluate dQ/da in pA
    }
  }
}

/* BUNDLE ADJUSTMENT FOR STRUCTURE PARAMETERS ONLY */

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * hx the prediction of the measurements, i.e. the projections of 3D points in the m images. The measurements
 * are returned in the order (hx_11^T, .. hx_1m^T, ..., hx_n1^T, .. hx_nm^T)^T, where hx_ij is the predicted
 * projection of the i-th point on the j-th camera.
 * Notice that depending on idxij, some of the hx_ij might be missing
 *
 */
static void img_projsS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pmeas, *Kparms, *camparams;
  //int n;
  int m, nnz;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  Kparms=gl->intrcalib;
  camparams=gl->camparams;

  //n=idxij->nr;
  m=idxij->nc;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pqr=camparams+j*cnp;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=p + rcsubs[i]*pnp;
      pmeas=hx + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

      calcImgProj(Kparms, pqr, pt, ppt, pmeas); // evaluate Q in pmeas
    }
  }
}

/* Given a parameter vector p made up of the 3D coordinates of n points, compute in
 * jac the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is returned in the order (B_11, ..., B_1m, ..., B_n1, ..., B_nm),
 * where B_ij=dx_ij/db_i (see HZ).
 * Notice that depending on idxij, some of the B_ij might be missing
 *
 */
static void img_projsS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pB, *Kparms, *camparams;
  //int n;
  int m, nnz, Bsz, idx;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  Kparms=gl->intrcalib;
  camparams=gl->camparams;

  //n=idxij->nr;
  m=idxij->nc;
  Bsz=mnp*pnp;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pqr=camparams+j*cnp;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=p + rcsubs[i]*pnp;
      idx=idxij->val[rcidxs[i]];
      pB=jac + idx*Bsz; // set pB to point to B_ij

      calcImgProjJacS(Kparms, pqr, pt, ppt, (double (*)[3])pB); // evaluate dQ/da in pB
    }
  }
}

/****************************************************************************************************/
/* MEASUREMENT VECTOR AND JACOBIAN COMPUTATION FOR VARYING CAMERA INTRINSICS, POSE AND 3D STRUCTURE */
/****************************************************************************************************/

/*** MEASUREMENT VECTOR AND JACOBIAN COMPUTATION FOR THE SIMPLE DRIVERS ***/

/* A note about the computation of Jacobians below:
 *
 * When performing BA that includes the camera intrinsics, it would be
 * very desirable to allow for certain parameters such as skew, aspect
 * ratio and principal point to be fixed. The straighforward way to
 * implement this would be to code a separate version of the Jacobian
 * computation routines for each subset of non-fixed parameters. Here,
 * this is bypassed by developing only one set of Jacobian computation
 * routines which estimate the former for all 5 intrinsics and then set
 * the columns corresponding to fixed parameters to zero.
 */

/* FULL BUNDLE ADJUSTMENT, I.E. SIMULTANEOUS ESTIMATION OF CAMERA AND STRUCTURE PARAMETERS */

/* Given the parameter vectors aj and bi of camera j and point i, computes in xij the
 * predicted projection of point i on image j
 */
static void img_projKRTS(int j, int i, double *aj, double *bi, double *xij, void *adata)
{
  calcImgProj(aj, aj+5, aj+5+3, bi, xij); // 5 for the calibration + 3 for the quaternion's vector part
}

/* Given the parameter vectors aj and bi of camera j and point i, computes in Aij, Bij the
 * jacobian of the predicted projection of point i on image j
 */
static void img_projKRTS_jac(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata)
{
struct globs_ *gl;
int ncK;

  calcImgProjJacKRTS(aj, aj+5, aj+5+3, bi, (double (*)[5+6])Aij, (double (*)[3])Bij); // 5 for the calibration + 3 for the quaternion's vector part

  /* clear the columns of the Jacobian corresponding to fixed calibration parameters */
  gl=(struct globs_ *)adata;
  ncK=gl->nccalib;
  if(ncK){
    int cnp, mnp, j0;

    cnp=gl->cnp;
    mnp=gl->mnp;
    j0=5-ncK;

    for(i=0; i<mnp; ++i, Aij+=cnp)
      for(j=j0; j<5; ++j)
        Aij[j]=0.0; // Aij[i*cnp+j]=0.0;
  }
}

/* BUNDLE ADJUSTMENT FOR CAMERA PARAMETERS ONLY */

/* Given the parameter vector aj of camera j, computes in xij the
 * predicted projection of point i on image j
 */
static void img_projKRT(int j, int i, double *aj, double *xij, void *adata)
{
  int pnp;

  double *ptparams;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  pnp=gl->pnp;
  ptparams=gl->ptparams;

  calcImgProj(aj, aj+5, aj+5+3, ptparams+i*pnp, xij); // 5 for the calibration + 3 for the quaternion's vector part
}

/* Given the parameter vector aj of camera j, computes in Aij
 * the jacobian of the predicted projection of point i on image j
 */
static void img_projKRT_jac(int j, int i, double *aj, double *Aij, void *adata)
{
struct globs_ *gl;
double *ptparams;
int pnp, ncK;
  
  gl=(struct globs_ *)adata;
  pnp=gl->pnp;
  ptparams=gl->ptparams;

  calcImgProjJacKRT(aj, aj+5, aj+5+3, ptparams+i*pnp, (double (*)[5+6])Aij); // 5 for the calibration + 3 for the quaternion's vector part

  /* clear the columns of the Jacobian corresponding to fixed calibration parameters */
  ncK=gl->nccalib;
  if(ncK){
    int cnp, mnp, j0;

    cnp=gl->cnp;
    mnp=gl->mnp;
    j0=5-ncK;

    for(i=0; i<mnp; ++i, Aij+=cnp)
      for(j=j0; j<5; ++j)
        Aij[j]=0.0; // Aij[i*cnp+j]=0.0;
  }
}

/* BUNDLE ADJUSTMENT FOR STRUCTURE PARAMETERS ONLY */

/* Given the parameter vector bi of point i, computes in xij the
 * predicted projection of point i on image j
 */
static void img_projKS(int j, int i, double *bi, double *xij, void *adata)
{
  int cnp;

  double *camparams, *aj;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp;
  camparams=gl->camparams;
  aj=camparams+j*cnp;

  calcImgProj(aj, aj+5, aj+5+3, bi, xij); // 5 for the calibration + 3 for the quaternion's vector part
}

/* Given the parameter vector bi of point i, computes in Bij
 * the jacobian of the predicted projection of point i on image j
 */
static void img_projKS_jac(int j, int i, double *bi, double *Bij, void *adata)
{
  int cnp;

  double *camparams, *aj;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp;
  camparams=gl->camparams;
  aj=camparams+j*cnp;

  calcImgProjJacS(aj, aj+5, aj+5+3, bi, (double (*)[3])Bij); // 5 for the calibration + 3 for the quaternion's vector part
}

/*** MEASUREMENT VECTOR AND JACOBIAN COMPUTATION FOR THE EXPERT DRIVERS ***/

/* FULL BUNDLE ADJUSTMENT, I.E. SIMULTANEOUS ESTIMATION OF CAMERA AND STRUCTURE PARAMETERS */

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * hx the prediction of the measurements, i.e. the projections of 3D points in the m images. The measurements
 * are returned in the order (hx_11^T, .. hx_1m^T, ..., hx_n1^T, .. hx_nm^T)^T, where hx_ij is the predicted
 * projection of the i-th point on the j-th camera.
 * Notice that depending on idxij, some of the hx_ij might be missing
 *
 */
static void img_projsKRTS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pa, *pb, *pqr, *pt, *ppt, *pmeas, *pcalib;
  //int n;
  int m, nnz;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;

  //n=idxij->nr;
  m=idxij->nc;
  pa=p; pb=p+m*cnp;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pcalib=pa+j*cnp;
    pqr=pcalib+5;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=pb + rcsubs[i]*pnp;
      pmeas=hx + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

      calcImgProj(pcalib, pqr, pt, ppt, pmeas); // evaluate Q in pmeas
    }
  }
}

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * jac the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is returned in the order (A_11, ..., A_1m, ..., A_n1, ..., A_nm, B_11, ..., B_1m, ..., B_n1, ..., B_nm),
 * where A_ij=dx_ij/db_j and B_ij=dx_ij/db_i (see HZ).
 * Notice that depending on idxij, some of the A_ij, B_ij might be missing
 *
 */
static void img_projsKRTS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
  register int i, j, ii, jj;
  int cnp, pnp, mnp, ncK;
  double *pa, *pb, *pqr, *pt, *ppt, *pA, *pB, *pcalib;
  //int n;
  int m, nnz, Asz, Bsz, ABsz, idx;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  ncK=gl->nccalib;

  //n=idxij->nr;
  m=idxij->nc;
  pa=p; pb=p+m*cnp;
  Asz=mnp*cnp; Bsz=mnp*pnp; ABsz=Asz+Bsz;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pcalib=pa+j*cnp;
    pqr=pcalib+5;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=pb + rcsubs[i]*pnp;
      idx=idxij->val[rcidxs[i]];
      pA=jac + idx*ABsz; // set pA to point to A_ij
      pB=pA  + Asz; // set pB to point to B_ij

      calcImgProjJacKRTS(pcalib, pqr, pt, ppt, (double (*)[5+6])pA, (double (*)[3])pB); // evaluate dQ/da, dQ/db in pA, pB

      /* clear the columns of the Jacobian corresponding to fixed calibration parameters */
      if(ncK){
        int jj0=5-ncK;

        for(ii=0; ii<mnp; ++ii, pA+=cnp)
          for(jj=jj0; jj<5; ++jj)
            pA[jj]=0.0; // pA[ii*cnp+jj]=0.0;
      }
    }
  }
}

/* BUNDLE ADJUSTMENT FOR CAMERA PARAMETERS ONLY */

/* Given a parameter vector p made up of the parameters of m cameras, compute in
 * hx the prediction of the measurements, i.e. the projections of 3D points in the m images.
 * The measurements are returned in the order (hx_11^T, .. hx_1m^T, ..., hx_n1^T, .. hx_nm^T)^T,
 * where hx_ij is the predicted projection of the i-th point on the j-th camera.
 * Notice that depending on idxij, some of the hx_ij might be missing
 *
 */
static void img_projsKRT_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pmeas, *pcalib, *ptparams;
  //int n;
  int m, nnz;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  ptparams=gl->ptparams;

  //n=idxij->nr;
  m=idxij->nc;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pcalib=p+j*cnp;
    pqr=pcalib+5;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
	    ppt=ptparams + rcsubs[i]*pnp;
      pmeas=hx + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

      calcImgProj(pcalib, pqr, pt, ppt, pmeas); // evaluate Q in pmeas
    }
  }
}

/* Given a parameter vector p made up of the parameters of m cameras, compute in jac
 * the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is returned in the order (A_11, ..., A_1m, ..., A_n1, ..., A_nm),
 * where A_ij=dx_ij/db_j (see HZ).
 * Notice that depending on idxij, some of the A_ij might be missing
 *
 */
static void img_projsKRT_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
  register int i, j, ii, jj;
  int cnp, pnp, mnp, ncK;
  double *pqr, *pt, *ppt, *pA, *pcalib, *ptparams;
  //int n;
  int m, nnz, Asz, idx;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  ncK=gl->nccalib;
  ptparams=gl->ptparams;

  //n=idxij->nr;
  m=idxij->nc;
  Asz=mnp*cnp;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pcalib=p+j*cnp;
    pqr=pcalib+5;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=ptparams + rcsubs[i]*pnp;
      idx=idxij->val[rcidxs[i]];
      pA=jac + idx*Asz; // set pA to point to A_ij

      calcImgProjJacKRT(pcalib, pqr, pt, ppt, (double (*)[5+6])pA); // evaluate dQ/da in pA

      /* clear the columns of the Jacobian corresponding to fixed calibration parameters */
      if(ncK){
        int jj0;

        jj0=5-ncK;
        for(ii=0; ii<mnp; ++ii, pA+=cnp)
          for(jj=jj0; jj<5; ++jj)
            pA[jj]=0.0; // pA[ii*cnp+jj]=0.0;
      }
    }
  }
}

/* BUNDLE ADJUSTMENT FOR STRUCTURE PARAMETERS ONLY */

/* Given a parameter vector p made up of the 3D coordinates of n points and the parameters of m cameras, compute in
 * hx the prediction of the measurements, i.e. the projections of 3D points in the m images. The measurements
 * are returned in the order (hx_11^T, .. hx_1m^T, ..., hx_n1^T, .. hx_nm^T)^T, where hx_ij is the predicted
 * projection of the i-th point on the j-th camera.
 * Notice that depending on idxij, some of the hx_ij might be missing
 *
 */
static void img_projsKS_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *hx, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pmeas, *pcalib, *camparams;
  //int n;
  int m, nnz;
  struct globs_ *gl;

  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  camparams=gl->camparams;

  //n=idxij->nr;
  m=idxij->nc;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pcalib=camparams+j*cnp;
    pqr=pcalib+5;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=p + rcsubs[i]*pnp;
      pmeas=hx + idxij->val[rcidxs[i]]*mnp; // set pmeas to point to hx_ij

      calcImgProj(pcalib, pqr, pt, ppt, pmeas); // evaluate Q in pmeas
    }
  }
}

/* Given a parameter vector p made up of the 3D coordinates of n points, compute in
 * jac the jacobian of the predicted measurements, i.e. the jacobian of the projections of 3D points in the m images.
 * The jacobian is returned in the order (B_11, ..., B_1m, ..., B_n1, ..., B_nm),
 * where B_ij=dx_ij/db_i (see HZ).
 * Notice that depending on idxij, some of the B_ij might be missing
 *
 */
static void img_projsKS_jac_x(double *p, struct sba_crsm *idxij, int *rcidxs, int *rcsubs, double *jac, void *adata)
{
  register int i, j;
  int cnp, pnp, mnp;
  double *pqr, *pt, *ppt, *pB, *pcalib, *camparams;
  //int n;
  int m, nnz, Bsz, idx;
  struct globs_ *gl;
  
  gl=(struct globs_ *)adata;
  cnp=gl->cnp; pnp=gl->pnp; mnp=gl->mnp;
  camparams=gl->camparams;

  //n=idxij->nr;
  m=idxij->nc;
  Bsz=mnp*pnp;

  for(j=0; j<m; ++j){
    /* j-th camera parameters */
    pcalib=camparams+j*cnp;
    pqr=pcalib+5;
    pt=pqr+3; // quaternion vector part has 3 elements

    nnz=sba_crsm_col_elmidxs(idxij, j, rcidxs, rcsubs); /* find nonzero hx_ij, i=0...n-1 */

    for(i=0; i<nnz; ++i){
      ppt=p + rcsubs[i]*pnp;
      idx=idxij->val[rcidxs[i]];
      pB=jac + idx*Bsz; // set pB to point to B_ij

      calcImgProjJacS(pcalib, pqr, pt, ppt, (double (*)[3])pB); // evaluate dQ/da in pB
    }
  }
}

/* Driver for sba_xxx_levmar */
void sba_driver(char *camsfname, char *ptsfname, char *calibfname, int cnp, int pnp, int mnp,
                void (*caminfilter)(double *pin, int nin, double *pout, int nout),
                void (*camoutfilter)(double *pin, int nin, double *pout, int nout),
                int filecnp)
{
  double *motstruct, *motstruct_copy, *imgpts, *covimgpts;
  double K[9], ical[5]; // intrinsic calibration matrix & temp. storage for its params
  char *vmask;
  double opts[SBA_OPTSSZ], info[SBA_INFOSZ], phi;
  int howto, expert, analyticjac, fixedcal, n, prnt, verbose=0;
  int nframes, numpts3D, numprojs, nvars;
  const int nconstframes=1;

  static char *howtoname[]={"BA_MOTSTRUCT", "BA_MOT", "BA_STRUCT", "BA_MOT_MOTSTRUCT"};

  clock_t start_time, end_time;


  /* NOTE: readInitialSBAEstimate() sets covimgpts to NULL if no covariances are supplied */
  readInitialSBAEstimate(camsfname, ptsfname, cnp, pnp, mnp, caminfilter, filecnp, //NULL, 0, 
                         &nframes, &numpts3D, &numprojs, &motstruct, &imgpts, &covimgpts, &vmask);

  //printSBAData(motstruct, cnp, pnp, mnp, camoutfilter, filecnp, nframes, numpts3D, imgpts, numprojs, vmask);

  /* set up globs structure */
  globs.cnp=cnp; globs.pnp=pnp; globs.mnp=mnp;
  if(calibfname){ // read intrinsics only if fixed for all cameras
    readCalibParams(calibfname, K);
    ical[0]=K[0]; // fu
    ical[1]=K[2]; // u0
    ical[2]=K[5]; // v0
    ical[3]=K[4]/K[0]; // ar
    ical[4]=K[1]; // s
    globs.intrcalib=ical;
    fixedcal=1; /* fixed intrinsics */
  }
  else{ // intrinsics are to be found in the cameras parameter file
    globs.intrcalib=NULL;
    /* specify the number of intrinsic parameters that are to be fixed
     * equal to their initial values, as follows:
     * 0: all free, 1: skew fixed, 2: skew, ar fixed, 4: skew, ar, ppt fixed
     * Note that a value of 3 does not make sense
     */
    globs.nccalib=1;
    fixedcal=0; /* varying intrinsics */
  }

  globs.ptparams=NULL;
  globs.camparams=NULL;

  /* call sparse LM routine */
  opts[0]=SBA_INIT_MU; opts[1]=SBA_STOP_THRESH; opts[2]=SBA_STOP_THRESH;
  opts[3]=SBA_STOP_THRESH;
  //opts[3]=0.05*numprojs; // uncomment to force termination if the average reprojection error drops below 0.05
  opts[4]=0.0;
  //opts[4]=1E-05; // uncomment to force termination if the relative reduction in the RMS reprojection error drops below 1E-05

  /* Notice the various BA options demonstrated below */

  /* minimize motion & structure, motion only, or
   * motion and possibly motion & structure in a 2nd pass?
   */
  howto=BA_MOTSTRUCT;
  //howto=BA_MOT;
  //howto=BA_STRUCT;
  //howto=BA_MOT_MOTSTRUCT;

  /* simple or expert drivers? */
  //expert=0;
  expert=1;

  /* analytic or approximate jacobian? */
  //analyticjac=0;
  analyticjac=1;

  /* print motion & structure estimates,
   * motion only or structure only upon completion?
   */
  prnt=BA_NONE;
  //prnt=BA_MOTSTRUCT;
  //prnt=BA_MOT;
  //prnt=BA_STRUCT;

  start_time=clock();
  switch(howto){
    case BA_MOTSTRUCT: /* BA for motion & structure */
      nvars=nframes*cnp+numpts3D*pnp;
      if(expert)
        n=sba_motstr_levmar_x(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, pnp, imgpts, covimgpts, mnp,
                            fixedcal? img_projsRTS_x : img_projsKRTS_x, analyticjac? (fixedcal? img_projsRTS_jac_x : img_projsKRTS_jac_x) : NULL,
                            (void *)(&globs), 150, verbose, opts, info);
      else
        n=sba_motstr_levmar(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, pnp, imgpts, covimgpts, mnp,
                            fixedcal? img_projRTS : img_projKRTS, analyticjac? (fixedcal? img_projRTS_jac : img_projKRTS_jac) : NULL,
                            (void *)(&globs), 150, verbose, opts, info);
    break;

    case BA_MOT: /* BA for motion only */
      globs.ptparams=motstruct+nframes*cnp;
      nvars=nframes*cnp;
      if(expert)
        n=sba_mot_levmar_x(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, imgpts, covimgpts, mnp,
                          fixedcal? img_projsRT_x : img_projsKRT_x, analyticjac? (fixedcal? img_projsRT_jac_x : img_projsKRT_jac_x) : NULL,
                          (void *)(&globs), MAXITER, verbose, opts, info);
      else
        n=sba_mot_levmar(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, imgpts, covimgpts, mnp,
                          fixedcal? img_projRT : img_projKRT, analyticjac? (fixedcal? img_projRT_jac : img_projKRT_jac) : NULL,
                          (void *)(&globs), MAXITER, verbose, opts, info);
    break;

    case BA_STRUCT: /* BA for structure only */
      globs.camparams=motstruct;
      nvars=numpts3D*pnp;
      if(expert)
        n=sba_str_levmar_x(numpts3D, nframes, vmask, motstruct+nframes*cnp, pnp, imgpts, covimgpts, mnp,
                          fixedcal? img_projsS_x : img_projsKS_x, analyticjac? (fixedcal? img_projsS_jac_x : img_projsKS_jac_x) : NULL,
                          (void *)(&globs), MAXITER, verbose, opts, info);
      else
        n=sba_str_levmar(numpts3D, nframes, vmask, motstruct+nframes*cnp, pnp, imgpts, covimgpts, mnp,
                          fixedcal? img_projS : img_projKS, analyticjac? (fixedcal? img_projS_jac : img_projKS_jac) : NULL,
                          (void *)(&globs), MAXITER, verbose, opts, info);
    break;

    case BA_MOT_MOTSTRUCT: /* BA for motion only; if error too large, then BA for motion & structure */
      if((motstruct_copy=(double *)malloc((nframes*cnp + numpts3D*pnp)*sizeof(double)))==NULL){
        fprintf(stderr, "memory allocation failed in sba_driver()\n");
        exit(1);
      }

      memcpy(motstruct_copy, motstruct, (nframes*cnp + numpts3D*pnp)*sizeof(double)); // save starting point for later use
      globs.ptparams=motstruct+nframes*cnp;
      nvars=nframes*cnp;

      if(expert)
        n=sba_mot_levmar_x(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, imgpts, covimgpts, mnp,
                          fixedcal? img_projsRT_x : img_projsKRT_x, analyticjac? (fixedcal? img_projsRT_jac_x : img_projsKRT_jac_x) : NULL,
                          (void *)(&globs), MAXITER, verbose, opts, info);
      else
        n=sba_mot_levmar(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, imgpts, covimgpts, mnp,
                        fixedcal? img_projRT : img_projKRT, analyticjac? (fixedcal? img_projRT_jac : img_projKRT_jac) : NULL,
                        (void *)(&globs), MAXITER, verbose, opts, info);

      if((phi=info[1]/numprojs)>SBA_MAX_REPROJ_ERROR){
        fflush(stdout); fprintf(stdout, "Refining structure (motion only error %g)...\n", phi); fflush(stdout);
        memcpy(motstruct, motstruct_copy, (nframes*cnp + numpts3D*pnp)*sizeof(double)); // reset starting point

        if(expert)
          n=sba_motstr_levmar_x(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, pnp, imgpts, NULL, mnp,
                                fixedcal? img_projsRTS_x : img_projsKRTS_x, analyticjac? (fixedcal? img_projsRTS_jac_x : img_projsKRTS_jac_x) : NULL,
                                (void *)(&globs), 150, verbose, opts, info);
        else
          n=sba_motstr_levmar(numpts3D, nframes, nconstframes, vmask, motstruct, cnp, pnp, imgpts, NULL, mnp,
                              fixedcal? img_projRTS : img_projKRTS, analyticjac? (fixedcal? img_projRTS_jac : img_projKRTS_jac) : NULL,
                              (void *)(&globs), 150, verbose, opts, info);
      }
      free(motstruct_copy);

    break;

    default:
      fprintf(stderr, "unknown BA method \"%d\" in sba_driver()!\n", howto);
      exit(1);
  }
  end_time=clock();
  

	fflush(stdout);
  fprintf(stdout, "SBA using %d 3D pts, %d frames and %d image projections, %d variables\n", numpts3D, nframes, numprojs, nvars);
  fprintf(stdout, "\nMethod %s, %s driver, %s Jacobian, %s intrinsics, %s covariances", howtoname[howto],
                  expert? "expert" : "simple",
                  analyticjac? "analytic" : "approximate",
                  fixedcal? "fixed" : "variable",
                  covimgpts? "with" : "without");
  if(globs.nccalib) fprintf(stdout, " (%d fixed)", globs.nccalib);
  fputs("\n\n", stdout); 
  fprintf(stdout, "SBA returned %d in %g iter, reason %g, error %g [initial %g], %d/%d func/fjac evals, %d lin. systems\n", n,
                    info[5], info[6], info[1]/numprojs, info[0]/numprojs, (int)info[7], (int)info[8], (int)info[9]);
  fprintf(stdout, "Elapsed time: %.2lf seconds, %.2lf msecs\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC,
                  ((double) (end_time - start_time)) / CLOCKS_PER_MSEC);
  fflush(stdout);

  /* refined motion and structure are now in motstruct */
  switch(prnt){
    case BA_NONE:
    break;

    case BA_MOTSTRUCT:
      printSBAMotionData(motstruct, nframes, cnp, camoutfilter, filecnp);
      printSBAStructureData(motstruct, nframes, numpts3D, cnp, pnp);
    break;

    case BA_MOT:
      printSBAMotionData(motstruct, nframes, cnp, camoutfilter, filecnp);
    break;

    case BA_STRUCT:
      printSBAStructureData(motstruct, nframes, numpts3D, cnp, pnp);
    break;

    default:
      fprintf(stderr, "unknown print option \"%d\" in sba_driver()!\n", prnt);
      exit(1);
  }

  //printSBAData(motstruct, cnp, pnp, mnp, camoutfilter, filecnp, nframes, numpts3D, imgpts, numprojs, vmask);

  /* just in case... */
  globs.intrcalib=NULL;
  globs.nccalib=0;

  free(motstruct);
  free(imgpts);
  if(covimgpts) free(covimgpts);
  free(vmask);
}

/* convert a vector of camera parameters so that rotation is represented by
 * the vector part of the input quaternion. The function converts the
 * input quaternion into a unit one with a positive scalar part. Remaining
 * parameters are left unchanged.
 *
 * Input parameter layout: intrinsics (5, optional), rot. quaternion (4), translation (3)
 * Output parameter layout: intrinsics (5, optional), rot. quaternion vector part (3), translation (3)
 */
void quat2vec(double *inp, int nin, double *outp, int nout)
{
double mag, sg;
register int i;

  /* intrinsics */
  if(nin==5+7) // are they present?
    for(i=0; i<5; ++i)
      outp[i]=inp[i];
  else
    i=0;

  /* rotation */
  /* normalize and ensure that the quaternion's scalar component is positive; if not,
   * negate the quaternion since two quaternions q and -q represent the same
   * rotation
   */
  mag=sqrt(inp[i]*inp[i] + inp[i+1]*inp[i+1] + inp[i+2]*inp[i+2] + inp[i+3]*inp[i+3]);
  sg=(inp[i]>=0.0)? 1.0 : -1.0;
  mag=sg/mag;
  outp[i]  =inp[i+1]*mag;
  outp[i+1]=inp[i+2]*mag;
  outp[i+2]=inp[i+3]*mag;
  i+=3;

  /* translation*/
  for( ; i<nout; ++i)
    outp[i]=inp[i+1];
}

/* convert a vector of camera parameters so that rotation is represented by
 * a full unit quaternion instead of its input 3-vector part. Remaining
 * parameters are left unchanged.
 *
 * Input parameter layout: intrinsics (5, optional), rot. quaternion vector part (3), translation (3)
 * Output parameter layout: intrinsics (5, optional), rot. quaternion (4), translation (3)
 */
void vec2quat(double *inp, int nin, double *outp, int nout)
{
double w;
register int i;

  /* intrinsics */
  if(nin==5+7-1) // are they present?
    for(i=0; i<5; ++i)
      outp[i]=inp[i];
  else
    i=0;

  /* rotation */
  /* recover the quaternion's scalar component from the vector one */
  w=sqrt(1.0 - (inp[i]*inp[i] + inp[i+1]*inp[i+1] + inp[i+2]*inp[i+2]));
  outp[i]  =w;
  outp[i+1]=inp[i];
  outp[i+2]=inp[i+1];
  outp[i+3]=inp[i+2];
  i+=4;

  /* translation */
  for( ; i<nout; ++i)
    outp[i]=inp[i-1];
}


int main(int argc, char *argv[])
{
int cnp=6, /* 3 rot params + 3 trans params */
    pnp=3, /* euclidean 3D points */
    mnp=2; /* image points are 2D */

  if(argc!=3 && argc!=4){
    fprintf(stderr, "Usage is %s <camera params> <point params> [<intrinsic calibration params>]\n", argv[0]);
    exit(1);
  }

  if(argc==4){
    fprintf(stderr, "Starting BA with fixed intrinsic parameters\n");
    sba_driver(argv[1], argv[2], argv[3], cnp, pnp, mnp, quat2vec, vec2quat, cnp+1);
  }
  else{
    fprintf(stderr, "Starting BA with varying intrinsic parameters\n");
    cnp+=5; /* 5 more params for calibration */
    sba_driver(argv[1], argv[2], NULL, cnp, pnp, mnp, quat2vec, vec2quat, cnp+1);
  }

  return 0;
}
