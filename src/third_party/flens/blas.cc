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

#include <cmath>
#include <cstdlib>
#include <flens/blas.h>

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
copy(int N, const float *x, int incX, float *y, int incY)
{
    cblas_scopy(N, x, incX, y, incY);
}

void
copy(int N, const double *x, int incX, double *y, int incY)
{
    cblas_dcopy(N, x, incX, y, incY);
}

void
copy(int N, const std::complex<double> *x, int incX, std::complex<double> *y, int incY)
{
    cblas_zcopy(N, x, incX, y, incY);
}

//- scal
void
scal(const int N, float alpha, float *X, int incX)
{
    cblas_sscal(N, alpha, X, incX);
}

void
scal(int N, double alpha, double *X, int incX)
{
    cblas_dscal(N, alpha, X, incX);
}

void
scal(const int N, std::complex<double> alpha,
     std::complex<double> *X, int incX)
{
    cblas_zscal(N, &alpha, X, incX);
}

//- axpy
void
axpy(int N, float alpha, const float *X, int incX, float *Y, int incY)
{
    cblas_saxpy(N, alpha, X, incX, Y, incY);
}

void
axpy(int N, double alpha, const double *X, int incX, double *Y, int incY)
{
    cblas_daxpy(N, alpha, X, incX, Y, incY);
}

void
axpy(int N, std::complex<double> alpha, const std::complex<double> *X,
     int incX, std::complex<double> *Y, int incY)
{
    cblas_zaxpy(N, &alpha, X, incX, Y, incY);
}

//- dot
double
dot(int N, const float *X, int incX, const float *Y, int incY)
{
    return cblas_sdot(N, X, incX, Y, incY);
}

double
dot(int N, const double *X, int incX, const double *Y, int incY)
{
    return cblas_ddot(N, X, incX, Y, incY);
}

//- nrm2
double
nrm2(int N, const float *X, int incX)
{
    return cblas_snrm2(N, X, incX);
}

double
nrm2(int N, const double *X, int incX)
{
    return cblas_dnrm2(N, X, incX);
}

double
nrm2(int N, const std::complex<double>* X, int incX) {
    return cblas_dznrm2(N, X, incX);
}

//- amax
int
amax(int N, const float *X, int incX)
{
    return cblas_isamax(N, X, incX);
}

int
amax(int N, const double *X, int incX)
{
    return cblas_idamax(N, X, incX);
}

int
amax(int N, const std::complex<double> *X, int incX)
{
    return cblas_izamax(N, X, incX);
}

//- amin
int
amin(int N, const float *X, int incX)
{
    int pos = 0;
    const float *max = X;
    for (int i=1; i<N; ++i) {
        if (std::abs(*(X+=incX))<std::abs(*max)) {
            max = X;
            pos = i;
        }
    }
    return pos;
}

int
amin(int N, const double *X, int incX)
{
    int pos = 0;
    const double *max = X;
    for (int i=1; i<N; ++i) {
        if (std::abs(*(X+=incX))<std::abs(*max)) {
            max = X;
            pos = i;
        }
    }
    return pos;
}

int
amin(int N, const std::complex<double> *X, int incX)
{
    int pos = 0;
    const std::complex<double> *max = X;
    for (int i=1; i<N; ++i) {
        if (std::abs(*(X+=incX))<std::abs(*max)) {
            max = X;
            pos = i;
        }
    }
    return pos;
}

//- asum
int
asum(int N, const int *x, int incX)
{
    int sum = 0;
    for (int i=0; i<N; i+=incX) {
        sum += std::abs(x[i]);
    }
    return sum;
}

double
asum(int N, const double *x, int incX)
{
    double sum = 0.;
    for (int i=0; i<N; i+=incX) {
        sum += std::fabs(x[i]);
    }
    return 0;
}


//- Level 2 --------------------------------------------------------------------

//- gemv
void
gemv(StorageOrder Order, Transpose Trans,
     int m, int n, float alpha, const float *A, int lda,
     const float *X, int incX,
     float beta, float *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_sgemv(order, trans, m, n, alpha, A, lda, X, incX, beta, Y, incY);
}

void
gemv(StorageOrder Order, Transpose Trans,
     int m, int n, double alpha, const double *A, int lda,
     const double *X, int incX,
     double beta, double *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_dgemv(order, trans, m, n, alpha, A, lda, X, incX, beta, Y, incY);
}

