/* ////////////////////////////////////////////////////////////////////////////////
// 
//  Matlab MEX file for sba's simple drivers
//  Copyright (C) 2007-2008 Manolis Lourakis (lourakis at ics forth gr)
//  Institute of Computer Science, Foundation for Research & Technology - Hellas
//  Heraklion, Crete, Greece.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//////////////////////////////////////////////////////////////////////////////// */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include <time.h>

#include <sba.h>

#include "mex.h"

/**
#define DEBUG
**/

#define _MAX_(A, B)     ((A)>=(B)? (A) : (B))
#define _MIN_(A, B)     ((A)<=(B)? (A) : (B))

#define MININARGS      12
#define MINOUTARGS     2

#define BA_MOTSTRUCT            0
#define BA_MOT                  1
#define BA_STRUCT               2


#define CLOCKS_PER_MSEC (CLOCKS_PER_SEC/1000.0)

#define MAXNAMELEN              256


#ifdef WIN32
#include <windows.h>

#define LOADFUNCTION GetProcAddress
#define FUNCTIONISVALID(ptr) ((ptr)!=NULL)

typedef HINSTANCE LibHandle;

#else /* !WIN32, assume Un*x */
#include <dlfcn.h>

#define LOADFUNCTION dlsym
#define FUNCTIONISVALID(ptr) (dlerror()==NULL)

typedef void *LibHandle;

#endif /* WIN32 */

struct mexdata {
  /* matlab or dynlib names of the fitting function & its Jacobian */
  char projname[MAXNAMELEN], projacname[MAXNAMELEN];

  /* binary flags specifying if input p0 is a row or column vector */
  int isrow_p0;

  /* rhs args to be passed to matlab. note that rhs[0], rhs[1] are reserved
   * for passing the camera & structure parameter vectors, respectively.
   * If present, problem-specific data are passed in rhs[2], rhs[3], ... 
   */
  mxArray **rhs;
  int nrhs; /* >= 2 */

  /* dynamic libraries stuff */
  LibHandle projlibhandle, projaclibhandle; /* handles */
  double **dynadata; /* optional additional data to be passed to the dynlib functions */
  char projlibname[MAXNAMELEN], projaclibname[MAXNAMELEN]; /* filenames */


  /* problem dimension parameters */
  int cnp, pnp, mnp;

  /* pointers to camera & structure parameters */
  double *pa, *pb;
};


/* display printf-style error messages in matlab */
static void matlabFmtdErrMsgTxt(char *fmt, ...)
{
char  buf[256];
va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

  mexErrMsgTxt(buf);
}

/* display printf-style warning messages in matlab */
static void matlabFmtdWarnMsgTxt(char *fmt, ...)
{
char  buf[256];
va_list args;

	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	va_end(args);

  mexWarnMsgTxt(buf);
}

#if 0
/* matlab matrices are in column-major, this routine converts them to row major for sba */
static double *getTransposeDbl(mxArray *Am)
{
int m, n;
double *At, *A;
register int i, j;

  m=mxGetM(Am);
  n=mxGetN(Am);
  A=mxGetPr(Am);
  At=mxMalloc(m*n*sizeof(double));

  for(i=0; i<m; ++i)
    for(j=0; j<n; ++j)
      At[i*n+j]=A[i+j*m];
  
  return At;
}
#endif

/* get visibility mask from a dense matlab array */
static char *getVMaskDense(mxArray *Am)
{
int m, n;
double *A;
char *At;
register int i, j;

  m=mxGetM(Am);
  n=mxGetN(Am);
  A=mxGetPr(Am);
  At=mxMalloc(m*n*sizeof(char));

  for(i=0; i<m; ++i)
    for(j=0; j<n; ++j)
      At[i*n+j]=(A[i+j*m])? 1 : 0;
  
  return At;
}

/* get visibility mask from a sparse matlab array */
/* matlab stores sparse arrays in the Compressed Column Storage (CCS) format */
char *getVMaskSparse(mxArray *Am)
{
int m, n, *rowidx, *colptr;
/* int nnz=mxGetNzmax(Am); */
double *val;
register int i, j;
char *At;

  m=mxGetM(Am);
  n=mxGetN(Am);
  val=mxGetPr(Am);
  rowidx=mxGetIr(Am);
  colptr=mxGetJc(Am);

  At=mxMalloc(m*n*sizeof(char));
  for(i=0; i<m*n; ++i)
    At[i]=0;

  for(j=0; j<n; ++j)
    for(i=colptr[j]; i<colptr[j+1]; ++i){
      /* element rowidx[i], j equals val[i]. matlab's sparse matrix indices are zero based */
      if(val[i]!=0.0){
        /* printf("(%d %d): %g\n", rowidx[i], j, val[i]); */
        At[rowidx[i]*n+j]=1;
      }
    }

  return At;
}

/* memory copy (i.e., x=y) using loop unrolling and blocking.
 * see http://www.abarnett.demon.co.uk/tutorial.html
 */
