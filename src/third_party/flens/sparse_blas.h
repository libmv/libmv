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

#ifndef FLENS_SPARSE_BLAS_H
#define FLENS_SPARSE_BLAS_H 1

#include <flens/storage.h>

namespace flens {

//-- crs - compressed row storage ----------------------------------------------

template <typename T>
    void
    crs_gemv(Transpose trans, int m, int n, T alpha,
             const T *a, const int *ia, const int *ja,
             const T *x, T beta, T *y);

template <typename T>
    void
    crs_symv(StorageUpLo upLo, int m, T alpha,
             const T *a, const int *ia, const int *ja,
             const T *x, T beta, T *y);

//-- csr - compressed sparse row - (the Intel variant for crs) -----------------

void
csrmv(Transpose trans, int m, int k, float alpha, char *matdescra,
      float  *values, int *columns,  int *pointerB, int *pointerE,
      float *x, float beta, float *y);

void
csrmv(Transpose trans, int m, int k, double alpha, char *matdescra,
      double  *values, int *columns,  int *pointerB, int *pointerE,
      double *x, double beta, double *y);

void
csrmm(Transpose transA, int m, int n, int k, float alpha, char *matdescra,
      const float *values, const int *columns, const int *pointerB,
      const int *pointerE, const float *B, int ldb,
      float beta, float *C, int ldc);

void
csrmm(Transpose transA, int m, int n, int k, double alpha, char *matdescra,
      const double *values, const int *columns, const int *pointerB,
      const int *pointerE, const double *B, int ldb,
      double beta, double *C, int ldc);

} // namespace flens

#include <flens/sparse_blas.tcc>

#endif // FLENS_SPBLAS_H