void
gemv(StorageOrder Order, Transpose Trans,
     int m, int n, std::complex<double> alpha,
     const std::complex<double> *A, int lda,
     const std::complex<double> *X, int incX,
     std::complex<double> beta, std::complex<double> *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_zgemv(order, trans, m, n, &alpha, A, lda, X, incX, &beta, Y, incY);
}

//- gbmv
void
gbmv(StorageOrder Order, Transpose Trans,
     int m, int n, int kl, int ku, float alpha, const float *A, int lda,
     const float *X, int incX,
     float beta, float *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_sgbmv(order, trans, m, n, kl, ku,
                alpha, A, lda, X, incX, beta, Y, incY);
}

void
gbmv(StorageOrder Order, Transpose Trans,
     int m, int n, int kl, int ku, double alpha, const double *A, int lda,
     const double *X, int incX,
     double beta, double *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_dgbmv(order, trans, m, n, kl, ku,
                alpha, A, lda, X, incX, beta, Y, incY);
}

//- trmv
void
trmv(StorageOrder Order, StorageUpLo UpLo, Transpose Trans, UnitDiag Diag,
     int n, const float *A, int lda, float *X, int incX)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_strmv(order, upLo, trans, diag, n, A, lda, X, incX);
}

void
trmv(StorageOrder Order, StorageUpLo UpLo, Transpose Trans, UnitDiag Diag,
     int n, const double *A, int lda, double *X, int incX)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_dtrmv(order, upLo, trans, diag, n, A, lda, X, incX);
}

//- tbmv
void
tbmv(StorageOrder Order, StorageUpLo UpLo, Transpose Trans, UnitDiag Diag,
     int n, int k, const float *A, const int lda, float *X, int incX)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_stbmv(order, upLo, trans, diag, n, k, A, lda, X, incX);
}

void
tbmv(StorageOrder Order, StorageUpLo UpLo, Transpose Trans, UnitDiag Diag,
     int n, int k, const double *A, const int lda, double *X, int incX)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_dtbmv(order, upLo, trans, diag, n, k, A, lda, X, incX);
}

//- tpmv
void
tpmv(StorageOrder Order, StorageUpLo UpLo, Transpose Trans, UnitDiag Diag,
     int n, const float *A, float *x, int incX)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_stpmv(order, upLo, trans, diag, n, A, x, incX);
}

void
tpmv(StorageOrder Order, StorageUpLo UpLo, Transpose Trans, UnitDiag Diag,
     int n, const double *A, double *x, int incX)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE trans = (Trans==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_dtpmv(order, upLo, trans, diag, n, A, x, incX);
}

//- symv
void
symv(StorageOrder Order, StorageUpLo UpLo,
     int n, float alpha, const float *A, int lda,
     const float *X, int incX,
     float beta, float *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_ssymv(order, upLo, n, alpha, A, lda, X, incX, beta, Y, incY);
}

void
symv(StorageOrder Order, StorageUpLo UpLo,
     int n, double alpha, const double *A, int lda,
     const double *X, int incX,
     double beta, double *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_dsymv(order, upLo, n, alpha, A, lda, X, incX, beta, Y, incY);
}

//- sbmv
void
sbmv(StorageOrder Order, StorageUpLo UpLo,
     int n, int k, float alpha, const float *A, int lda,
     const float *X, int incX,
     float beta, float *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_ssbmv(order, upLo, n, k, alpha, A, lda, X, incX, beta, Y, incY);
}

void
sbmv(StorageOrder Order, StorageUpLo UpLo,
     int n, int k, double alpha, const double *A, int lda,
     const double *X, int incX,
     double beta, double *Y, int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_dsbmv(order, upLo, n, k, alpha, A, lda, X, incX, beta, Y, incY);
}

//- spmv
void
spmv(StorageOrder Order, StorageUpLo UpLo,
     int n, float alpha, const float *A,
     const float *x, const int incX,
     float beta, float *y, const int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_sspmv(order, upLo, n, alpha, A, x, incX, beta, y, incY);
}

void
spmv(StorageOrder Order, StorageUpLo UpLo,
     int n, double alpha, const double *A,
     const double *x, const int incX,
     double beta, double *y, const int incY)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_dspmv(order, upLo, n, alpha, A, x, incX, beta, y, incY);
}

