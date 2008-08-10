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

#ifndef FLENS_TINYMATRIX_H
#define FLENS_TINYMATRIX_H 1

#include <flens/matvec.h>
#include <flens/listinitializer.h>
#include <flens/fixedsizearray.h>

namespace flens {

// == TinyGeMatrix =============================================================

template <typename A>
class TinyGeMatrix
    : public GeneralMatrix<TinyGeMatrix<A> >
{
    public:
        typedef typename TypeInfo<TinyGeMatrix<A> >::ElementType T;        
        
        TinyGeMatrix();

        // -- operators --------------------------------------------------------

        ListInitializer<A>
        operator=(const T &value);

        TinyGeMatrix<A> &
        operator=(const TinyGeMatrix<A> &rhs);

        template <typename RHS>
            TinyGeMatrix<A> &
            operator=(const Matrix<RHS> &rhs);

        template <typename RHS>
            TinyGeMatrix<A> &
            operator+=(const Matrix<RHS> &rhs);

        template <typename RHS>
            TinyGeMatrix<A> &
            operator-=(const Vector<RHS> &rhs);

        template <typename RHS>
            TinyGeMatrix<A> &
            operator*=(const Matrix<RHS> &rhs);

        template <typename RHS>
            bool
            operator==(const TinyGeMatrix<RHS> &rhs) const;

        TinyGeMatrix<A> &
        operator*=(const T);

        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        //-- methods -----------------------------------------------------------

        void
        resize(int numRows, int numCols);

        void
        resize(int numRows, int numCols, int firstRow, int firstCol);

        int
        numRows() const;

        int
        numCols() const;

        int
        firstRow() const;

        int
        lastRow() const;

        int
        firstCol() const;

        int
        lastCol() const;

        const A &
        engine() const;

        A &
        engine();

    private:
        A _engine;
};

template <typename A>
struct TypeInfo<TinyGeMatrix<A> >
{
    typedef TinyGeMatrix<A> Impl;
    typedef typename A::ElementType ElementType;
};

} // namespace flens

#include <flens/tinymatrix.tcc>

#endif // FLENS_TINYMATRIX_H
