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

namespace flens {

//== SparseGeMatrix ============================================================

// -- constructors -------------------------------------------------------------
template <typename E>
SparseGeMatrix<E>::SparseGeMatrix()
	: _initializer(0)
{
}

template <typename E>
SparseGeMatrix<E>::SparseGeMatrix(int numRows, int numCols, int k)
    : _engine(numRows, numCols, k)
{
    _initializer = engine().initializer();
}

// -- operators ----------------------------------------------------------------
template <typename E>
template <typename RHS>
SparseGeMatrix<E> &
SparseGeMatrix<E>::operator=(const Matrix<RHS> &rhs)
{
    copy(rhs.impl(), *this);
//    _initializer = 0;
    return *this;
}

template <typename E>
SparseGeMatrix<E> &
SparseGeMatrix<E>::operator*=(T alpha)
{
    engine() *= alpha;
    return *this;
}

template <typename E>
SparseGeMatrix<E> &
SparseGeMatrix<E>::operator/=(T alpha)
{
    engine() /= alpha;
    return *this;
}

template <typename E>
typename SparseGeMatrix<E>::T &
SparseGeMatrix<E>::operator()(int row, int col)
{
    return _initializer->operator()(row,col);
}

// -- methods ------------------------------------------------------------------
template <typename E>
void
SparseGeMatrix<E>::finalize()
{
    delete _initializer;
    _initializer = 0;
}

template <typename E>
int
SparseGeMatrix<E>::numRows() const
{
    return _engine.numRows();
}

template <typename E>
int
SparseGeMatrix<E>::numCols() const
{
    return _engine.numCols();
}

template <typename E>
int
SparseGeMatrix<E>::numNonZeros() const
{
    assert(!_initializer);

    return _engine.numNonZeros();
}

template <typename E>
typename SparseGeMatrix<E>::const_iterator
SparseGeMatrix<E>::begin() const
{
    assert(!_initializer);

    return _engine.begin();
}

template <typename E>
typename SparseGeMatrix<E>::iterator
SparseGeMatrix<E>::begin()
{
    assert(!_initializer);

    return _engine.begin();
}

template <typename E>
typename SparseGeMatrix<E>::const_iterator
SparseGeMatrix<E>::end() const
{
    assert(!_initializer);

    return _engine.end();
}

template <typename E>
typename SparseGeMatrix<E>::iterator
SparseGeMatrix<E>::end()
{
    assert(!_initializer);

    return _engine.end();
}

template <typename E>
void
SparseGeMatrix<E>::resize(int m, int n, int k)
{
    delete _initializer;
    _engine = E(m, n, k);
    _initializer = _engine.initializer();
}

// -- implementation -----------------------------------------------------------
template <typename E>
const E &
SparseGeMatrix<E>::engine() const
{
    return _engine;
}

template <typename E>
E &
SparseGeMatrix<E>::engine()
{
    return _engine;
}

//== SparseSyMatrix ============================================================

// -- constructors -------------------------------------------------------------
template <typename E>
SparseSyMatrix<E>::SparseSyMatrix(int dim, int k)
    : _engine(dim, dim, k)
{
    _initializer = engine().initializer();
}

// -- operators ----------------------------------------------------------------
template <typename E>
SparseSyMatrix<E> &
SparseSyMatrix<E>::operator*=(T alpha)
{
    assert(!_initializer);

    engine() *= alpha;
    return *this;
}

template <typename E>
SparseSyMatrix<E> &
SparseSyMatrix<E>::operator/=(T alpha)
{
    assert(!_initializer);

    engine() /= alpha;
    return *this;
}

template <typename E>
typename SparseSyMatrix<E>::T &
SparseSyMatrix<E>::operator()(int row, int col)
{
    return _initializer->operator()(row,col);
}

// -- methods ------------------------------------------------------------------
template <typename E>
void
SparseSyMatrix<E>::finalize()
{
    delete _initializer;
    _initializer = 0;
}

template <typename E>
int
SparseSyMatrix<E>::dim() const
{
    assert(_engine.numRows()==_engine.numCols());
    return _engine.numRows();
}

template <typename E>
int
SparseSyMatrix<E>::numNonZeros() const
{
    assert(!_initializer);

    return _engine.numNonZeros();
}

template <typename E>
typename SparseSyMatrix<E>::const_iterator
SparseSyMatrix<E>::begin() const
{
    assert(!_initializer);

    return _engine.begin();
}

template <typename E>
typename SparseSyMatrix<E>::iterator
SparseSyMatrix<E>::begin()
{
    assert(!_initializer);

    return _engine.begin();
}

template <typename E>
typename SparseSyMatrix<E>::const_iterator
SparseSyMatrix<E>::end() const
{
    assert(!_initializer);

    return _engine.end();
}

template <typename E>
typename SparseSyMatrix<E>::iterator
SparseSyMatrix<E>::end()
{
    assert(!_initializer);

    return _engine.end();
}

// -- implementation -----------------------------------------------------------
template <typename E>
const E &
SparseSyMatrix<E>::engine() const
{
    return _engine;
}

template <typename E>
E &
SparseSyMatrix<E>::engine()
{
    return _engine;
}

//== SparseSymmertricMatrix ====================================================

// -- constructors -------------------------------------------------------------
template <typename E>
SparseSymmetricMatrix<E>::SparseSymmetricMatrix(int numRows, int numCols)
    : _engine(numRows, numCols)
{
    _initializer = engine().initializer();
}

// -- operators ----------------------------------------------------------------
template <typename E>
typename E::ElementType &
SparseSymmetricMatrix<E>::operator()(int row, int col)
{
    return _initializer->operator()(std::min(row,col), std::max(row,col));
}

// -- methods ------------------------------------------------------------------
template <typename E>
void
SparseSymmetricMatrix<E>::finalize()
{
    delete _initializer;
    _initializer = 0;
}

template <typename E>
int
SparseSymmetricMatrix<E>::numRows() const
{
    return _engine.numRows();
}

template <typename E>
int
SparseSymmetricMatrix<E>::numCols() const
{
    return _engine.numCols();
}

template <typename E>
int
SparseSymmetricMatrix<E>::numNonZeros() const
{
    assert(!_initializer);

    return _engine.numNonZeros();
}

template <typename E>
typename SparseSymmetricMatrix<E>::const_iterator
SparseSymmetricMatrix<E>::begin() const
{
    assert(!_initializer);

    return _engine.begin();
}

template <typename E>
typename SparseSymmetricMatrix<E>::const_iterator
SparseSymmetricMatrix<E>::end() const
{
    assert(!_initializer);

    return _engine.end();
}

template <typename E>
const E &
SparseSymmetricMatrix<E>::engine() const
{
    return _engine;
}

template <typename E>
E &
SparseSymmetricMatrix<E>::engine()
{
    return _engine;
}

} // namespace flens