#define UNROLLBLOCKSIZE  8 
static void dblcopy_(double *x, double *y, int n)
{ 
register int i=0; 
int blockn;

  /* n may not be divisible by UNROLLBLOCKSIZE, 
  * go as near as we can first, then tidy up.
  */ 
  blockn=(n/UNROLLBLOCKSIZE)*UNROLLBLOCKSIZE; 

  /* unroll the loop in blocks of `UNROLLBLOCKSIZE' */ 
  while(i<blockn) { 
    x[i]  =y[i];
    x[i+1]=y[i+1];
    x[i+2]=y[i+2];
    x[i+3]=y[i+3];
    x[i+4]=y[i+4];
    x[i+5]=y[i+5];
    x[i+6]=y[i+6];
    x[i+7]=y[i+7];

    /* update the counter */ 
    i+=UNROLLBLOCKSIZE;
  } 

 /* 
  * There may be some left to do.
  * This could be done as a simple for() loop, 
  * but a switch is faster (and more interesting) 
  */ 

  if(i<n){ 
    /* Jump into the case at the place that will allow
     * us to finish off the appropriate number of items. 
     */ 

    switch(n-i){ 
      case 7 : x[i]=y[i]; ++i;
      case 6 : x[i]=y[i]; ++i;
      case 5 : x[i]=y[i]; ++i;
      case 4 : x[i]=y[i]; ++i;
      case 3 : x[i]=y[i]; ++i;
      case 2 : x[i]=y[i]; ++i;
      case 1 : x[i]=y[i]; ++i;
    }
  }

  return;
}

/** next 7 functions handle user projection & Jacobian functions coded in Matlab **/
static void proj_motstrMATLAB(int j, int i, double *aj, double *bi, double *xij, void *adata)
{
mxArray *lhs[1];
register double *mp;
register int k;
struct mexdata *dat=(struct mexdata *)adata;

  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->projname);

  /* copy back results & cleanup */
  mp=mxGetPr(lhs[0]);
  for(k=0; k<dat->mnp; ++k)
    xij[k]=mp[k];

  /* delete the vector created by matlab */
  mxDestroyArray(lhs[0]);
}

static void projac_motstrMATLAB(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata)
{
mxArray *lhs[2];
register double *mp;
register int k;
struct mexdata *dat=(struct mexdata *)adata;

  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  /* invoke matlab */
  mexCallMATLAB(2, lhs, dat->nrhs, dat->rhs, dat->projacname);

  /* copy back results & cleanup. Note that the Jacobians
   * computed by matlab are returned in row-major as vectors
   */
  mp=mxGetPr(lhs[0]);
  /*
  for(k=0; k<dat->mnp*dat->cnp; ++k)
    Aij[k]=mp[k];
  */
  dblcopy_(Aij, mp, dat->mnp*dat->cnp);

  mp=mxGetPr(lhs[1]);
  /*
  for(k=0; k<dat->mnp*dat->pnp; ++k)
    Bij[k]=mp[k];
  */
  dblcopy_(Bij, mp, dat->mnp*dat->pnp);

  /* delete the vectors created by matlab */
  mxDestroyArray(lhs[0]);
  mxDestroyArray(lhs[1]);
}


static void proj_motMATLAB(int j, int i, double *aj, double *xij, void *adata)
{
mxArray *lhs[1];
register double *mp;
double *bi;
register int k;
struct mexdata *dat=(struct mexdata *)adata;

  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  bi=dat->pb + i*dat->pnp;
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->projname);

  /* copy back results & cleanup */
  mp=mxGetPr(lhs[0]);
  for(k=0; k<dat->mnp; ++k)
    xij[k]=mp[k];

  /* delete the vector created by matlab */
  mxDestroyArray(lhs[0]);
}

static void projac_motMATLAB(int j, int i, double *aj, double *Aij, void *adata)
{
mxArray *lhs[1];
register double *mp;
double *bi;
register int k;
struct mexdata *dat=(struct mexdata *)adata;

  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  bi=dat->pb + i*dat->pnp;
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->projacname);

  /* copy back results & cleanup. Note that the Jacobian 
   * computed by matlab is returned in row-major as a vector
   */
  mp=mxGetPr(lhs[0]);
  /*
  for(k=0; k<dat->mnp*dat->cnp; ++k)
    Aij[k]=mp[k];
  */
  dblcopy_(Aij, mp, dat->mnp*dat->cnp);

  /* delete the vector created by matlab */
  mxDestroyArray(lhs[0]);
}

static void proj_strMATLAB(int j, int i, double *bi, double *xij, void *adata)
{
mxArray *lhs[1];
register double *mp;
double *aj;
register int k;
struct mexdata *dat=(struct mexdata *)adata;

  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  aj=dat->pa + j*dat->cnp;
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->projname);

  /* copy back results & cleanup */
  mp=mxGetPr(lhs[0]);
  for(k=0; k<dat->mnp; ++k)
    xij[k]=mp[k];

  /* delete the vector created by matlab */
  mxDestroyArray(lhs[0]);
}

static void projac_strMATLAB(int j, int i, double *bi, double *Bij, void *adata)
{
mxArray *lhs[1];
register double *mp;
double *aj;
register int k;
struct mexdata *dat=(struct mexdata *)adata;

  /* prepare to call matlab */
  mp=mxGetPr(dat->rhs[0]);
  aj=dat->pa + j*dat->cnp;
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  /* invoke matlab */
  mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->projacname);

  /* copy back results & cleanup. Note that the Jacobian 
   * computed by matlab is returned in row-major as a vector
   */
  mp=mxGetPr(lhs[0]);
  /*
  for(k=0; k<dat->mnp*dat->pnp; ++k)
    Bij[k]=mp[k];
  */
  dblcopy_(Bij, mp, dat->mnp*dat->pnp);

  /* delete the vector created by matlab */
  mxDestroyArray(lhs[0]);
}