//- Level 3 --------------------------------------------------------------------

//- gemm
void
gemm(StorageOrder Order,
     Transpose TransA, Transpose TransB,
     int m, int n, int k, float alpha,
     const float *A, int lda,
     const float *B, int ldb,
     float beta, float *C, const int ldc)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_TRANSPOSE transB = (TransB==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_sgemm(order, transA, transB,
                m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
}

void
gemm(StorageOrder Order,
     Transpose TransA, Transpose TransB,
     int m, int n, int k, double alpha,
     const double *A, int lda,
     const double *B, int ldb,
     double beta, double *C, const int ldc)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_TRANSPOSE transB = (TransB==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_dgemm(order, transA, transB,
                m, n, k, alpha, A, lda, B, ldb, beta, C, ldc);
}

void
gemm(StorageOrder Order,
     Transpose TransA, Transpose TransB,
     int m, int n, int k, const std::complex<double>& alpha,
     const std::complex<double> *A, int lda,
     const std::complex<double> *B, int ldb,
     const std::complex<double>& beta, std::complex<double> *C, const int ldc)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_TRANSPOSE transB = (TransB==NoTrans) ? CblasNoTrans : CblasTrans;

    cblas_zgemm(order, transA, transB,
                m, n, k, &alpha, A, lda, B, ldb, &beta, C, ldc);

}


//- symm
void
symm(StorageOrder Order, BlasSide Side, StorageUpLo UpLo,
     int m, int n, float alpha,
     const float *A, const int lda,
     const float *B, const int ldb,
     const float beta, float *C, const int ldc)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_SIDE side = (Side==Left) ? CblasLeft : CblasRight;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_ssymm(order, side, upLo, m, n, alpha, A, lda, B, ldb, beta, C, ldc);
}

void
symm(StorageOrder Order, BlasSide Side, StorageUpLo UpLo,
     int m, int n, double alpha,
     const double *A, const int lda,
     const double *B, const int ldb,
     const double beta, double *C, const int ldc)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_SIDE side = (Side==Left) ? CblasLeft : CblasRight;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;

    cblas_dsymm(order, side, upLo, m, n, alpha, A, lda, B, ldb, beta, C, ldc);
}

//- trmm
void
trmm(StorageOrder Order, BlasSide Side, StorageUpLo UpLo,
     Transpose TransA,
     UnitDiag Diag, int m, int n,
     float alpha, const float *A, int lda,
     float *B, int ldb)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_SIDE side = (Side==Left) ? CblasLeft : CblasRight;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_strmm(order, side, upLo, transA, diag, m, n, alpha, A, lda, B, ldb);
}

void
trmm(StorageOrder Order, BlasSide Side, StorageUpLo UpLo,
     Transpose TransA,
     UnitDiag Diag, int m, int n,
     double alpha, const double *A, int lda,
     double *B, int ldb)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_SIDE side = (Side==Left) ? CblasLeft : CblasRight;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_dtrmm(order, side, upLo, transA, diag, m, n, alpha, A, lda, B, ldb);
}

//- trsm
void
trsm(StorageOrder Order, BlasSide Side, StorageUpLo UpLo,
     Transpose TransA,
     UnitDiag Diag, int m, int n,
     float alpha, const float *A, int lda,
     float *B, int ldb)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_SIDE side = (Side==Left) ? CblasLeft : CblasRight;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_strsm(order, side, upLo, transA, diag, m, n, alpha, A, lda, B, ldb);
}

void
trsm(StorageOrder Order, BlasSide Side, StorageUpLo UpLo,
     Transpose TransA,
     UnitDiag Diag, int m, int n,
     double alpha, const double *A, int lda,
     double *B, int ldb)
{
    CBLAS_ORDER order = (Order==RowMajor) ? CblasRowMajor : CblasColMajor;
    CBLAS_SIDE side = (Side==Left) ? CblasLeft : CblasRight;
    CBLAS_UPLO upLo = (UpLo==Upper) ? CblasUpper : CblasLower;
    CBLAS_TRANSPOSE transA = (TransA==NoTrans) ? CblasNoTrans : CblasTrans;
    CBLAS_DIAG diag = (Diag==NonUnit) ? CblasNonUnit : CblasUnit;

    cblas_dtrsm(order, side, upLo, transA, diag, m, n, alpha, A, lda, B, ldb);
}

} // namespace flens
