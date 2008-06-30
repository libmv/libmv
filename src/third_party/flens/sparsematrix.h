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

#ifndef FLENS_SPARSEMATRIX_H
#define FLENS_SPARSEMATRIX_H 1

namespace flens {

//== SparseGeMatrix ============================================================

template <typename Engine>
class SparseGeMatrix
    : public GeneralMatrix<SparseGeMatrix<Engine> >
{
    public:
        // shortcut for element type
        typedef typename SparseGeMatrix<Engine>::ElementType  T;

        typedef typename Engine::Initializer     Initializer;
        typedef typename Engine::const_iterator  const_iterator;
        typedef typename Engine::iterator        iterator;

        // -- constructors -----------------------------------------------------
        SparseGeMatrix();

        SparseGeMatrix(int numRows, int numCols, int k=1);

        // -- operators --------------------------------------------------------
        template <typename RHS>
            SparseGeMatrix<Engine> &
            operator=(const Matrix<RHS> &rhs);

        SparseGeMatrix &
        operator*=(T alpha);

        SparseGeMatrix &
        operator/=(T alpha);

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------
        void
        finalize();

        int
        numRows() const;

        int
        numCols() const;

        int
        numNonZeros() const;

        const_iterator
        begin() const;

        iterator
        begin();

        const_iterator
        end() const;

        iterator
        end();

        // TODO: discuss with Michael
        void
        resize(int m, int n, int k=1);

        // TODO: discuss with Michael
        //template <typename RHS>
        //void initWith(const Matrix<RHS> &rhs, double eps=0.);

        // -- implementation ---------------------------------------------------
        const Engine &
        engine() const;

        Engine &
        engine();

    private:
        Engine       _engine;
        Initializer *_initializer;
};

template <typename I>
struct TypeInfo<SparseGeMatrix<I> >
{
    typedef SparseGeMatrix<I> Impl;
    typedef typename I::ElementType ElementType;
};

//== SparseSyMatrix ============================================================

template <typename Engine>
class SparseSyMatrix
    : public SymmetricMatrix<SparseSyMatrix<Engine> >
{
    public:
        // shortcut for element type
        typedef typename SparseSyMatrix<Engine>::ElementType  T;

        typedef typename Engine::Initializer     Initializer;
        typedef typename Engine::const_iterator  const_iterator;
        typedef typename Engine::iterator        iterator;

        // -- constructors -----------------------------------------------------
        SparseSyMatrix(int dim, int k=1);

        // -- operators --------------------------------------------------------
        SparseSyMatrix &
        operator*=(T alpha);

        SparseSyMatrix &
        operator/=(T alpha);

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------
        void
        finalize();

        int
        dim() const;

        int
        numNonZeros() const;

        const_iterator
        begin() const;

        iterator
        begin();

        const_iterator
        end() const;

        iterator
        end();

        // -- implementation ---------------------------------------------------
        const Engine &
        engine() const;

        Engine &
        engine();

    private:
        Engine       _engine;
        Initializer *_initializer;
};

template <typename I>
struct TypeInfo<SparseSyMatrix<I> >
{
    typedef SparseSyMatrix<I> Impl;
    typedef typename I::ElementType ElementType;
};

//== SparseSymmertricMatrix ====================================================

template <typename Engine>
class SparseSymmetricMatrix
    : public Matrix<SparseSymmetricMatrix<Engine> >
{
    public:
        typedef typename Engine::ElementType     T;
        typedef typename Engine::Initializer     Initializer;
        typedef typename Engine::const_iterator  const_iterator;

        // -- constructors -----------------------------------------------------
        SparseSymmetricMatrix(int numRows, int numCols);

        // -- operators --------------------------------------------------------
        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------
        void
        finalize();

        int
        numRows() const;

        int
        numCols() const;

        int
        numNonZeros() const;

        const_iterator
        begin() const;

        const_iterator
        end() const;

        // -- implementation ---------------------------------------------------
        const Engine &
        engine() const;

        Engine &
        engine();

    private:
        Engine       _engine;
        Initializer *_initializer;
};

template <typename I>
struct TypeInfo<SparseSymmetricMatrix<I> >
{
    typedef SparseSymmetricMatrix<I> Impl;
    typedef typename I::ElementType ElementType;
};

} // namespace flens

#include <flens/sparsematrix.tcc>

#endif // FLENS_SPARSEMATRIX_H
