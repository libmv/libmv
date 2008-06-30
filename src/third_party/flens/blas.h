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

#ifndef FLENS_BLAS_H
#define FLENS_BLAS_H 1

#include <complex>

#include <flens/storage.h>
#include <flens/blas_generic.h>

#ifdef VECLIB
#    include <Accelerate/Accelerate.h>
#elif defined MKL
#    ifdef MAC
#        include <Intel_MKL/mkl_cblas.h>
#    else
#        include <mkl_cblas.h>
#    endif
#else
    extern "C" {
#        include <cblas.h>
    }
#endif

namespace flens {

//- Level 1 --------------------------------------------------------------------

//- copy
void
copy(int N, const float *x, int incX, float *y, int incY);

void
copy(int N, const double *x, int incX, double *y, int incY);

void
copy(int N, const std::complex<double> *x, int incX,
     std::complex<double> *y, int incY);

//- scal
void
scal(const int N, float alpha, float *x, int incX);

void
scal(const int N, double alpha, double *x, int incX);

void
scal(const int N, std::complex<double> alpha,
     std::complex<double> *x, int incX);

//- axpy
void
axpy(int N, float alpha, const float *x, int incX, float *y, int incY);

void
axpy(int N, double alpha, const double *x, int incX, double *y, int incY);

void
axpy(int N, std::complex<double> alpha, const std::complex<double> *x,
     int incX, std::complex<double> *y, int incY);

//- dot
double
dot(int N, const float *x, int incX, const float *y, int incY);

double
dot(int N, const double *x, int incX, const double *y, int incY);

//- nrm2
double
nrm2(int N, const float *x, int incX);

double
nrm2(int N, const double *x, int incX);

double
nrm2(int N, const std::complex<double>* X, int incX);

//- amax
int
amax(int N, const float *x, int incX);

int
amax(int N, const double *x, int incX);

int
amax(int N, const std::complex<double> *x, int incX);

//- Level 1 Extentsions --------------------------------------------------------
//- amin
int
amin(int N, const float *x, int incX);

int
amin(int N, const double *x, int incX);

int
amin(int N, const std::complex<double> *x, int incX);

int
asum(int N, const int *x, int incX);

double
asum(int N, const double *x, int incX);

// B = A
template <typename T>
void
copy(StorageOrder order, int M, int N, const T *A, int lda, T *B, int ldb);


//- Level 2 --------------------------------------------------------------------

//- gemv
void
gemv(StorageOrder order, Transpose trans,
     int m, int n, float alpha, const float *A, int lda,
     const float *x, int incX,
     float beta, float *y, int incY);

void
gemv(StorageOrder order, Transpose trans,
     int m, int n, double alpha, const double *A, int lda,
     const double *x, int incX,
     double beta, double *y, int incY);

void
gemv(StorageOrder Order, Transpose Trans,
     int m, int n, std::complex<double> alpha,
     const std::complex<double> *A, int lda,
     const std::complex<double> *X, int incX,
     std::complex<double> beta, std::complex<double> *Y, int incY);

//- gbmv
void
gbmv(StorageOrder order, Transpose trans,
     int m, int n, int kl, int ku, float alpha, const float *A, int lda,
     const float *x, int incX,
     float beta, float *y, int incY);

void
gbmv(StorageOrder order, Transpose trans,
     int m, int n, int kl, int ku, double alpha, const double *A, int lda,
     const double *x, int incX,
     double beta, double *y, int incY);

//- trmv
void
trmv(StorageOrder order, StorageUpLo upLo, Transpose trans, UnitDiag diag,
     int n, const float *A, int lda, float *x, int incX);

void
trmv(StorageOrder order, StorageUpLo upLo, Transpose trans, UnitDiag diag,
     int n, const double *A, int lda, double *x, int incX);

//- tbmv
void
tbmv(StorageOrder order, StorageUpLo upLo, Transpose trans, UnitDiag diag,
     int n, int k, const float *A, const int lda, float *x, int incX);

void
tbmv(StorageOrder order, StorageUpLo upLo, Transpose trans, UnitDiag diag,
     int n, int k, const double *A, const int lda, double *x, int incX);

//- tpmv
void
tpmv(StorageOrder order, StorageUpLo upLo, Transpose trans, UnitDiag diag,
     int n, const float *A, float *x, int incX);

void
tpmv(StorageOrder order, StorageUpLo upLo, Transpose trans, UnitDiag diag,
     int n, const double *A, double *x, int incX);

//- symv
void
symv(StorageOrder order, StorageUpLo upLo,
     int n, float alpha, const float *A, int lda,
     const float *x, int incX,
     float beta, float *y, int incY);

void
symv(StorageOrder order, StorageUpLo upLo,
     int n, double alpha, const double *A, int lda,
     const double *x, int incX,
     double beta, double *y, int incY);

//- sbmv
void
sbmv(StorageOrder order, StorageUpLo upLo,
     int n, int k, float alpha, const float *A, int lda,
     const float *x, int incX,
     float beta, float *y, int incY);

void
sbmv(StorageOrder order, StorageUpLo upLo,
     int n, int k, double alpha, const double *A, int lda,
     const double *x, int incX,
     double beta, double *y, int incY);

//- spmv
void
spmv(StorageOrder order, StorageUpLo upLo,
     int n, float alpha, const float *A,
     const float *x, const int incX,
     float beta, float *y, const int incY);

void
spmv(StorageOrder order, StorageUpLo upLo,
     int n, double alpha, const double *A,
     const double *x, const int incX,
     double beta, double *y, const int incY);

//- Level 3 --------------------------------------------------------------------

//- gemm
void
gemm(StorageOrder order,
     Transpose transA, Transpose transB,
     int m, int n, int k, float alpha,
     const float *A, int lda,
     const float *B, int ldb,
     float beta, float *C, const int ldc);

void
gemm(StorageOrder order,
     Transpose transA, Transpose transB,
     int m, int n, int k, double alpha,
     const double *A, int lda,
     const double *B, int ldb,
     double beta, double *C, const int ldc);

void
gemm(StorageOrder order,
     Transpose transA, Transpose transB,
     int m, int n, int k, const std::complex<double>& alpha,
     const std::complex<double> *A, int lda,
     const std::complex<double> *B, int ldb,
     const std::complex<double>& beta, std::complex<double> *C, const int ldc);

enum BlasSide {
    Left,
    Right
};

//- symm
void
symm(StorageOrder order, BlasSide side, StorageUpLo upLo,
     int m, int n, float alpha,
     const float *A, const int lda,
     const float *B, const int ldb,
     const float beta, float *C, const int ldc);

void
symm(StorageOrder order, BlasSide side, StorageUpLo upLo,
     int m, int n, double alpha,
     const double *A, const int lda,
     const double *B, const int ldb,
     const double beta, double *C, const int ldc);

//- trmm
void
trmm(StorageOrder order, BlasSide side, StorageUpLo upLo,
     Transpose transA,
     UnitDiag diag, int m, int n,
     float alpha, const float *A, int lda,
     float *B, int ldb);

void
trmm(StorageOrder order, BlasSide side, StorageUpLo upLo,
     Transpose transA,
     UnitDiag diag, int m, int n,
     double alpha, const double *A, int lda,
     double *B, int ldb);

//- trsm
void
trsm(StorageOrder order, BlasSide side, StorageUpLo upLo,
     Transpose transA,
     UnitDiag diag, int m, int n,
     float alpha, const float *A, int lda,
     float *B, int ldb);

void
trsm(StorageOrder order, BlasSide side, StorageUpLo upLo,
     Transpose transA,
     UnitDiag diag, int m, int n,
     double alpha, const double *A, int lda,
     double *B, int ldb);

} // namespace flens

#include <flens/blas.tcc>

#endif // FLENS_BLAS_H
