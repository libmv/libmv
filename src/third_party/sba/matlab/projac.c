#include <math.h>

/* Sample C code for the projection and Jacobian functions for Euclidean BA,
 * to be loaded from a shared (i.e., dynamic) library by sba's matlab MEX interface
 */

/* Compilation instructions:
 *
 * Un*x/GCC:    gcc -fPIC -O3 -shared -o projac.so projac.c
 * Win32/MSVC:  cl /nologo /O2 projac.c /link /dll /out:projac.dll
 */

#if defined(_MSC_VER) /* DLL directives for MSVC */
#define API_MOD    __declspec(dllexport)
#define CALL_CONV  __cdecl
#else /* define empty */
#define API_MOD 
#define CALL_CONV
#endif /* _MSC_VER */

API_MOD void CALL_CONV imgproj_motstr(double *rt, double *xyz, double *xij, double **adata)
{
double *cal=adata[0];
double a1, a2, a3, a4, a5;
double qr1, qr2, qr3, qr4, t1, t2, t3;
double X, Y, Z;

  a1=cal[0]; a2=cal[1]; a3=cal[2]; a4=cal[3]; a5=cal[4];
  qr2=rt[0]; qr3=rt[1]; qr4=rt[2];
  t1=rt[3]; t2=rt[4]; t3=rt[5];
  X=xyz[0]; Y=xyz[1]; Z=xyz[2];

  qr1=sqrt(1-(qr2*qr2+qr3*qr3+qr4*qr4));

  xij[0]=-((t3+Z+(-2*qr1*X+2*qr4*Y-2*qr3*Z)*qr3+(2*qr1*Y+2*qr4*X-2*qr2*Z)*qr2)*a3+(t2+Y+(2*qr1*X-2*qr4*Y)*qr4+2*qr3*qr4*Z+(2*qr3*X-2*qr1*Z-2*qr2*Y)*qr2)*a2+(X+t1+(-2*qr4*X-2*qr1*Y)*qr4+(-2*qr3*X+2*qr1*Z)*qr3+(2*qr3*Y+2*qr4*Z)*qr2)*a1)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2);
  
  xij[1]=((-Z-t3)*a5+(-2*a5*qr4*Y+2*a5*qr1*X+2*a5*Z*qr3)*qr3+(-2*a5*qr1*Y-2*a5*qr4*X+2*a5*Z*qr2)*qr2+(-t2-Y+(-2*qr1*X+2*qr4*Y)*qr4-2*qr3*qr4*Z+(-2*qr3*X+2*qr1*Z+2*qr2*Y)*qr2)*a4)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2);
}