/* check the supplied matlab projection function and its Jacobian. Returns 1 on error, 0 otherwise */
static int checkFuncAndJacobianMATLAB(double *aj, double *bi, int chkproj, int chkjac, int mintype, struct mexdata *dat)
{
mxArray *lhs[2]={NULL, NULL};
register int k;
int nlhs, ret=0;
double *mp;

  mexSetTrapFlag(1); /* handle errors in the MEX-file */

  mp=mxGetPr(dat->rhs[0]);
  for(k=0; k<dat->cnp; ++k)
    mp[k]=aj[k];

  mp=mxGetPr(dat->rhs[1]);
  for(k=0; k<dat->pnp; ++k)
    mp[k]=bi[k];

  if(chkproj){
    /* attempt to call the supplied proj */
    k=mexCallMATLAB(1, lhs, dat->nrhs, dat->rhs, dat->projname);
    if(k){
      fprintf(stderr, "sba: error calling '%s'.\n", dat->projname);
      ret=1;
    }
    else if(!lhs[0] || !mxIsDouble(lhs[0]) || mxIsComplex(lhs[0]) || !(mxGetM(lhs[0])==1 || mxGetN(lhs[0])==1) ||
        _MAX_(mxGetM(lhs[0]), mxGetN(lhs[0]))!=dat->mnp){
      fprintf(stderr, "sba: '%s' should produce a real vector with %d elements (got %d).\n",
                      dat->projname, dat->mnp, _MAX_(mxGetM(lhs[0]), mxGetN(lhs[0])));
      ret=1;
    }
    /* delete the vector created by matlab */
    mxDestroyArray(lhs[0]);
  }

  if(chkjac){
    lhs[0]=lhs[1]=NULL;
    nlhs=(mintype==BA_MOTSTRUCT)? 2 : 1;
    /* attempt to call the supplied jac */
    k=mexCallMATLAB(nlhs, lhs, dat->nrhs, dat->rhs, dat->projacname);
    if(k){
      fprintf(stderr, "sba: error calling '%s'.\n", dat->projacname);
      ret=1;
    }
    else if(mintype==BA_MOTSTRUCT || mintype==BA_MOT){
      if(!lhs[0] || !mxIsDouble(lhs[0]) || mxIsComplex(lhs[0]) ||
                    _MIN_(mxGetM(lhs[0]), mxGetN(lhs[0]))!=1 || 
                    _MAX_(mxGetM(lhs[0]), mxGetN(lhs[0]))!=dat->mnp*dat->cnp){
        fprintf(stderr, "sba: '%s' should produce a real %d row or column vector as its first output arg (got %dx%d).\n",
                        dat->projacname, dat->mnp*dat->cnp, mxGetM(lhs[0]), mxGetN(lhs[0]));
        ret=1;
      }
    }
    else{ /* BA_STRUCT */
      if(!lhs[0] || !mxIsDouble(lhs[0]) || mxIsComplex(lhs[0]) ||
                    _MIN_(mxGetM(lhs[0]), mxGetN(lhs[0]))!=1 ||
                    _MAX_(mxGetM(lhs[0]), mxGetN(lhs[0]))!=dat->mnp*dat->pnp){
        fprintf(stderr, "sba: '%s' should produce a real %d row or column vector as its first output arg (got %dx%d).\n",
                        dat->projacname, dat->mnp*dat->pnp, mxGetM(lhs[0]), mxGetN(lhs[0]));
        ret=1;
      }
    }

    if(lhs[0] && mxIsSparse(lhs[0])){
      fprintf(stderr, "sba: '%s' should produce a real dense vector as its first output arg, not a sparse one.\n");
      ret=1;
    }

    if(nlhs==2){ /* BA_MOTSTRUCT */
      if(!lhs[1] || !mxIsDouble(lhs[1]) || mxIsComplex(lhs[1]) ||
                    _MIN_(mxGetM(lhs[1]), mxGetN(lhs[1]))!=1 ||
                    _MAX_(mxGetM(lhs[1]), mxGetN(lhs[1]))!=dat->mnp*dat->pnp){
        fprintf(stderr, "sba: '%s' should produce a real %d row or column vector as its second output arg (got %dx%d).\n",
                      dat->projacname, dat->mnp*dat->pnp, mxGetM(lhs[1]), mxGetN(lhs[1]));
        ret=1;
      }
      else if(lhs[1] && mxIsSparse(lhs[1])){
        fprintf(stderr, "sba: '%s' should produce a real dense vector as its second output arg, not a sparse one.\n");
        ret=1;
      }
    }

    /* delete the vectors created by matlab */
    for(k=0; k<nlhs; ++k)
      mxDestroyArray(lhs[k]);
  }

  mexSetTrapFlag(0); /* on error terminate the MEX-file and return control to the MATLAB prompt */

  return ret;
}


