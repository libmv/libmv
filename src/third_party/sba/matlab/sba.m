function [ret, popt, info]=sba(n, m, mcon, vmask, p0, cnp, pnp, x, mnp, proj, projac, itmax, verbose, opts, reftype, varargin)
% SBA  matlab MEX interface to the sba generic sparse bundle adjustment 
% library available from http://www.ics.forth.gr/~lourakis/sba/
% 
% Additional, problem specific data in the form of real vectors or matrices can be added as the last arguments of
% sba's invocation (i.e. varargin). These data are passed uniterpreted to all invocations of proj and projac,
% see below for details.
%
% In the following, the word "vector" is meant to imply either a row or a column vector.
%
% required input arguments:
% - n: number of 3D points
%
% - m: number of images
%
% - mcon: number of images whose parameters should not be modified
%
% - vmask: nxm matrix specifying the points visible in each camera. It can be either a dense or a sparse matrix;
%      in both cases, a nonzero element at position (i, j) indicates that point i is visible in image j.
%
% - p0: vector of doubles holding the initial parameter estimates laid out as (a1, ..., am, b1, ..., bn),
%      aj, bi being the j-th image and i-th point parameters, respectively
%
% - cnp: number of parameters for each camera
%
% - pnp: number of parameters for each 3D point
%
% - x: vector of doubles holding the measurements (i.e., image projections) laid out as
%      (x_11, .. x_1m, ..., x_n1, .. x_nm), where x_ij is the projection of the i-th point on the j-th image.
%      If point i is not visible in image j, x_ij is missing; see vmask(i, j).
%
% - covx: optional vector of doubles holding the covariance matrices of the measurements laid out row-by-row as
%      (sigma_x_11, .. sigma_x_1m, ..., sigma_x_n1, .. sigma_x_nm), where sigma_x_ij is the covariance matrix of x_ij.
%      If point i is not visible in image j, sigma_x_ij is missing; see vmask(i, j).
%      Note that since sigma_x_ij are symmetric, the row-major ordering of their elements is identical to Matlab's
%      native column-major ordering!
%
% - mnp: number of parameters for each image projections
%
% - proj: String defining the name of a function  computing the projection on camera j with parameters aj of
%         point i with parameters bi. If it does not contain the '@' character, it is assumed to refer to a
%         matlab function accepting at least two input args, i.e. xij=proj(aj, bi, varargin);
%         Any additional arguments passed to sba, are passed unaltered to each invocation of proj.
%         If proj is of the form XX@YY, it is assumed to refer to a function named XX loaded from the shared
%         library YY (i.e., a .so file in Un*x, a .dll in Windows). XX should be defined as
%         void (*XX)(double *aj, double *bi, double *xij, double **adata);
%         where adata is a list of pointers to any additional arguments passed to sba (i.e. adata[0], adata[1], ...)
%         Remember that using a projection function from a dynamic library is much more efficient than coding it in
%         matlab. See also the comments for projac below.
%
% - itmax: maximum number of iterations.
%
%
% additional optional input arguments:
% - projac: String defining the name of a function computing the Jacobian of proj above.
%      If it does not contain the '@' character, it is assumed to be a matlab function, as follows:
%          if reftype equals 'motstr', then [Aij, Bij]=projac(aj, bi, varargin);
%          if reftype equals 'mot', then Aij=projac(aj, bi, varargin);
%          if reftype equals 'str', then Bij=projac(aj, bi, varargin);
%          Aij denotes the Jacobian of xij with respect to aj, Bij the Jacobian of xij with respect to bi; see
%          below for an explanation of argument reftype. Aij and Bij are assumed to be double vectors containing
%          the respective Jacobians in row-major order.
%          Again, any additional arguments passed to sba, are passed unaltered to each invocation of projac.
%      If the string  is of the form XX@YY, then it is assumed to refer to a function named XX loaded from the
%      shared library YY. XX should be defined as follows:
%          if reftype equals 'motstr', then void (*XX)(double *aj, double *bi, double *Aij, double *Bij, double **adata);
%          if reftype equals 'mot', then void (*XX)(double *aj, double *bi, double *Aij, double **adata);
%          if reftype equals 'str', then void (*XX)(double *aj, double *bi, double *Bij, double **adata); 
%          adata is a list of pointers to any additional arguments that can be accessed as adata[0], adata[1], ...
%      As is the case for proj above, using a Jacobian function from a dynamic library is much more efficient than coding
%      it in matlab.
%      If is omitted or is equal to the empty string (i.e. ''), then the Jacobian is approximated with finite
%      differences through repeated invocations of proj.
%
% - verbose: verbosity level
%
% - opts: vector of doubles specifying the bundle adjustment parameters.
%      If an empty vector (i.e. []) is specified, defaults are used.
%      If N<SBA_OPTSSZ parameters are specified, the remaining SBA_OPTSSZ-N ones are set to defaults
%
% - reftype: String defining the type of refinement to be carried out. It should be one of the following:
%      'motstr' refinement of motion & structure, default
%      'mot'    refinement of motion only
%      'str'    refinement of structure only (mcon is redundant in this case)
%      If omitted, a default of 'motstr' is assumed. Depending on the minimization type, the MEX
%      interface will invoke one of sba_motstr_levmar(), sba_mot_levmar() or sba_str_levmar()
%
% - varargin: Denotes a list of additional, problem specific variables (real vectors & arrays), 
%      which are passed unaltered to each invocation of proj and projac.
%
%
% output arguments
% - ret: return value of sba, corresponding to the number of iterations if successful, -1 otherwise.
%
% - popt: estimated minimizer, i.e. minimized parameters vector.
%
% - info: optional array of doubles, which upon return provides information regarding the minimization.
%      See sba_levmar.c for more details.
%
 
error('sba.m is used only for providing documentation to sba; make sure that sba.c has been compiled using mex');
