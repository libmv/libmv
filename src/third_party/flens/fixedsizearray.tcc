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

#include <flens/tiny_blas.h>

namespace flens {

//== FixedSizeArray1D ==========================================================

template <typename T, int N>
FixedSizeArray1D<T,N>::FixedSizeArray1D()
{
}

template <typename T, int N>
FixedSizeArray1D<T,N>::FixedSizeArray1D(const FixedSizeArray1D<T,N> &rhs)
{
    copy<N,T,1,T,1>(rhs._data, _data);
}

template <typename T, int N>
FixedSizeArray1D<T,N>::~FixedSizeArray1D()
{
}

// -- operators --------------------------------------------------------

template <typename T, int N>
ListInitializer<FixedSizeArray1D<T,N> >
FixedSizeArray1D<T,N>::operator=(const T &value)
{
    return ListInitializer<FixedSizeArray1D<T,N> >(_data, value);
}

template <typename T, int N>
FixedSizeArray1D<T, N>
FixedSizeArray1D<T,N>::operator=(const FixedSizeArray1D<T, N> &rhs)
{
    copy<N,T,1,T,1>(rhs._data, _data);
    return *this;
}
        
template <typename T, int N>
const T &
FixedSizeArray1D<T,N>::operator()(int index) const
{
    assert(index>=0);
    assert(index<N);
    return _data[index];
}

template <typename T, int N>
T &
FixedSizeArray1D<T,N>::operator()(int index)
{
    assert(index>=0);
    assert(index<N);
    return _data[index];
}

// -- methods ----------------------------------------------------------

template <typename T, int N>
int
FixedSizeArray1D<T,N>::firstIndex() const
{
    return TypeInfo<FixedSizeArray1D<T,N> >::firstIndex;
}

template <typename T, int N>
int
FixedSizeArray1D<T,N>::lastIndex() const
{
    return TypeInfo<FixedSizeArray1D<T,N> >::lastIndex;
}

template <typename T, int N>
int
FixedSizeArray1D<T,N>::length() const
{
    return TypeInfo<FixedSizeArray1D<T,N> >::length;
}

template <typename T, int N>
int
FixedSizeArray1D<T,N>::stride() const
{
    return TypeInfo<FixedSizeArray1D<T,N> >::stride;
}

template <typename T, int N>
const T *
FixedSizeArray1D<T,N>::data() const
{
    return _data;
}

template <typename T, int N>
T *
FixedSizeArray1D<T,N>::data()
{
    return _data;
}

//== FixedSizeArray2D ==========================================================

template <typename T, int M, int N>
FixedSizeArray2D<T,M,N>::FixedSizeArray2D()
{
}

template <typename T, int M, int N>
FixedSizeArray2D<T,M,N>::FixedSizeArray2D(const FixedSizeArray2D<T,M,N> &rhs)
{
    copy<M,N,T,T>(rhs._data, _data);
}

template <typename T, int M, int N>
FixedSizeArray2D<T,M,N>::~FixedSizeArray2D()
{
}

// -- operators --------------------------------------------------------

template <typename T, int M, int N>
ListInitializer<FixedSizeArray2D<T,M,N> >
FixedSizeArray2D<T,M,N>::operator=(const T &value)
{
    return ListInitializer<FixedSizeArray2D<T,M,N> >(0, 0, *this, value);
}

template <typename T, int M, int N>
FixedSizeArray2D<T,M,N>
FixedSizeArray2D<T,M,N>::operator=(const FixedSizeArray2D<T,M,N> &rhs)
{
    copy<M,N,T,T>(rhs._data, _data);
    return *this;
}
        
template <typename T, int M, int N>
const T &
FixedSizeArray2D<T,M,N>::operator()(int row, int col) const
{
    assert(row>=0);
    assert(row<M);
    assert(col>=0);
    assert(col<N);
    return _data[row][col];
}

template <typename T, int M, int N>
T &
FixedSizeArray2D<T,M,N>::operator()(int row, int col)
{
    assert(row>=0);
    assert(row<M);
    assert(col>=0);
    assert(col<N);
    return _data[row][col];
}

// -- methods ----------------------------------------------------------

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::firstRow() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::firstRow;
}

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::lastRow() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::lastRow;
}

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::firstCol() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::firstCol;
}

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::lastCol() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::lastCol;
}

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::numRows() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::numRows;
}

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::numCols() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::numCols;
}

template <typename T, int M, int N>
int
FixedSizeArray2D<T,M,N>::leadingDimension() const
{
    return TypeInfo<FixedSizeArray2D<T,M,N> >::leadingDimension;
}

//== IO ========================================================================

template <typename T, int N>
std::ostream &
operator<<(std::ostream &out, const FixedSizeArray1D<T,N> &x)
{
    for (int i=0; i<N; ++i) {
        out << x(i) << " ";
    }
    out << std::endl;
    return out;
}

template <typename T, int M, int N>
std::ostream &
operator<<(std::ostream &out, const FixedSizeArray2D<T,M,N> &x)
{
    for (int i=0; i<M; ++i) {
        for (int j=0; j<N; ++j) {
            out.width(11);
            out.setf(std::ios::fixed);
            out << x(i,j) << " ";
        }
        out << ";" << std::endl << "    ";
    }
    out << std::endl;
    return out;
}


} // namespace flens
