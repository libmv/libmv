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

// == TinyGeMatrix =============================================================

template <typename A>
TinyGeMatrix<A>::TinyGeMatrix()
{
}

// -- operators ----------------------------------------------------------------

template <typename A>
ListInitializer<A>
TinyGeMatrix<A>::operator=(const T &value)
{
    return ListInitializer<A>(0, 0, _engine, value);
}

template <typename A>
TinyGeMatrix<A> &
TinyGeMatrix<A>::operator=(const TinyGeMatrix<A> &rhs)
{
    copy(rhs, *this);
    return *this;
}

template <typename A>
template <typename RHS>
TinyGeMatrix<A> &
TinyGeMatrix<A>::operator=(const Matrix<RHS> &rhs)
{
    copy(rhs.impl(), *this);
    return *this;
}

template <typename A>
template <typename RHS>
TinyGeMatrix<A> &
TinyGeMatrix<A>::operator+=(const Matrix<RHS> &rhs)
{
    axpy(T(1), rhs.impl(), *this);
    return *this;
}

template <typename A>
template <typename RHS>
TinyGeMatrix<A> &
TinyGeMatrix<A>::operator-=(const Vector<RHS> &rhs)
{
    axpy(T(-1), rhs.impl(), *this);
    return *this;
}

template <typename A>
const typename TinyGeMatrix<A>::T &
TinyGeMatrix<A>::operator()(int row, int col) const
{
    return _engine(row, col);
}

template <typename A>
typename TinyGeMatrix<A>::T &
TinyGeMatrix<A>::operator()(int row, int col)
{
    return _engine(row, col);
}

//-- methods -------------------------------------------------------------------

template <typename A>
void
TinyGeMatrix<A>::resize(int numRows, int numCols)
{
    assert((numRows==this->numRows()) && (numCols==this->numCols()));
}

template <typename A>
void
TinyGeMatrix<A>::resize(int numRows, int numCols, int firstRow, int firstCol)
{
    assert((numRows==this->numRows()) && (numCols==this->numCols()));
    assert((firstRow==this->firstRow()) && (firstCol==this->firstCol()));
}

template <typename A>
int
TinyGeMatrix<A>::numRows() const
{
    return _engine.numRows();
}

template <typename A>
int
TinyGeMatrix<A>::numCols() const
{
    return _engine.numCols();
}

template <typename A>
int
TinyGeMatrix<A>::firstRow() const
{
    return _engine.firstRow();
}

template <typename A>
int
TinyGeMatrix<A>::lastRow() const
{
    return _engine.lastRow();
}

template <typename A>
int
TinyGeMatrix<A>::firstCol() const
{
    return _engine.firstCol();
}

template <typename A>
int
TinyGeMatrix<A>::lastCol() const
{
    return _engine.lastCol();
}

template <typename A>
const A &
TinyGeMatrix<A>::engine() const
{
    return _engine;
}

template <typename A>
A &
TinyGeMatrix<A>::engine()
{
    return _engine;
}

} // namespace flens
