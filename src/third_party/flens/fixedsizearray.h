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

#ifndef FLENS_FIXEDSIZEARRAY_H
#define FLENS_FIXEDSIZEARRAY_H 1

#include <flens/listinitializer.h>

namespace flens {


//== FixedSizeArray1D ==========================================================

template <typename T, int N>
class FixedSizeArray1D
{
    public:
        typedef T ElementType;

        FixedSizeArray1D();

        FixedSizeArray1D(const FixedSizeArray1D<T,N> &rhs);

        ~FixedSizeArray1D();

        // -- operators --------------------------------------------------------

        ListInitializer<FixedSizeArray1D<T,N> >
        operator=(const T &value);

        FixedSizeArray1D<T, N>
        operator=(const FixedSizeArray1D<T, N> &rhs);
        
        const T &
        operator()(int index) const;

        T &
        operator()(int index);

        // -- methods ----------------------------------------------------------

        int
        firstIndex() const;

        int
        lastIndex() const;

        int
        length() const;

        int
        stride() const;
        
        const T *
        data() const;

        T *
        data();

        T _data[N];
};

template <typename T, int N>
struct TypeInfo<FixedSizeArray1D<T,N> >
{
    typedef T ElementType;
    
    static const int length = N;
    static const int stride = 1;
    static const int firstIndex = 0;
    static const int lastIndex = N-1;
};

//== FixedSizeArray2D ==========================================================

template <typename T, int M, int N>
class FixedSizeArray2D
{
    public:
        typedef T ElementType;

        FixedSizeArray2D();

        FixedSizeArray2D(const FixedSizeArray2D<T,M,N> &rhs);

        ~FixedSizeArray2D();

        // -- operators --------------------------------------------------------

        ListInitializer<FixedSizeArray2D<T,M,N> >
        operator=(const T &value);

        FixedSizeArray2D<T,M,N>
        operator=(const FixedSizeArray2D<T,M,N> &rhs);
        
        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        // -- methods ----------------------------------------------------------

        int
        firstRow() const;

        int
        lastRow() const;

        int
        firstCol() const;

        int
        lastCol() const;

        int
        numRows() const;

        int
        numCols() const;

        int
        leadingDimension() const;

        T _data[M][N];
};

template <typename T, int M, int N>
struct TypeInfo<FixedSizeArray2D<T,M,N> >
{
    typedef T ElementType;
    
    static const int numRows = M;
    static const int numCols = N;
    static const int leadingDimension = N;
    static const int firstRow = 0;
    static const int lastRow = M-1;
    static const int firstCol = 0;
    static const int lastCol = N-1;
};

//== IO ========================================================================

template <typename T, int N>
std::ostream &
operator<<(std::ostream &out, const FixedSizeArray1D<T,N> &x);

template <typename T, int M, int N>
std::ostream &
operator<<(std::ostream &out, const FixedSizeArray2D<T,M,N> &x);

} // namespace flens

#include <flens/fixedsizearray.tcc>

#endif // FLENS_FIXEDSIZEARRAY_H
