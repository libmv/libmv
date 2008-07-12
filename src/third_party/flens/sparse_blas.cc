/*
 *   Copyright (c) 2007, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cassert>
#include <flens/sparse_blas.h>
#include <iostream>

#ifdef MKL
#    ifdef MAC
#        include <Intel_MKL/mkl_spblas.h>
#    else
#        include <mkl_spblas.h>
#    endif
#endif

namespace flens {

//-- csr - compressed sparse row - (the Intel variant for crs) -----------------

#ifdef MKL_SCS
void
csrmv(Transpose Trans, int m, int k, float alpha, char *matdescra,
      float  *values, int *columns,  int *pointerB, int *pointerE,
      float *x, float beta, float *y)
{
        char trans = (Trans==NoTrans) ? 'N' : 'T';

        mkl_scsrmv (&trans, &m, &k, &alpha, matdescra,
                    values, columns, pointerB, pointerE,
                    x, &beta, y);
}

void
csrmv(Transpose Trans, int m, int k, double alpha, char *matdescra,
      double  *values, int *columns,  int *pointerB, int *pointerE,
      double *x, double beta, double *y)
{
        char trans = (Trans==NoTrans) ? 'N' : 'T';

        mkl_dcsrmv (&trans, &m, &k, &alpha, matdescra,
                    values, columns, pointerB, pointerE,
                    x, &beta, y);
}

void
csrmm(Transpose transA, int m, int n, int k, float alpha, char *matdescrA,
      const float *values, const int *columns, const int *pointerB,
      const int *pointerE, const float *B, int ldb,
      float beta, float *C, int ldc)
{
        char trans = (transA==NoTrans) ? 'N' : 'T';

        // TODO: what about constness?
        mkl_scsrmm(&trans, &m, &n, &k, &alpha, matdescrA,
                   const_cast<float *>(values),
                   const_cast<int *>(columns),
                   const_cast<int *>(pointerB),
                   const_cast<int *>(pointerE),
                   const_cast<float *>(B),
                   &ldb, &beta, C, &ldc);
}

void
csrmm(Transpose transA, int m, int n, int k, double alpha, char *matdescrA,
      const double *values, const int *columns, const int *pointerB,
      const int *pointerE, const double *B, int ldb,
      double beta, double *C, int ldc)
{
    char trans = (transA==NoTrans) ? 'N' : 'T';

    // TODO: what about constness?
    mkl_dcsrmm(&trans, &m, &n, &k, &alpha, matdescrA,
               const_cast<double *>(values),
               const_cast<int *>(columns),
               const_cast<int *>(pointerB),
               const_cast<int *>(pointerE),
               const_cast<double *>(B),
               &ldb, &beta, C, &ldc);
}
#endif // MKL

} // namespace flens
