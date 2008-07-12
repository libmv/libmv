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

#include <algorithm>
#include <cassert>

namespace flens {

//== Array =====================================================================

template <typename T>
ListInitializer<Array<T> >::ListInitializer(T *begin, int length, T value)
    : _it(begin), _end(begin+length)
{
    assert(_it!=_end);
    *_it = value;
}

template <typename T>
ListInitializer<Array<T> >::ListInitializer(T *begin, T *end, T value)
    : _it(begin), _end(end)
{
    assert(_it!=_end);
    *_it = value;
}

template <typename T>
ListInitializer<Array<T> >
ListInitializer<Array<T> >::operator,(T value)
{
    ++_it;
    assert(_it<_end);
    return ListInitializer<Array<T> >(_it, _end, value);
}

//== DenseVector ===============================================================

template <typename Engine>
ListInitializer<DenseVector<Engine> >::ListInitializer(T *begin, int stride,
                                                       int length, T value)
    : _it(begin), _stride(stride), _end(begin+stride*length)
{
    assert(_it!=_end);
    *_it = value;
}

template <typename Engine>
ListInitializer<DenseVector<Engine> >::ListInitializer(T *begin, int stride,
                                                       T *end, T value)
    : _it(begin), _stride(stride), _end(end)
{
    assert(_it!=_end);
    *_it = value;
}

template <typename Engine>
ListInitializer<DenseVector<Engine> >
ListInitializer<DenseVector<Engine> >::operator,(T value)
{
    _it += _stride;
    assert(_it<_end);
    return ListInitializer<DenseVector<Engine> >(_it, _stride, _end, value);
}

//------------------------------------------------------------------------------

template <typename I>
ListInitializerSwitch<DenseVector<I> >::ListInitializerSwitch(
                                                            T *it, int stride,
                                                            int length, T value)
    : _it(it), _stride(stride), _end(it+length*stride), _value(value),
      _wipeOnDestruct(true)
{
}

template <typename I>
ListInitializerSwitch<DenseVector<I> >::ListInitializerSwitch(
                                const ListInitializerSwitch<DenseVector<I> > &l)
    : _it(l._it), _stride(l._stride), _end(l._end), _value(l._value),
      _wipeOnDestruct(false)
{
}

template <typename I>
ListInitializerSwitch<DenseVector<I> >::~ListInitializerSwitch()
{
    if (_wipeOnDestruct) {
        if (_stride == 1) {
            std::fill(_it,_end, _value);
        } else {
            for (T* p=_it; p<_end; p+=_stride) {
                *p = _value;
            }
        }
    }
}

template <typename I>
ListInitializer<DenseVector<I> >
ListInitializerSwitch<DenseVector<I> >::operator,(T x)
{
    assert(_it!=_end);
    _wipeOnDestruct = false;
    *_it = _value;
    return ListInitializer<DenseVector<I> >(_it+=_stride, _stride, _end, x);
}

//== GeMatrix ==================================================================

template <typename Engine>
ListInitializer<GeMatrix<Engine> >::ListInitializer(GeMatrix<Engine> &A,
                                                    int row, int col, T value)
    : _A(A), _row(row), _col(col)
{
    _A(row,col) = value;
}

template <typename Engine>
ListInitializer<GeMatrix<Engine> >
ListInitializer<GeMatrix<Engine> >::operator,(T value)
{
    (_col<_A.lastCol()) ? ++_col
                        : (_col=_A.firstCol(), ++_row);
    return ListInitializer<GeMatrix<Engine> >(_A, _row, _col, value);
}

//------------------------------------------------------------------------------

template <typename I>
ListInitializerSwitch<GeMatrix<I> >::ListInitializerSwitch(
                                      GeMatrix<I> &A, int row, int col, T value)
    : _A(A), _row(row), _col(col), _value(value), _wipeOnDestruct(true)
{
}

template <typename I>
ListInitializerSwitch<GeMatrix<I> >::ListInitializerSwitch(
                                   const ListInitializerSwitch<GeMatrix<I> > &l)
    : _A(l._A), _row(l._row), _col(l._col), _value(l._value),
      _wipeOnDestruct(false)
{
}

template <typename I>
ListInitializerSwitch<GeMatrix<I> >::~ListInitializerSwitch()
{
    if (_wipeOnDestruct) {
        assert(_row==_A.firstRow());
        assert(_col==_A.firstCol());
        for (int i=_A.firstRow(); i<=_A.lastRow(); ++i) {
            for (int j=_A.firstCol(); j<=_A.lastCol(); ++j) {
                _A(i,j) = _value;
            }
        }
    }
}

template <typename I>
ListInitializer<GeMatrix<I> >
ListInitializerSwitch<GeMatrix<I> >::operator,(T x)
{
    _wipeOnDestruct = false;
    _A(_row,_col) = _value;
    return ListInitializer<GeMatrix<I> >(_A, _row, ++_col, x);
}

//== FixedSizeArray1D ==========================================================

template <typename T, int N>
ListInitializer<FixedSizeArray1D<T,N> >::ListInitializer(T *begin, T value)
    : _it(begin), _end(begin+N)
{
    assert(_it!=_end);
    *_it = value;
}

template <typename T, int N>
ListInitializer<FixedSizeArray1D<T,N> >::ListInitializer(T *begin,
                                                         T *end,
                                                         T value)
    : _it(begin), _end(end)
{
    assert(_it!=_end);
    *_it = value;
}

template <typename T, int N>
ListInitializer<FixedSizeArray1D<T,N> >
ListInitializer<FixedSizeArray1D<T,N> >::operator,(T value)
{
    ++_it;
    assert(_it<_end);
    return ListInitializer<FixedSizeArray1D<T,N> >(_it, _end, value);
}

//== FixedSizeArray2D ==========================================================

template <typename T, int M, int N>
ListInitializer<FixedSizeArray2D<T,M,N> >::ListInitializer(int _i, int _j,
                                                           Data &_data,
                                                           T value)
    : i(_i), j(_j), data(_data)
{
    data(i,j) = value;
}

template <typename T, int M, int N>
ListInitializer<FixedSizeArray2D<T,M,N> >
ListInitializer<FixedSizeArray2D<T,M,N> >::operator,(T value)
{
    assert((i<M)&&(j<N));

    ++j;
    if (j==N) {
        j=0;
        ++i;
    }
    return ListInitializer<FixedSizeArray2D<T,M,N> >(i, j, data, value);
}

} // namespace flens