API_MOD void CALL_CONV imgprojac_motstr(double *rt, double *xyz, double *jrt, double *jst, double **adata)
{
double *cal=adata[0];
double a1, a2, a3, a4, a5;
double qr1, qr2, qr3, qr4, t1, t2, t3;
double X, Y, Z;

  a1=cal[0]; a2=cal[1]; a3=cal[2]; a4=cal[3]; a5=cal[4];
  qr2=rt[0]; qr3=rt[1]; qr4=rt[2];
  t1=rt[3]; t2=rt[4]; t3=rt[5];
  X=xyz[0]; Y=xyz[1]; Z=xyz[2];

  qr1=sqrt(1-(qr2*qr2+qr3*qr3+qr4*qr4));

  jrt[0]=(-(2/qr1*X*qr2*qr3+(-2/qr1*Y*qr2-2*Z)*qr2+2*qr1*Y+2*qr4*X-2*qr2*Z)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+(t3+Z+(-2*qr1*X+2*qr4*Y-2*qr3*Z)*qr3+(2*qr1*Y+2*qr4*X-2*qr2*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*(-2/qr1*X*qr2*qr3+(2/qr1*Y*qr2+2*Z)*qr2-2*qr4*X-2*qr1*Y+2*qr2*Z))*a3+(-(-2/qr1*X*qr2*qr4+(2/qr1*Z*qr2-2*Y)*qr2+2*qr3*X-2*qr1*Z-2*qr2*Y)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+((2*qr3*X-2*qr1*Z-2*qr2*Y)*qr2+Y+(2*qr1*X-2*qr4*Y)*qr4+2*qr3*qr4*Z)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*(-2/qr1*X*qr2*qr3+(2/qr1*Y*qr2+2*Z)*qr2-2*qr4*X-2*qr1*Y+2*qr2*Z)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*(4*qr2*Z+(4*(qr2*qr2)-2+2*(qr3*qr3)+2*(qr4*qr4))/qr1*Y+(-2*qr3*qr2-2*qr4*qr1)/qr1*X)*t2)*a2+(-(2/qr1*Y*qr2*qr4-2/qr1*Z*qr2*qr3+2*qr3*Y+2*qr4*Z)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+(X+(-2*qr4*X-2*qr1*Y)*qr4+(-2*qr3*X+2*qr1*Z)*qr3+(2*qr3*Y+2*qr4*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*(-2/qr1*X*qr2*qr3+(2/qr1*Y*qr2+2*Z)*qr2-2*qr4*X-2*qr1*Y+2*qr2*Z)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*(4*qr2*Z+(4*(qr2*qr2)-2+2*(qr3*qr3)+2*(qr4*qr4))/qr1*Y+(-2*qr3*qr2-2*qr4*qr1)/qr1*X)*t1)*a1;

  jrt[1]=(-((2/qr1*X*qr3-2*Z)*qr3-2*qr1*X+2*qr4*Y-2*qr3*Z-2/qr1*Y*qr3*qr2)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+(t3+Z+(-2*qr1*X+2*qr4*Y-2*qr3*Z)*qr3+(2*qr1*Y+2*qr4*X-2*qr2*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr3+2*Z)*qr3+2*qr1*X-2*qr4*Y+2*qr3*Z+2/qr1*Y*qr3*qr2))*a3+(-(-2/qr1*X*qr3*qr4+2*qr4*Z+(2*X+2/qr1*Z*qr3)*qr2)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+((2*qr3*X-2*qr1*Z-2*qr2*Y)*qr2+Y+(2*qr1*X-2*qr4*Y)*qr4+2*qr3*qr4*Z)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr3+2*Z)*qr3+2*qr1*X-2*qr4*Y+2*qr3*Z+2/qr1*Y*qr3*qr2)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*(4*qr3*Z+(2*qr3*qr2-2*qr4*qr1)/qr1*Y+(-4*(qr3*qr3)+2-2*(qr2*qr2)-2*(qr4*qr4))/qr1*X)*t2)*a2+(-(2/qr1*Y*qr3*qr4+(-2*X-2/qr1*Z*qr3)*qr3-2*qr3*X+2*qr1*Z+2*qr2*Y)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+(X+(-2*qr4*X-2*qr1*Y)*qr4+(-2*qr3*X+2*qr1*Z)*qr3+(2*qr3*Y+2*qr4*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr3+2*Z)*qr3+2*qr1*X-2*qr4*Y+2*qr3*Z+2/qr1*Y*qr3*qr2)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*(4*qr3*Z+(2*qr3*qr2-2*qr4*qr1)/qr1*Y+(-4*(qr3*qr3)+2-2*(qr2*qr2)-2*(qr4*qr4))/qr1*X)*t1)*a1;

  jrt[2]=(-((2/qr1*X*qr4+2*Y)*qr3+(-2/qr1*Y*qr4+2*X)*qr2)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+(t3+Z+(-2*qr1*X+2*qr4*Y-2*qr3*Z)*qr3+(2*qr1*Y+2*qr4*X-2*qr2*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr4-2*Y)*qr3+(-2*X+2/qr1*Y*qr4)*qr2))*a3+(-((-2/qr1*X*qr4-2*Y)*qr4+2*qr1*X-2*qr4*Y+2*qr3*Z+2/qr1*Z*qr4*qr2)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+((2*qr3*X-2*qr1*Z-2*qr2*Y)*qr2+Y+(2*qr1*X-2*qr4*Y)*qr4+2*qr3*qr4*Z)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr4-2*Y)*qr3+(-2*X+2/qr1*Y*qr4)*qr2)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*((2*qr4*qr2-2*qr1*qr3)/qr1*Y+(-2*qr4*qr3-2*qr1*qr2)/qr1*X)*t2)*a2+(-((-2*X+2/qr1*Y*qr4)*qr4-2*qr4*X-2*qr1*Y-2/qr1*Z*qr4*qr3+2*qr2*Z)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)+(X+(-2*qr4*X-2*qr1*Y)*qr4+(-2*qr3*X+2*qr1*Z)*qr3+(2*qr3*Y+2*qr4*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr4-2*Y)*qr3+(-2*X+2/qr1*Y*qr4)*qr2)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*((2*qr4*qr2-2*qr1*qr3)/qr1*Y+(-2*qr4*qr3-2*qr1*qr2)/qr1*X)*t1)*a1;

  jrt[3]=-a1/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2);

  jrt[4]=-a2/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2);

  jrt[5]=(-1/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)-(t3+Z+(-2*qr1*X+2*qr4*Y-2*qr3*Z)*qr3+(2*qr1*Y+2*qr4*X-2*qr2*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)))*a3-(t2+Y+(2*qr1*X-2*qr4*Y)*qr4+2*qr3*qr4*Z+(2*qr3*X-2*qr1*Z-2*qr2*Y)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*a2-(X+t1+(-2*qr4*X-2*qr1*Y)*qr4+(-2*qr3*X+2*qr1*Z)*qr3+(2*qr3*Y+2*qr4*Z)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*a1;

  jrt[6]=((2/qr1*X*qr2*qr4+(-2/qr1*Z*qr2+2*Y)*qr2-2*qr3*X+2*qr1*Z+2*qr2*Y)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)-((-2*qr3*X+2*qr1*Z+2*qr2*Y)*qr2-Y+(-2*qr1*X+2*qr4*Y)*qr4-2*qr3*qr4*Z)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*(-2/qr1*X*qr2*qr3+(2/qr1*Y*qr2+2*Z)*qr2-2*qr4*X-2*qr1*Y+2*qr2*Z)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*(4*qr2*Z+(4*(qr2*qr2)-2+2*(qr3*qr3)+2*(qr4*qr4))/qr1*Y+(-2*qr3*qr2-2*qr4*qr1)/qr1*X)*t2)*a4;

  jrt[7]=((2/qr1*X*qr3*qr4-2*qr4*Z+(-2*X-2/qr1*Z*qr3)*qr2)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)-((-2*qr3*X+2*qr1*Z+2*qr2*Y)*qr2-Y+(-2*qr1*X+2*qr4*Y)*qr4-2*qr3*qr4*Z)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr3+2*Z)*qr3+2*qr1*X-2*qr4*Y+2*qr3*Z+2/qr1*Y*qr3*qr2)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*(4*qr3*Z+(2*qr3*qr2-2*qr4*qr1)/qr1*Y+(-4*(qr3*qr3)+2-2*(qr2*qr2)-2*(qr4*qr4))/qr1*X)*t2)*a4;

  jrt[8]=(((2/qr1*X*qr4+2*Y)*qr4-2*qr1*X+2*qr4*Y-2*qr3*Z-2/qr1*Z*qr4*qr2)/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)-((-2*qr3*X+2*qr1*Z+2*qr2*Y)*qr2-Y+(-2*qr1*X+2*qr4*Y)*qr4-2*qr3*qr4*Z)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*((-2/qr1*X*qr4-2*Y)*qr3+(-2*X+2/qr1*Y*qr4)*qr2)+1/(((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3)*((-1+2*(qr3*qr3)+2*(qr2*qr2))*Z+(-2*qr4*qr3-2*qr1*qr2)*Y+(2*qr1*qr3-2*qr4*qr2)*X-t3))*((2*qr4*qr2-2*qr1*qr3)/qr1*Y+(-2*qr4*qr3-2*qr1*qr2)/qr1*X)*t2)*a4;

  jrt[9]=0;

  jrt[10]=-a4/(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2);

  jrt[11]=(-t2-Y+(-2*qr1*X+2*qr4*Y)*qr4-2*qr3*qr4*Z+(-2*qr3*X+2*qr1*Z+2*qr2*Y)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2))*a4;

  jst[0]=-(((-2*qr1*Z-2*qr1*t3)*qr4+(-2*qr1*Y-2*t2*qr1)*qr3+((2*t2-2*Y)*qr4+(2*Z-2*t3)*qr3)*qr2)*a2+(-Z-t3+(2*t3+2*Z)*(qr4*qr4)+(2*qr4*Y-2*t1*qr1+2*qr3*t3)*qr3+(2*t1*qr4-2*qr1*Y+2*qr2*Z)*qr2)*a1)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2));

  jst[1]=((Z+t3+(-2*t3-2*Z)*(qr4*qr4)+(-2*qr1*X-2*t2*qr4-2*qr3*Z)*qr3+(-2*qr4*X-2*t2*qr1-2*t3*qr2)*qr2)*a2+((-2*qr1*Z-2*qr1*t3)*qr4+(-2*t1+2*X)*qr4*qr3+(-2*t1*qr1-2*qr1*X+(2*t3-2*Z)*qr3)*qr2)*a1)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2));

  jst[2]=((-t2-Y+(-2*qr1*X+2*qr4*Y)*qr4+(2*qr4*t3+(2*t2+2*Y)*qr3)*qr3+(-2*qr1*t3+2*qr3*X+2*t2*qr2)*qr2)*a2+(-X-t1+(2*qr1*Y+2*qr4*X)*qr4+(2*qr1*t3+2*t1*qr3)*qr3+(2*qr3*Y+2*qr4*t3+(2*t1+2*X)*qr2)*qr2)*a1)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2));

  jst[3]=2*a4*((qr1*Z+qr1*t3)*qr4+(qr1*Y+t2*qr1)*qr3+((Y-t2)*qr4+(t3-Z)*qr3)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2));

  jst[4]=-a4*(-Z-t3+(2*Z+2*t3)*(qr4*qr4)+(2*qr1*X+2*t2*qr4+2*qr3*Z)*qr3+(2*qr4*X+2*t2*qr1+2*t3*qr2)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2));

  jst[5]=a4*(-t2-Y+(-2*qr1*X+2*qr4*Y)*qr4+(2*qr4*t3+(2*t2+2*Y)*qr3)*qr3+(-2*qr1*t3+2*qr3*X+2*t2*qr2)*qr2)/((-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2)*(-Z-t3+(2*qr1*X-2*qr4*Y+2*qr3*Z)*qr3+(-2*qr4*X-2*qr1*Y+2*qr2*Z)*qr2));
}
