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

#ifndef FLENS_LAPACK_FLENS_H
#define FLENS_LAPACK_FLENS_H 1

#include <flens/densevector.h>
#include <flens/generalmatrix.h>

namespace flens {

//-- getrf ---------------------------------------------------------------------

template <typename FS>
    int
    trf(GeMatrix<FS> &A, DenseVector<Array<int> > &P);

//-- getri ---------------------------------------------------------------------

template <typename FS>
    int
    tri(GeMatrix<FS> &A, DenseVector<Array<int> > &P);

//-- gbtrf ---------------------------------------------------------------------

template <typename BS>
    int
    trf(GbMatrix<BS> &A, DenseVector<Array<int> > &P);

//-- getrs ---------------------------------------------------------------------

template <typename MA, typename MB>
    int
    trs(Transpose trans, const GeMatrix<MA> &LU,
        const DenseVector<Array<int> > &P, GeMatrix<MB> &B);

template <typename MA, typename VB>
    int
    trs(Transpose trans, const GeMatrix<MA> &LU,
        const DenseVector<Array<int> > &P,
        DenseVector<VB> &B);

//-- gbtrs ---------------------------------------------------------------------

template <typename MA, typename MB>
    int
    trs(Transpose trans, const GbMatrix<MA> &LU,
        const DenseVector<Array<int> > &P, GeMatrix<MB> &B);

template <typename MA, typename VB>
    int
    trs(Transpose trans, const GbMatrix<MA> &LU,
        const DenseVector<Array<int> > &P,
        DenseVector<VB> &B);

//-- gesv ----------------------------------------------------------------------

template <typename MA, typename MB>
    int
    sv(GeMatrix<MA> &A, DenseVector<Array<int> > &P, GeMatrix<MB> &B);

template <typename MA, typename VB>
    int
    sv(GeMatrix<MA> &A, DenseVector<Array<int> > &P, DenseVector<VB> &B);

//-- gbsv ----------------------------------------------------------------------

template <typename MA, typename MB>
    int
    sv(GbMatrix<MA> &A, DenseVector<Array<int> > &P, GeMatrix<MB> &B);

template <typename MA, typename VB>
    int
    sv(GbMatrix<MA> &A, DenseVector<Array<int> > &P, DenseVector<VB> &B);

//-- trtrs ---------------------------------------------------------------------

template <typename MA, typename MB>
    int
    trs(Transpose trans, const TrMatrix<MA> &A, GeMatrix<MB> &B);

template <typename MA, typename VB>
    int
    trs(Transpose trans, const TrMatrix<MA> &A, DenseVector<VB> &B);

//-- geqrf ---------------------------------------------------------------------

template <typename MA, typename VT>
    int
    qrf(GeMatrix<MA> &A, DenseVector<VT> &tau);

//-- orgqr ---------------------------------------------------------------------

template <typename MA, typename VT>
    int
    orgqr(GeMatrix<MA> &A, const DenseVector<VT> &tau);

//-- ormqr ---------------------------------------------------------------------

template <typename MA, typename VT, typename MC>
int
ormqr(BlasSide side, Transpose trans,
      const GeMatrix<MA> &A, const DenseVector<VT> &tau,
      GeMatrix<MC> &C);

//-- gels ----------------------------------------------------------------------

template <typename MA, typename MB>
    int
    ls(Transpose trans, GeMatrix<MA> &A, GeMatrix<MB> &B);

//-- gelss ---------------------------------------------------------------------

template <typename MA, typename MB>
    int
    lss(GeMatrix<MA> &A, GeMatrix<MB> &B);

//-- geev ----------------------------------------------------------------------

template <typename MA, typename WR, typename WI, typename VL, typename VR>
    int
    ev(bool leftEV, bool rigthEV,
       GeMatrix<MA> &A, DenseVector<WR> &wr, DenseVector<WI> &wi,
       GeMatrix<VL> &vl, GeMatrix<VR> &vr);

template <typename MA, typename W, typename VL, typename VR>
    int
    ev(bool leftEv, bool rigthEv,
       GeMatrix<MA> &A, DenseVector<W> &w, GeMatrix<VL> &vl, GeMatrix<VR> &vr);

//-- gesvd ---------------------------------------------------------------------

template <typename MA, typename VS, typename MU, typename MV>
int
svd(GeMatrix<MA> &A,
    DenseVector<VS> &s,
    GeMatrix<MU> &U,
    GeMatrix<MV> &V);

}; // namespace flens

#include <flens/lapack_flens.tcc>

#endif // FLENS_LAPACK_FLENS_H
