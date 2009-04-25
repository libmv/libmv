#ifndef _READPARAMS_H_
#define _READPARAMS_H_

void readInitialSBAEstimate(char *camsfname, char *ptsfname, int cnp, int pnp, int mnp, 
                            void (*infilter)(double *pin, int nin, double *pout, int nout), int cnfp,
                            int *ncams, int *n3Dpts, int *n2Dprojs,
                            double **motstruct, double **imgpts, double **covimgpts, char **vmask);

extern void readCalibParams(char *fname, double ical[9]);
extern void printSBAMotionData(double *motstruct, int ncams, int cnp,
                               void (*outfilter)(double *pin, int nin, double *pout, int nout), int cnop);
extern void printSBAStructureData(double *motstruct, int ncams, int n3Dpts, int cnp, int pnp);
extern void printSBAData(double *motstruct, int cnp, int pnp, int mnp, 
                         void (*outfilter)(double *pin, int nin, double *pout, int nout), int cnop,
                         int ncams, int n3Dpts, double *imgpts, int n2Dprojs, char *vmask);

#endif /* _READPARAMS_H_ */
