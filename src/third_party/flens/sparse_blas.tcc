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

#include <flens/storage.h>

namespace flens {

//-- crs - compressed row storage ----------------------------------------------

template <typename T>
void
crs_gemv(Transpose trans, int m, int n, T alpha,
         const T *a, const int *ia, const int *ja,
         const T *x, T beta, T *y)
{
    assert((beta==T(0)) || (beta==T(1)));

    // shift to index base 1
    a = a-1;
    ia = ia-1;
    ja = ja-1;
    x = x-1;
    y = y-1;

    const bool init = (beta==T(0));

    if (trans==NoTrans) {
        if (init) {
            for (int i=1; i<=m; ++i) {
                y[i] = T(0);
                for (int k=ia[i]; k<ia[i+1]; ++k) {
                    y[i] += alpha*a[k]*x[ja[k]];
                }
            }
        } else {
            for (int i=1; i<=m; ++i) {
                for (int k=ia[i]; k<ia[i+1]; ++k) {
                    y[i] += alpha*a[k]*x[ja[k]];
                }
            }
        }
    } else {
        if (init) {
            for (int i=1; i<=n; ++i) {
                y[i] = T(0);
            }
        }
        for (int i=1; i<=m; ++i) {
            for (int k=ia[i]; k<ia[i+1]; ++k) {
                y[ja[k]] += alpha*a[k]*x[i];
            }
        }
    }
}

template <typename T>
void
crs_symv(StorageUpLo upLo, int m, T alpha,
         const T *a, const int *ia, const int *ja,
         const T *x, T beta, T *y)
{
    assert((beta==T(0)) || (beta==T(1)));

    // shift to index base 1
    a = a-1;
    ia = ia-1;
    ja = ja-1;
    x = x-1;
    y = y-1;

    if (beta==T(0)) {
        for (int i=1; i<=m; ++i) {
            y[i] = 0;
        }
    }

    T a_k, x_i, y_i;
    if (upLo==Upper) {
        for (int i=1; i<=m; ++i) {
            assert(ja[ia[i]]==i);
            y_i = y[i];
            x_i = x[i];
            y_i += alpha*a[ia[i]]*x_i;
            for (int k=ia[i]+1; k<ia[i+1]; ++k) {
                a_k = a[k];
                y_i += alpha*a_k*x[ja[k]];
                y[ja[k]] += alpha*a_k*x_i;
            }
            y[i] = y_i;
        }
    } else {
        for (int i=1; i<=m; ++i) {
            y_i = y[i];
            x_i = x[i];
            for (int k=ia[i]; k<ia[i+1]-1; ++k) {
                a_k = a[k];
                y_i += alpha*a_k*x[ja[k]];
                y[ja[k]] += alpha*a_k*x_i;
            }
            assert(ja[ia[i+1]-1]==i);
            y_i += alpha*a[ia[i+1]-1]*x_i;
            y[i] = y_i;
        }
    }
}

} // namespace flens