/** next 6 functions handle user projection & Jacobian functions coming from dynlibs **/
static void proj_motstrDL(int j, int i, double *aj, double *bi, double *xij, void *adata)
{
struct mexdata *dat=(struct mexdata *)adata;
typedef void (*projMS)(double *aj, double *bi, double *xij, double **adata);
projMS proj;

  /* get pointer to function... */
  proj=(projMS)LOADFUNCTION(dat->projlibhandle, dat->projname);
  if(!FUNCTIONISVALID(proj))
    matlabFmtdErrMsgTxt("sba: error loading projection function '%s' from dynamic library %s\n", dat->projname, dat->projlibname);

  /* ...and call it */
  (*proj)(aj, bi, xij, dat->dynadata);
}

static void projac_motstrDL(int j, int i, double *aj, double *bi, double *Aij, double *Bij, void *adata)
{
struct mexdata *dat=(struct mexdata *)adata;
typedef void (*projacMS)(double *aj, double *bi, double *Aij, double *Bij, double **adata);
projacMS projac;

  /* get pointer to function... */
  projac=(projacMS)LOADFUNCTION(dat->projaclibhandle, dat->projacname);
  if(!FUNCTIONISVALID(projac))
    matlabFmtdErrMsgTxt("sba: error loading Jacobian function '%s' from dynamic library %s\n", dat->projacname, dat->projaclibname);

  /* ...and call it */
  (*projac)(aj, bi, Aij, Bij, dat->dynadata);
}


static void proj_motDL(int j, int i, double *aj, double *xij, void *adata)
{
struct mexdata *dat=(struct mexdata *)adata;
typedef void (*projM)(double *aj, double *bi, double *xij, double **adata);
projM proj;
double *bi;

  /* get pointer to function... */
  proj=(projM)LOADFUNCTION(dat->projlibhandle, dat->projname);
  if(!FUNCTIONISVALID(proj))
    matlabFmtdErrMsgTxt("sba: error loading projection function '%s' from dynamic library %s\n", dat->projname, dat->projlibname);

  /* ...and call it */
  bi=dat->pb + i*dat->pnp;
  (*proj)(aj, bi, xij, dat->dynadata);
}

static void projac_motDL(int j, int i, double *aj, double *Aij, void *adata)
{
struct mexdata *dat=(struct mexdata *)adata;
typedef void (*projacM)(double *aj, double *bi, double *Aij, double **adata);
projacM projac;
double *bi;

  /* get pointer to function... */
  projac=(projacM)LOADFUNCTION(dat->projaclibhandle, dat->projacname);
  if(!FUNCTIONISVALID(projac))
    matlabFmtdErrMsgTxt("sba: error loading Jacobian function '%s' from dynamic library %s\n", dat->projacname, dat->projaclibname);

  /* ...and call it */
  bi=dat->pb + i*dat->pnp;
  (*projac)(aj, bi, Aij, dat->dynadata);
}

static void proj_strDL(int j, int i, double *bi, double *xij, void *adata)
{
struct mexdata *dat=(struct mexdata *)adata;
typedef void (*projS)(double *aj, double *bi, double *xij, double **adata);
projS proj;
double *aj;

  /* get pointer to function... */
  proj=(projS)LOADFUNCTION(dat->projlibhandle, dat->projname);
  if(!FUNCTIONISVALID(proj))
    matlabFmtdErrMsgTxt("sba: error loading projection function '%s' from dynamic library %s\n", dat->projname, dat->projlibname);

  /* ...and call it */
  aj=dat->pa + j*dat->cnp;
  (*proj)(aj, bi, xij, dat->dynadata);
}

static void projac_strDL(int j, int i, double *bi, double *Bij, void *adata)
{
struct mexdata *dat=(struct mexdata *)adata;
typedef void (*projacS)(double *aj, double *bi, double *Bij, double **adata);
projacS projac;
double *aj;

  /* get pointer to function... */
  projac=(projacS)LOADFUNCTION(dat->projaclibhandle, dat->projacname);
  if(!FUNCTIONISVALID(projac))
    matlabFmtdErrMsgTxt("sba: error loading Jacobian function '%s' from dynamic library %s\n", dat->projacname, dat->projaclibname);

  /* ...and call it */
  aj=dat->pa + j*dat->cnp;
  (*projac)(aj, bi, Bij, dat->dynadata);
}

