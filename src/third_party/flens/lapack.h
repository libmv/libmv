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

#ifndef FLENS_LAPACK_H
#define FLENS_LAPACK_H 1

#include <flens/densevector.h>
#include <flens/generalmatrix.h>
#include <flens/lapack_generic.h>

namespace flens {

using std::complex;

int
getrf(int m, int n, float *a, int lda, int *ipiv);

int
getrf(int m, int n, double *a, int lda, int *ipiv);

int
getrf(int m, int n, std::complex<double> *a, int lda, int *ipiv);

int
gbtrf(int m, int n, int kl, int ku, float *ab, int ldab, int *ipiv);

int
gbtrf(int m, int n, int kl, int ku, double *ab, int ldab, int *ipiv);

int
getri(int n, float *a, int lda, const int *ipiv,
      float *work, int lwork);

int
getri(int n, double *a, int lda, const int *ipiv,
      double *work, int lwork);

int
getri(int n, std::complex<double> *a, int lda, const int *ipiv,
      std::complex<double> *work, int lwork);

int
getrs(Transpose trans, int n, int nrhs, const float *a, int lda,
      const int *ipiv, float *b, int ldb);

int
getrs(Transpose trans, int n, int nrhs, const double *a, int lda,
      const int *ipiv, double *b, int ldb);

int
gbtrs(Transpose trans, int n, int kl, int ku, int nrhs,
      const float *ab, int ldab, const int *ipiv, float *b, int ldb);

int
gbtrs(Transpose trans, int n, int kl, int ku, int nrhs,
      const double *ab, int ldab, const int *ipiv, double *b, int ldb);

int
gesv(int n, int nrhs, float *a, int lda, int *ipiv, float *b, int ldb);

int
gesv(int n, int nrhs, double *a, int lda, int *ipiv, double *b, int ldb);

int
gesv(int n, int nrhs, std::complex<double> *a, int lda, int *ipiv,
     std::complex<double> *b, int ldb);

int
gbsv(int n, int kl, int ku, int nrhs, float *ab, int ldab,
     int *ipiv, float *b, int ldb);

int
gbsv(int n, int kl, int ku, int nrhs, double *ab, int ldab,
     int *ipiv, double *b, int ldb);

int
gbsv(int n, int kl, int ku, int nrhs, std::complex<double> *ab, int ldab,
     int *ipiv, std::complex<double> *b, int ldb);

int
trtrs(StorageUpLo upLo, Transpose trans, UnitDiag diag, int n, int nrhs,
      const float *a, int lda, float *b, int ldb);

int
trtrs(StorageUpLo upLo, Transpose trans, UnitDiag diag, int n, int nrhs,
      const double *a, int lda, double *b, int ldb);

int
geqrf(int m, int n, float *a, int lda, float *tau, float *work, int lwork);

int
geqrf(int m, int n, double *a, int lda, double *tau, double *work, int lwork);

int
orgqr(int m, int n, int k, float *a, int lda, const float *tau,
      float *work, int lwork);

int
orgqr(int m, int n, int k, double *a, int lda, const double *tau,
      double *work, int lwork);

int
ormqr(BlasSide side, Transpose trans, int m, int n, int k,
      const float *a, int lda, const float *tau, float *c, int ldc,
      float *work, int lwork);

int
ormqr(BlasSide side, Transpose trans, int m, int n, int k,
      const double *a, int lda, const double *tau, double *c, int ldc,
      double *work, int lwork);

int
gels(Transpose trans, int m, int n, int nrhs, float *a, int lda,
     float *b, int ldb, float *work, int lwork);

int
gels(Transpose trans, int m, int n, int nrhs, double *a, int lda,
     double *b, int ldb, double *work, int lwork);

int
gels(Transpose trans, int m, int n, int nrhs, std::complex<double> *a, int lda,
     std::complex<double> *b, int ldb, std::complex<double> *work, int lwork);

int
gelss(int m, int n, int nrhs, float *a, int lda, float *b, int ldb,
     float *s, float rcond, int rank, float *work, int lwork);

int
gelss(int m, int n, int nrhs, double *a, int lda, double *b, int ldb,
     double *s, double rcond, int rank, double *work, int lwork);

int
gelss(int m, int n, int nrhs, std::complex<double> *a, int lda,
      std::complex<double> *b, int ldb, std::complex<double> *s,
      std::complex<double> rcond, int rank,
      std::complex<double> *work, int lwork);

int
geev(bool jobvl, bool jobvr, int n, float *a, int lda,
     float *wr, float *wi, float *vl, int ldvl, float *vr, int ldvr,
     float *work, int lwork);

int
geev(bool jobvl, bool jobvr, int n, double *a, int lda,
     double *wr, double *wi, double *vl, int ldvl, double *vr, int ldvr,
     double *work, int lwork);

int
geev(bool jobvl, bool jobvr, int n, std::complex<float> *a, int lda,
     std::complex<float> *w,
     std::complex<float> *vl, int ldvl,
     std::complex<float> *vr, int ldvr,
     std::complex<float> *work, int lwork, float *rwork);

int
geev(bool jobvl, bool jobvr, int n, std::complex<double> *a, int lda,
     std::complex<double> *w,
     std::complex<double> *vl, int ldvl,
     std::complex<double> *vr, int ldvr,
     std::complex<double> *work, int lwork, double *rwork);

int
gesvd(char jobu, char jobvt,
      int m, int n, float *a, int lda,
      float *s,
      float *u, int ldu,
      float *vt, int ldvt,
      float *work, int lwork);

int
gesvd(char jobu, char jobvt,
      int m, int n, double *a, int lda,
      double *s,
      double *u, int ldu,
      double *vt, int ldvt,
      double *work, int lwork);

} // namespace flens

#endif // FLENS_LAPACK_H
