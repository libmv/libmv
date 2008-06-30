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

#ifndef FLENS_BANDSTORAGE_H
#define FLENS_BANDSTORAGE_H 1

#include <flens/fullstorage.h>

namespace flens {

// forward decalarations
template <typename T, StorageOrder Order>
    class BandStorage;

template <typename T, StorageOrder Order>
    class BandStorageView;

template <typename T, StorageOrder Order>
class ConstBandStorageView
{
    public:
        typedef T                              ElementType;
        typedef ConstBandStorageView           ConstView;
        typedef BandStorageView<T, Order>      View;
        typedef BandStorage<T, Order>          NoView;

        typedef ConstArrayView<T>              ConstVectorView;
        typedef ArrayView<T>                   VectorView;
        typedef Array<T>                       VectorNoView;


        ConstBandStorageView(const ConstFullStorageView<T, ColMajor> &data,
                             int numRows, int numCols, int indexBase=1);

        ConstBandStorageView(const FullStorageView<T, ColMajor> &data,
                             int numRows, int numCols, int indexBase=1);

        ConstBandStorageView(const ConstBandStorageView<T, Order> &rhs);

        ~ConstBandStorageView();

        //-- operators ---------------------------------------------------------

        const T &
        operator()(int row, int col) const;

        //-- methods -----------------------------------------------------------

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
        numSubDiags() const;

        int
        numSuperDiags() const;

        int
        leadingDimension() const;

        const T *
        data() const;

        ConstArrayView<T>
        viewDiag(int diag, int viewIndexBase=1) const;

        ConstBandStorageView<T, Order>
        viewDiags(int fromDiag, int toDiag, int viewIndexBase=1) const;

    private:
        int _numRows, _numCols, _indexBase;
        ConstFullStorageView<T, ColMajor> _data;
};

//------------------------------------------------------------------------------

template <typename T, StorageOrder Order>
struct StorageInfo<ConstBandStorageView<T, Order> >
{
    static const StorageOrder order = Order;
};

//------------------------------------------------------------------------------

template <typename T, StorageOrder Order>
class BandStorageView
{
    public:
        typedef T                              ElementType;
        typedef ConstBandStorageView<T, Order> ConstView;
        typedef BandStorageView                View;
        typedef BandStorage<T, Order>          NoView;

        typedef ConstArrayView<T>              ConstVectorView;
        typedef ArrayView<T>                   VectorView;
        typedef Array<T>                       VectorNoView;


        BandStorageView(const FullStorageView<T, ColMajor> &data,
                        int numRows, int numCols, int indexBase=1);

        BandStorageView(const BandStorageView<T, Order> &rhs);

        ~BandStorageView();

        //-- operators ---------------------------------------------------------

        BandStorageView<T, Order> &
        operator=(const BandStorage<T, Order> &rhs);

        BandStorageView<T, Order> &
        operator=(const BandStorageView<T, Order> &rhs);

        BandStorageView<T, Order> &
        operator=(const ConstBandStorageView<T, Order> &rhs);

        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        operator ConstView() const;

        //-- methods -----------------------------------------------------------

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
        numSubDiags() const;

        int
        numSuperDiags() const;

        int
        leadingDimension() const;

        const T *
        data() const;

        T *
        data();

        void
        resize(int numRows, int numCols, int numSubDiags, int numSuperDiags,
               int indexBase=1);

        ConstArrayView<T>
        viewDiag(int diag, int viewIndexBase=1) const;

        ArrayView<T>
        viewDiag(int diag, int viewIndexBase=1);

        ConstBandStorageView<T, Order>
        viewDiags(int fromDiag, int toDiag, int viewIndexBase=1) const;

        BandStorageView<T, Order>
        viewDiags(int fromDiag, int toDiag, int viewIndexBase=1);

    private:
        int _numRows, _numCols, _indexBase;
        FullStorageView<T, ColMajor> _data;
};

//------------------------------------------------------------------------------

template <typename T, StorageOrder Order>
struct StorageInfo<BandStorageView<T, Order> >
{
    static const StorageOrder order = Order;
};

//==============================================================================

template <typename T, StorageOrder Order>
class BandStorage
{
    public:
        typedef T                              ElementType;
        typedef ConstBandStorageView<T, Order> ConstView;
        typedef BandStorageView<T, Order>      View;
        typedef BandStorage                    NoView;

        typedef ConstArrayView<T>              ConstVectorView;
        typedef ArrayView<T>                   VectorView;
        typedef Array<T>                       VectorNoView;

        BandStorage();

        BandStorage(int numRows, int numCols,
                    int numSubDiags, int numSuperDiags,
                    int indexBase=1);

        BandStorage(const BandStorage<T, Order> &rhs);

        ~BandStorage();

        //-- operators ---------------------------------------------------------

        BandStorage<T, Order> &
        operator=(const BandStorage<T, Order> &rhs);

        const T &
        operator()(int row, int col) const;

        T &
        operator()(int row, int col);

        //-- methods -----------------------------------------------------------

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
        numSubDiags() const;

        int
        numSuperDiags() const;

        int
        leadingDimension() const;

        const T *
        data() const;

        T *
        data();

        void
        resize(int numRows, int numCols, int numSubDiags, int numSuperDiags,
               int indexBase=1);

        ConstArrayView<T>
        viewDiag(int diag, int viewIndexBase=1) const;

        ArrayView<T>
        viewDiag(int diag, int viewIndexBase=1);

        ConstBandStorageView<T, Order>
        viewDiags(int fromDiag, int toDiag, int viewIndexBase=1) const;

        BandStorageView<T, Order>
        viewDiags(int fromDiag, int toDiag, int viewIndexBase=1);

    private:
        int _numRows, _numCols, _indexBase;
        FullStorage<T, ColMajor> _data;
};

//------------------------------------------------------------------------------

template <typename T, StorageOrder Order>
struct StorageInfo<BandStorage<T, Order> >
{
    static const StorageOrder order = Order;
};

} // namespace flens

#include <flens/bandstorage.tcc>

#endif // FLENS_BANDSTORAGE_H
