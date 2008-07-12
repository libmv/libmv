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

#include <cassert>
#include <flens/blas.h>
#include <flens/refcounter.h>
#include <flens/hacksforgmpxx.h>

namespace flens {

//== ConstArrayView ============================================================

template <typename T>
ConstArrayView<T>::ConstArrayView(const void *storage,
                                  const T *data,
                                  int length,
                                  int stride,
                                  int firstIndex)
    : _storage(storage),
      _data(data),
      _length(length),
      _stride(stride),
      _firstIndex(firstIndex)
{
    RefCounter::attach(_storage);
}

template <typename T>
ConstArrayView<T>::ConstArrayView(const NoView &rhs)
    : _storage(rhs.data()),
      _data(rhs.data()-rhs.firstIndex()),
      _length(rhs.length()),
      _stride(rhs.stride()),
      _firstIndex(rhs.firstIndex())
{
    RefCounter::attach(_storage);
}

template <typename T>
ConstArrayView<T>::ConstArrayView(const ConstArrayView &rhs)
    : _storage(rhs._storage),
      _data(rhs._data),
      _length(rhs._length),
      _stride(rhs._stride),
      _firstIndex(rhs._firstIndex)
{
    RefCounter::attach(_storage);
}

template <typename T>
ConstArrayView<T>::ConstArrayView(const ArrayView<T> &rhs)
    : _storage(rhs.storage()),
      _data(rhs.data()-rhs.firstIndex()),
      _length(rhs.length()),
      _stride(rhs.stride()),
      _firstIndex(rhs.firstIndex())
{
    RefCounter::attach(_storage);
}

template <typename T>
ConstArrayView<T>::~ConstArrayView()
{
    if (RefCounter::detach(_storage)) {
        assert(0);  // Const view can not free memory
    }
}

template <typename T>
const T &
ConstArrayView<T>::operator()(int index) const
{
    assert(index>=_firstIndex);
    assert(index<_firstIndex+_length);

    return _data[_firstIndex + _stride*(index-_firstIndex)];
}

template <typename T>
int
ConstArrayView<T>::firstIndex() const
{
    return _firstIndex;
}

template <typename T>
int
ConstArrayView<T>::lastIndex() const
{
    return _firstIndex+_length-1;
}

template <typename T>
int
ConstArrayView<T>::length() const
{
    return _length;
}

template <typename T>
int
ConstArrayView<T>::stride() const
{
    return _stride;
}

template <typename T>
const T *
ConstArrayView<T>::data() const
{
    return &_data[_firstIndex];
}

template <typename T>
const void *
ConstArrayView<T>::storage() const
{
    return _storage;
}

template <typename T>
ConstArrayView<T>
ConstArrayView<T>::view(int from, int to, int stride, int firstViewIndex) const
{
    assert(from<=to);
    assert(stride>=1);
    return ConstArrayView<T>(_storage,                             // storage
                             &(this->operator()(from))-_firstIndex,// data
                             (to-from)/stride+1,                   // length
                             stride*_stride,                       // stride
                             firstViewIndex);                     // firstIndex
}

//== ArrayView =================================================================

template <typename T>
ArrayView<T>::ArrayView(void *storage,
                        T *data,
                        int length,
                        int stride,
                        int firstIndex)
    : _storage(storage),
      _data(data),
      _length(length),
      _stride(stride),
      _firstIndex(firstIndex)
{
    RefCounter::attach(_storage);
}

template <typename T>
ArrayView<T>::ArrayView(const ArrayView &rhs)
    : _storage(rhs._storage),
      _data(rhs._data),
      _length(rhs._length),
      _stride(rhs._stride),
      _firstIndex(rhs._firstIndex)
{
    RefCounter::attach(_storage);
}

template <typename T>
ArrayView<T>::~ArrayView()
{
    if (RefCounter::detach(_storage)) {
        free(_storage);
    }
}

template <typename T>
ArrayView<T> &
ArrayView<T>::operator=(const Array<T> &rhs)
{
    assert(length()==rhs.length());
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
    return *this;
}

template <typename T>
ArrayView<T> &
ArrayView<T>::operator=(const ArrayView<T> &rhs)
{
    if (this!=&rhs) {
        assert(length()==rhs.length());
        copy(length(), rhs.data(), rhs.stride(), data(), stride());
    }
    return *this;
}

template <typename T>
ArrayView<T> &
ArrayView<T>::operator=(const ConstArrayView<T> &rhs)
{
    assert(length()==rhs.length());
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
    return *this;
}

template <typename T>
const T &
ArrayView<T>::operator()(int index) const
{
    assert(index>=_firstIndex);
    assert(index<_firstIndex+_length);

    return _data[_firstIndex + _stride*(index-_firstIndex)];
}

template <typename T>
T &
ArrayView<T>::operator()(int index)
{
    assert(index>=_firstIndex);
    assert(index<_firstIndex+_length);

    return _data[_firstIndex + _stride*(index-_firstIndex)];
}

template <typename T>
int
ArrayView<T>::firstIndex() const
{
    return _firstIndex;
}

template <typename T>
int
ArrayView<T>::lastIndex() const
{
    return _firstIndex+_length-1;
}

template <typename T>
int
ArrayView<T>::length() const
{
    return _length;
}

template <typename T>
int
ArrayView<T>::stride() const
{
    return _stride;
}

template <typename T>
const T *
ArrayView<T>::data() const
{
    return &_data[_firstIndex];
}

template <typename T>
T *
ArrayView<T>::data()
{
    return &_data[_firstIndex];
}

template <typename T>
const void *
ArrayView<T>::storage() const
{
    return _storage;
}

template <typename T>
void
ArrayView<T>::resize(int length, int firstIndex)
{
    assert((length==_length) && (firstIndex==_firstIndex));
}

template <typename T>
void
ArrayView<T>::resizeOrClear(int length, int firstIndex)
{
    assert((length==_length) && (firstIndex==_firstIndex));
    std::fill_n(this->data(), _length, T(0));
}

template <typename T>
ConstArrayView<T>
ArrayView<T>::view(int from, int to, int stride, int firstViewIndex) const
{
    assert(from<=to);
    assert(stride>=1);
    return ConstArrayView<T>(_storage,                             // storage
                             &(this->operator()(from))-_firstIndex,// data
                             (to-from)/stride+1,                   // length
                             stride*_stride,                       // stride
                             firstViewIndex);
}

template <typename T>
ArrayView<T>
ArrayView<T>::view(int from, int to, int stride, int firstViewIndex)
{
    assert(from<=to);
    assert(stride>=1);
    return ArrayView<T>(_storage,                              // storage
                        &(this->operator()(from))-_firstIndex, // data
                        (to-from)/stride+1,                    // length
                        stride*_stride,                        // stride
                        firstViewIndex);
}

template <typename T>
void
ArrayView<T>::shiftIndexTo(int firstIndex)
{
    assert(_firstIndex==firstIndex);
}


//== Array =====================================================================

template <typename T>
Array<T>::Array()
    : _length(0), _firstIndex(1), _data(0)
{
}

template <typename T>
Array<T>::Array(int length, int firstIndex)
    : _length(length), _firstIndex(firstIndex), _data(0)
{
    _allocate();
}

template <typename T>
Array<T>::Array(const Array<T> &rhs)
    : _length(rhs.length()), _firstIndex(rhs.firstIndex()), _data(0)
{
    _allocate();
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
}

template <typename T>
Array<T>::Array(const ArrayView<T> &rhs)
    : _length(rhs.length()), _firstIndex(rhs.firstIndex()), _data(0)
{
    _allocate();
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
}

template <typename T>
Array<T>::Array(const ConstArrayView<T> &rhs)
    : _length(rhs.length()), _firstIndex(rhs.firstIndex()), _data(0)
{
    _allocate();
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
}

template <typename T>
Array<T>::~Array()
{
    _release();
}

template <typename T>
Array<T> &
Array<T>::operator=(const Array<T> &rhs)
{
    if (this!=&rhs) {
        if (length()!=rhs.length()) {
            resize(rhs.length(), rhs.firstIndex());
        }
        copy(length(), rhs.data(), rhs.stride(), data(), stride());
    }
    return *this;
}

template <typename T>
Array<T> &
Array<T>::operator=(const ArrayView<T> &rhs)
{
    if (length()!=rhs.length()) {
        resize(rhs.length(), rhs.firstIndex());
    }
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
    return *this;
}

template <typename T>
Array<T> &
Array<T>::operator=(const ConstArrayView<T> &rhs)
{
    if (length()!=rhs.length()) {
        resize(rhs.length(), rhs.firstIndex());
    }
    copy(length(), rhs.data(), rhs.stride(), data(), stride());
    return *this;
}

template <typename T>
ListInitializer<Array<T> >
Array<T>::operator=(const T &value)
{
    return ListInitializer<Array<T> >(data(), length(), value);
}

template <typename T>
const T &
Array<T>::operator()(int index) const
{
    assert(index>=_firstIndex);
    assert(index<_firstIndex+_length);

    return _data[index];
}

template <typename T>
T &
Array<T>::operator()(int index)
{
    assert(index>=_firstIndex);
    assert(index<_firstIndex+_length);

    return _data[index];
}

template <typename T>
int
Array<T>::firstIndex() const
{
    return _firstIndex;
}

template <typename T>
int
Array<T>::lastIndex() const
{
    return _firstIndex+_length-1;
}

template <typename T>
int
Array<T>::length() const
{
    return _length;
}

template <typename T>
int
Array<T>::stride() const
{
    return 1;
}

template <typename T>
const T *
Array<T>::data() const
{
    return &_data[_firstIndex];
}

template <typename T>
T *
Array<T>::data()
{
    return &_data[_firstIndex];
}

template <typename T>
void
Array<T>::resize(int length, int firstIndex)
{
    if (length!=_length) {
        _release();
        _length = length;
        _firstIndex = firstIndex;
        _allocate();
    } else {
        _data += _firstIndex - firstIndex;
        _firstIndex = firstIndex;
    }
}

template <typename T>
void
Array<T>::resizeOrClear(int length, int firstIndex)
{
    if (length!=_length) {
        _release();
        _length = length;
        _firstIndex = firstIndex;
        _allocate();
    } else {
        _data += _firstIndex - firstIndex;
        _firstIndex = firstIndex;
        std::fill_n(this->data(), _length, T(0));
    }
}

template <typename T>
ConstArrayView<T>
Array<T>::view(int from, int to, int stride, int firstViewIndex) const
{
    assert(from<=to);
    assert(stride>=1);
    return ConstView(data(),                                    // storage
                     &(this->operator()(from))-firstViewIndex,  // data
                     (to-from)/stride+1,                        // length
                     stride,                                    // stride
                     firstViewIndex);                           // firstIndex
}

template <typename T>
ArrayView<T>
Array<T>::view(int from, int to, int stride, int firstViewIndex)
{
    assert(from<=to);
    assert(stride>=1);
    return View(data(),                                   // storage
                &(this->operator()(from))-firstViewIndex, // data
                (to-from)/stride+1,                       // length
                stride,                                   // stride
                firstViewIndex);                          // firstIndex
}

template <typename T>
void
Array<T>::shiftIndexTo(int firstIndex)
{
    _data += _firstIndex - firstIndex;
    _firstIndex = firstIndex;
}

template <typename T>
void
Array<T>::_allocate()
{
    assert(!_data);
    // TODO: discuss with Michael.
    if (length()<=0) {
        return;
    }

    _data = static_cast<T*>(calloc(_length, sizeof(T)))-_firstIndex;
    Initializer<Array<T> >::initialize(*this);
    assert(_data+_firstIndex);
}

template <typename T>
void
Array<T>::_release()
{
    if (_data) {
         if (RefCounter::detach(data())) {
#ifdef GMP
             T *fp;
             int i;
             for (i=0, fp=data(); i<_length; ++i, ++fp) {
                 (*fp).~T();
             }
#endif
             free(data());
         }
         _data=0;
         _firstIndex = 0;
    }
}

} // namespace flens
