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

template <typename T, StorageOrder Order, StorageUpLo UpLo>
PackedStorage<T, Order, UpLo>::PackedStorage()
    : _dim(0)
{
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
PackedStorage<T, Order, UpLo>::PackedStorage(int dim, int indexBase)
    : _dim(dim), _indexBase(indexBase), _data(dim*(dim+1)/2, 0)
{
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
PackedStorage<T,Order,UpLo>::PackedStorage(const PackedStorage<T,Order,UpLo> &r)
    : _dim(r._dim), _indexBase(r._indexBase), _data(r._data)
{
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
PackedStorage<T, Order, UpLo>::~PackedStorage()
{
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
PackedStorage<T,Order,UpLo> &
PackedStorage<T, Order, UpLo>::operator=(const PackedStorage<T,Order,UpLo> &rhs)
{
    if (this!=&rhs) {
        _dim = rhs._dim;
        _indexBase = rhs._indexBase;
        _data = rhs._data;
    }
    return *this;
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
const T &
PackedStorage<T, Order, UpLo>::operator()(int row, int col) const
{
    if (UpLo==Lower) {
        assert(row>=col);
    } else {
        assert(col>=row);
    }

    const int i = row - _indexBase;
    const int j = col - _indexBase;
    const int n = _dim;

    if ((Order==RowMajor) && (UpLo==Upper)) {
        return _data(j+i*(2*n-i-1)/2);
    }
    if ((Order==RowMajor) && (UpLo==Lower)) {
        return _data(j+i*(i+1)/2);
    }
    if ((Order==ColMajor) && (UpLo==Upper)) {
        return _data(i+j*(j+1)/2);
    }
    return _data(i+j*(2*n-j-1)/2);
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
T &
PackedStorage<T, Order, UpLo>::operator()(int row, int col)
{
    if (UpLo==Lower) {
        assert(row>=col);
    } else {
        assert(col>=row);
    }

    const int i = row - _indexBase;
    const int j = col - _indexBase;
    const int n = _dim;

    if ((Order==RowMajor) && (UpLo==Upper)) {
        return _data(j+i*(2*n-i-1)/2);
    }
    if ((Order==RowMajor) && (UpLo==Lower)) {
        return _data(j+i*(i+1)/2);
    }
    if ((Order==ColMajor) && (UpLo==Upper)) {
        return _data(i+j*(j+1)/2);
    }
    return _data(i+j*(2*n-j-1)/2);
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
int
PackedStorage<T, Order, UpLo>::firstIndex() const
{
    return _indexBase;
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
int
PackedStorage<T, Order, UpLo>::lastIndex() const
{
    return _indexBase+_dim-1;
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
int
PackedStorage<T, Order, UpLo>::dim() const
{
    return _dim;
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
const T *
PackedStorage<T, Order, UpLo>::data() const
{
    return _data.data();
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
T *
PackedStorage<T, Order, UpLo>::data()
{
    return _data.data();
}

template <typename T, StorageOrder Order, StorageUpLo UpLo>
void
PackedStorage<T, Order, UpLo>::resize(int dim, int indexBase)
{
    _dim = dim;
    _indexBase = indexBase;
    _data.resize(dim*(dim+1)/2, 0);
}

} // namespace flens