/*
 
[ret, p, info]=sba(n, m, mcon, vmask, p, cnp, pnp, x, covx, mnp, proj, projac, itmax, verbose, opts, reftype, ...);

    Most arguments are straightforward to explain, please refer to the description of their homonymous ones
    in the C version. Below, arguments that have a special meaning in matlab are discussed in more detail.

  * vmask is a nxm matrix specifying the points visible in each camera. It can be either a dense or a sparse
    matrix; in both cases, nonzero elements indicate that a certain point is visible from the corresponding
    camera.

  * reftype specifies the type of refinement to be carried out and can be one of:
      'motstr' % refinement of motion & structure, default
      'mot'    % refinement of motion only
      'str'    % refinement of structure only (mcon is redundant in this case)
    if reftype is omitted, 'motstr is assumed'.

  * proj is a matlab function that computes the projection on camera j with parameters aj
    of point i with parameters bi; it should accept at least two input args: xij=proj(aj, bi);
    Any additional arguments passed to sba, are passed unaltered to each invocation of proj.
    This technique has been used in the demonstrated example to pass the intrinsic calibration
    parameters.

  * projac is a matlab function computing the Jacobian of proj, as follows:
    if reftype equals 'motstr', then [Aij, Bij]=projac(aj, bi);
    if reftype equals 'mot', then Aij=projac(aj, bi);
    if reftype equals 'str', then Bij=projac(aj, bi);
    Aij denotes the Jacobian of xij with respect to aj, Bij the Jacobian of xij with respect to bi.
    Any additional arguments passed to sba, are passed unaltered to each invocation of projac.

*/

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *Prhs[])
{
register int i;
int n, m, mcon, cnp, pnp, mnp, nvars, nprojs, minnvars;
int status, reftype=BA_MOTSTRUCT, itmax, verbose=0, havejac, havedynproj, havedynprojac;
int len, nopts, nextra, nreserved, covlen;
double *p0, *p, *x, *covx=NULL;
double opts[SBA_OPTSSZ]={SBA_INIT_MU, SBA_STOP_THRESH, SBA_STOP_THRESH, SBA_STOP_THRESH, 0.0};
double info[SBA_INFOSZ];
char *vmask, *str;
register double *pdbl;
mxArray **prhs=(mxArray **)&Prhs[0];
struct mexdata mdata;
const int min1=MININARGS-1;

static char *reftypename[]={"motion & structure", "motion only", "structure only"};

clock_t start_time, end_time;

  /* parse input args; start by checking their number */
  if(nrhs<MININARGS)
    matlabFmtdErrMsgTxt("sba: at least %d input arguments required (got %d).", MININARGS, nrhs);
  if(nlhs<MINOUTARGS)
    matlabFmtdErrMsgTxt("sba: at least %d output arguments required (got %d).", MINOUTARGS, nlhs);
    
  /** n **/
  /* the first argument must be a scalar */
  if(!mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || mxGetM(prhs[0])!=1 || mxGetN(prhs[0])!=1)
    mexErrMsgTxt("sba: n must be a scalar.");
  n=(int)mxGetScalar(prhs[0]);

  /** m **/
  /* the second argument must be a scalar */
  if(!mxIsDouble(prhs[1]) || mxIsComplex(prhs[1]) || mxGetM(prhs[1])!=1 || mxGetN(prhs[1])!=1)
    mexErrMsgTxt("sba: m must be a scalar.");
  m=(int)mxGetScalar(prhs[1]);

  /** mcon **/
  /* the third argument must be a scalar */
  if(!mxIsDouble(prhs[2]) || mxIsComplex(prhs[2]) || mxGetM(prhs[2])!=1 || mxGetN(prhs[2])!=1)
    mexErrMsgTxt("sba: mcon must be a scalar.");
  mcon=(int)mxGetScalar(prhs[2]);

  /** mask **/
  /* the fourth argument must be a nxm matrix */
  if(!mxIsDouble(prhs[3]) || mxIsComplex(prhs[3]) || mxGetM(prhs[3])!=n || mxGetN(prhs[3])!=m)
    matlabFmtdErrMsgTxt("sba: mask must be a %dx%d matrix (got %dx%d).", n, m, mxGetM(prhs[3]), mxGetN(prhs[3]));
  if(mxIsSparse(prhs[3])) vmask=getVMaskSparse(prhs[3]);
  else vmask=getVMaskDense(prhs[3]);
#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "SBA: %s point visibility mask\n", mxIsSparse(prhs[3])? "sparse" : "dense");
#endif /* DEBUG */

  /** p **/
  /* the fifth argument must be a vector */
  if(!mxIsDouble(prhs[4]) || mxIsComplex(prhs[4]) || !(mxGetM(prhs[4])==1 || mxGetN(prhs[4])==1))
    mexErrMsgTxt("sba: p must be a real vector.");
  p0=mxGetPr(prhs[4]);
  /* determine if we have a row or column vector and retrieve its 
   * size, i.e. the number of parameters
   */
  if(mxGetM(prhs[4])==1){
    nvars=mxGetN(prhs[4]);
    mdata.isrow_p0=1;
  }
  else{
    nvars=mxGetM(prhs[4]);
    mdata.isrow_p0=0;
  }
  /* copy input parameter vector to avoid destroying it */
  p=mxMalloc(nvars*sizeof(double));
  /*
  for(i=0; i<nvars; ++i)
    p[i]=p0[i];
  */
  dblcopy_(p, p0, nvars);

  /** cnp **/
  /* the sixth argument must be a scalar */
  if(!mxIsDouble(prhs[5]) || mxIsComplex(prhs[5]) || mxGetM(prhs[5])!=1 || mxGetN(prhs[5])!=1)
    mexErrMsgTxt("sba: cnp must be a scalar.");
  cnp=(int)mxGetScalar(prhs[5]);

  /** pnp **/
  /* the seventh argument must be a scalar */
  if(!mxIsDouble(prhs[6]) || mxIsComplex(prhs[6]) || mxGetM(prhs[6])!=1 || mxGetN(prhs[6])!=1)
    mexErrMsgTxt("sba: pnp must be a scalar.");
  pnp=(int)mxGetScalar(prhs[6]);

  /* check that p has the right dimension */
  if(nvars!=m*cnp + n*pnp)
    matlabFmtdErrMsgTxt("sba: p must have %d elements (got %d).", m*cnp + n*pnp, nvars);

  /** x **/
  /* the eighth argument must be a vector */
  if(!mxIsDouble(prhs[7]) || mxIsComplex(prhs[7]) || !(mxGetM(prhs[7])==1 || mxGetN(prhs[7])==1))
    mexErrMsgTxt("sba: x must be a real vector.");
  x=mxGetPr(prhs[7]);
  nprojs=_MAX_(mxGetM(prhs[7]), mxGetN(prhs[7]));

  /* covx (optional) */
  /* check if the ninth argument is a vector */
  if(mxIsDouble(prhs[8]) && !mxIsComplex(prhs[8]) && (mxGetM(prhs[8])==1 || mxGetN(prhs[8])==1)){
    covlen=_MAX_(mxGetM(prhs[8]), mxGetN(prhs[8]));
    if(covlen>1){ /* make sure that argument is not a scalar */
      covx=mxGetPr(prhs[8]);

      ++prhs;
      --nrhs;
    }
  }

  /** mnp **/
  /* the ninth required argument must be a scalar */
  if(!mxIsDouble(prhs[8]) || mxIsComplex(prhs[8]) || mxGetM(prhs[8])!=1 || mxGetN(prhs[8])!=1)
    mexErrMsgTxt("sba: mnp must be a scalar.");
  mnp=(int)mxGetScalar(prhs[8]);
  nprojs/=mnp;

  /* check that x has the correct dimension, comparing with the elements in vmask */
  for(i=len=0; i<m*n; ++i)
    if(vmask[i]) ++len;
  if(nprojs!=len)
    matlabFmtdErrMsgTxt("sba: the size of x should agree with the number of non-zeros in vmask (got %d and %d).", nprojs, len);

  /* if supplied, check that covx has the correct dimension comparing with the elements in vmask */
  if(covx && covlen!=len*mnp*mnp)
    matlabFmtdErrMsgTxt("sba: covx must be a real vector of size %d (got %d).", len*mnp*mnp, covlen);

  /** proj **/ 
  /* the tenth required argument must be a string , i.e. a char row vector */
  if(mxIsChar(prhs[9])!=1)
    mexErrMsgTxt("sba: proj argument must be a string.");
  if(mxGetM(prhs[9])!=1)
    mexErrMsgTxt("sba: proj argument must be a string (i.e. char row vector).");
  /* retrieve supplied name */
  len=mxGetN(prhs[9])+1;
  status=mxGetString(prhs[9], mdata.projname, _MIN_(len, MAXNAMELEN));
  if(status!=0)
    mexErrMsgTxt("sba: not enough space. String is truncated.");
  /* check if we have a name@library pair */
  if((str=strchr(mdata.projname, '@'))){
    *str++='\0';
    /* copy the library name */
    strcpy(mdata.projlibname, str);
    /* attempt to load the library */
#ifdef WIN32
    mdata.projlibhandle=LoadLibrary(mdata.projlibname);
    if(!mdata.projlibhandle)
#else
    mdata.projlibhandle=dlopen(mdata.projlibname, RTLD_LAZY);
    if(!mdata.projlibhandle)
#endif /* WIN32 */
      matlabFmtdErrMsgTxt("sba: error loading dynamic library %s!\n", mdata.projlibname);
    havedynproj=1;
  }
  else{
    mdata.projlibhandle=NULL;
    havedynproj=0;
  }

  /** jac (optional) **/
  havejac=havedynprojac=0;
  /* check whether eleventh argument is a nonempty string */
  if(mxIsChar(prhs[10])==1){
    switch(mxGetM(prhs[10])){
      case 1:
        /* store supplied name */
        len=mxGetN(prhs[10])+1;
        status=mxGetString(prhs[10], mdata.projacname, _MIN_(len, MAXNAMELEN));
        if(status!=0)
          mexErrMsgTxt("sba: not enough space. String is truncated.");
        havejac=1;

        /* check if we have a name@library pair */
        if((str=strchr(mdata.projacname, '@'))){
          *str++='\0';
          /* copy the library name */
          strcpy(mdata.projaclibname, str);
          if(!havedynproj || strcmp(mdata.projlibname, mdata.projaclibname)){ /* is this a different library from that for the proj. function? */
            /* yes, attempt to load it */
#         ifdef WIN32
            mdata.projaclibhandle=LoadLibrary(mdata.projaclibname);
            if(!mdata.projaclibhandle)
#         else
            mdata.projaclibhandle=dlopen(mdata.projaclibname, RTLD_LAZY);
            if(!mdata.projaclibhandle)
#         endif /* WIN32 */
              matlabFmtdErrMsgTxt("sba: error loading dynamic library %s!\n", mdata.projaclibname);
          }
          else /* proj. function and Jacobian come from the same library */ 
            mdata.projaclibhandle=mdata.projlibhandle;
          havedynprojac=1;
        }
        else{
          mdata.projaclibhandle=NULL;
          havedynprojac=0;
        }

        /* falling through! */
      case 0: /* empty string, ignore */
        ++prhs;
        --nrhs;
        break;
      default:
        matlabFmtdErrMsgTxt("sba: projac argument must be a string (i.e. row vector); got %dx%d.",
                                  mxGetM(prhs[10]), mxGetN(prhs[10]));
    }
  }

#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "SBA: %s analytic Jacobian\n", havejac? "with" : "no");
#endif /* DEBUG */

  /** itmax **/
  /* the eleventh required argument must be a scalar */
  if(!mxIsDouble(prhs[10]) || mxIsComplex(prhs[10]) || mxGetM(prhs[10])!=1 || mxGetN(prhs[10])!=1)
    mexErrMsgTxt("sba: itmax must be a scalar.");
  itmax=(int)mxGetScalar(prhs[10]);

  /* all arguments below this point are optional */

  /* check if we have a scalar argument; if yes, this is taken to be the 'verbose' argument */
  if(nrhs>=MININARGS){
    if(mxIsDouble(prhs[min1]) && !mxIsComplex(prhs[min1]) && mxGetM(prhs[min1])==1 && mxGetN(prhs[min1])==1){
      verbose=(int)mxGetScalar(prhs[min1]);

      ++prhs;
      --nrhs;
    }
  }

  /* check if we have a vector argument; if yes, this is taken to be the 'opts' argument */
  if(nrhs>=MININARGS && mxIsDouble(prhs[min1]) && !mxIsComplex(prhs[min1]) && ((mxGetM(prhs[min1])==1 || mxGetN(prhs[min1])==1)
                                                                     || (mxGetM(prhs[min1])==0 && mxGetN(prhs[min1])==0))){
    pdbl=mxGetPr(prhs[min1]);
    nopts=_MAX_(mxGetM(prhs[min1]), mxGetN(prhs[min1]));
    if(nopts!=0){ /* if opts==[], nothing needs to be done and the defaults are used */
      if(nopts>SBA_OPTSSZ)
        matlabFmtdErrMsgTxt("sba: opts must have at most %d elements, got %d.", SBA_OPTSSZ, nopts);
      else if(nopts<SBA_OPTSSZ)
        matlabFmtdWarnMsgTxt("sba: only the %d first elements of opts specified, remaining set to defaults.", nopts);
      for(i=0; i<nopts; ++i)
        opts[i]=pdbl[i];
    }
#ifdef DEBUG
    else{
      fflush(stderr);
      fprintf(stderr, "SBA: empty options vector, using defaults\n");
    }
#endif /* DEBUG */

    ++prhs;
    --nrhs;
  }

  /* check if we have a string argument; if yes, this is taken to be the 'reftype' argument */
  if(nrhs>=MININARGS && mxIsChar(prhs[min1])==1 && mxGetM(prhs[min1])==1){
    char *refhowto;

    /* examine supplied name */
    len=mxGetN(prhs[min1])+1;
    refhowto=mxCalloc(len, sizeof(char));
    status=mxGetString(prhs[min1], refhowto, len);
    if(status!=0)
      mexErrMsgTxt("sba: not enough space. String is truncated.");

    for(i=0; refhowto[i]; ++i)
      refhowto[i]=tolower(refhowto[i]);

    if(!strcmp(refhowto, "motstr")) reftype=BA_MOTSTRUCT;
    else if(!strcmp(refhowto, "mot")) reftype=BA_MOT;
    else if(!strcmp(refhowto, "str")) reftype=BA_STRUCT;
    else matlabFmtdErrMsgTxt("sba: unknown minimization type '%s'.", refhowto);

    mxFree(refhowto);

    ++prhs;
    --nrhs;
  }
  else
    reftype=BA_MOTSTRUCT;


  /* arguments below this point are assumed to be extra arguments passed
   * to every invocation of the projection function and its Jacobian
   */

  nextra=nrhs-MININARGS+1;
#ifdef DEBUG
  fflush(stderr);
  fprintf(stderr, "SBA: %d extra args\n", nextra);
#endif /* DEBUG */
  /* handle any extra args and allocate memory for
   * passing them to matlab/C
   */
  if(!havedynproj || !havedynprojac){ /* at least one of the projection and Jacobian functions are in matlab */
    nreserved=2;
    mdata.nrhs=nextra+nreserved;
    mdata.rhs=(mxArray **)mxMalloc(mdata.nrhs*sizeof(mxArray *));
    for(i=0; i<nextra; ++i)
      mdata.rhs[i+nreserved]=(mxArray *)prhs[nrhs-nextra+i]; /* discard 'const' modifier */

    mdata.rhs[0]=mxCreateDoubleMatrix(1, cnp, mxREAL); /* camera parameters */
    mdata.rhs[1]=mxCreateDoubleMatrix(1, pnp, mxREAL); /* point  parameters */

    mdata.dynadata=NULL;
  }
  else
    mdata.rhs=NULL;

  mdata.dynadata=NULL;
  if(havedynproj || havedynprojac){ /* at least one of the projection and Jacobian functions are from a dynlib */
    if(nextra>0){
      mdata.dynadata=(double **)mxMalloc(nextra*sizeof(double *));
      for(i=0; i<nextra; ++i)
        mdata.dynadata[i]=mxGetPr(prhs[nrhs-nextra+i]);
    }
  }
#ifdef DEBUG
  fprintf(stderr, "Projection function: %s, Jacobian: %s, %s\n", havedynproj? "dynamic" : "matlab",
                                                                 havejac? "present" : "not present", havedynprojac? "dynamic" : "matlab");
#endif

  mdata.cnp=cnp; mdata.pnp=pnp;
  mdata.mnp=mnp;

  /* ensure that the supplied matlab function & Jacobian are as expected */
  if((!havedynproj || !havedynprojac) && /* at least one in matlab */
      checkFuncAndJacobianMATLAB(p, p+m*cnp, !havedynproj, havejac && !havedynprojac, reftype, &mdata)){ /* check using first camera & first point */
    status=SBA_ERROR;
    goto cleanup;
  }

  /* invoke sba */
  start_time=clock();
  switch(reftype){
    case BA_MOTSTRUCT:
      minnvars=nvars;
      mdata.pa=mdata.pb=NULL; /* not needed */
      status=sba_motstr_levmar(n, m, mcon, vmask, p, cnp, pnp, x, covx, mnp,
                              (havedynproj)? proj_motstrDL : proj_motstrMATLAB, (havejac)? (havedynprojac? projac_motstrDL : projac_motstrMATLAB) : NULL,
                              (void *)&mdata, itmax, verbose>1, opts, info);
      break;
    case BA_MOT:
      minnvars=m*cnp;
      mdata.pa=NULL; /* not needed */
      mdata.pb=p+m*cnp;
      /* note: only the first part of p is used in the call below (i.e. first m*cnp elements) */
      status=sba_mot_levmar(n, m, mcon, vmask, p, cnp, x, covx, mnp,
                            (havedynproj)? proj_motDL : proj_motMATLAB, (havejac)? (havedynprojac? projac_motDL : projac_motMATLAB) : NULL,
                            (void *)&mdata, itmax, verbose>1, opts, info);
      break;
    case BA_STRUCT:
      minnvars=n*pnp;
      mdata.pa=p;
      mdata.pb=NULL; /* not needed */
      status=sba_str_levmar(n, m, vmask, p+m*cnp, pnp, x, covx, mnp,
                            (havedynproj)? proj_strDL : proj_strMATLAB, (havejac)? (havedynprojac? projac_strDL : projac_strMATLAB) : NULL,
                            (void *)&mdata, itmax, verbose>1, opts, info);
      break;
    default: /* should not reach this point */
      matlabFmtdErrMsgTxt("sba: unknown refinement type %d requested.", reftype);
  }
  end_time=clock();

  if(verbose){
    fflush(stdout);
    mexPrintf("\nSBA using %d 3D pts, %d frames and %d image projections, %d variables\n",
              n, m, nprojs, minnvars);
    mexPrintf("\nRefining %s, %s Jacobian\n\n", reftypename[reftype], havejac? "analytic" : "approximate");
    mexPrintf("SBA returned %d in %g iter, reason %g, error %g [initial %g], %d/%d func/fjac evals, %d lin. systems\n",
              status, info[5], info[6], info[1]/nprojs, info[0]/nprojs, (int)info[7], (int)info[8], (int)info[9]);
    mexPrintf("Elapsed time: %.2lf seconds, %.2lf msecs\n", ((double) (end_time - start_time)) / CLOCKS_PER_SEC,
              ((double) (end_time - start_time)) / CLOCKS_PER_MSEC);
    fflush(stdout);
  }

  /* copy back return results */
  /** ret **/
  plhs[0]=mxCreateDoubleMatrix(1, 1, mxREAL);
  pdbl=mxGetPr(plhs[0]);
  *pdbl=(double)status;

  /** p **/
  plhs[1]=(mdata.isrow_p0==1)? mxCreateDoubleMatrix(1, nvars, mxREAL) : mxCreateDoubleMatrix(nvars, 1, mxREAL);
  pdbl=mxGetPr(plhs[1]);
  /*
  for(i=0; i<nvars; ++i)
    pdbl[i]=p[i];
  */
  dblcopy_(pdbl, p, nvars);

  /** info **/
  if(nlhs>MINOUTARGS){
    plhs[2]=mxCreateDoubleMatrix(1, SBA_INFOSZ, mxREAL);
    pdbl=mxGetPr(plhs[2]);
    /*
    for(i=0; i<SBA_INFOSZ; ++i)
      pdbl[i]=info[i];
    */
    dblcopy_(pdbl, info, SBA_INFOSZ);
  }

cleanup:
  /* cleanup */
  mxFree(vmask);
  mxFree(p);

  if(mdata.rhs){
    for(i=0; i<nreserved; ++i)
      mxDestroyArray(mdata.rhs[i]);
    mxFree(mdata.rhs);
  }

  if(mdata.dynadata) mxFree(mdata.dynadata);

  /* unload libraries */
  if(havedynproj){
#ifdef WIN32
    i=FreeLibrary(mdata.projlibhandle);
    if(i==0)
#else
    i=dlclose(mdata.projlibhandle);
    if(i!=0)
#endif
      matlabFmtdErrMsgTxt("sba: error unloading dynamic library %s!\n", mdata.projlibname);
  }
  if(havedynprojac){
    if(mdata.projaclibhandle!=mdata.projlibhandle){
#ifdef WIN32
      i=FreeLibrary(mdata.projaclibhandle);
      if(i==0)
#else
      i=dlclose(mdata.projaclibhandle);
      if(i!=0)
#endif
        matlabFmtdErrMsgTxt("sba: error unloading dynamic library %s!\n", mdata.projaclibname);
    }
  }
}
