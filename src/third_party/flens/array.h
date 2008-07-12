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

#ifndef FLENS_ARRAY_H
#define FLENS_ARRAY_H 1

#include <flens/listinitializer.h>

namespace flens {

// forward decalarations
template <typename T>
    class ArrayView;

template <typename T>
    class Array;


template <typename T>
class ConstArrayView
{
    public:
        typedef T               ElementType;
        typedef ConstArrayView  ConstView;
        typedef ArrayView<T>    View;
        typedef Array<T>        NoView;

        ConstArrayView(const void *storage, const T *data,
                       int length, int stride=1, int firstIndex=0);

        ConstArrayView(const NoView &rhs);

        ConstArrayView(const ConstArrayView &rhs);

        ConstArrayView(const View &rhs);

        ~ConstArrayView();

        //-- operators ---------------------------------------------------------

        const T &
        operator()(int index) const;

        //-- methods -----------------------------------------------------------

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

        const void *
        storage() const;

        ConstArrayView<T>
        view(int from, int to, int stride=1, int firstViewIndex=0) const;

    private:
        ConstArrayView<T> &
        operator=(const ConstArrayView<T> &rhs);

        const void *_storage;
        const T *_data;
        int _length, _stride, _firstIndex;
};

template <typename T>
class ArrayView
{
    public:
        typedef T                 ElementType;
        typedef ConstArrayView<T> ConstView;
        typedef ArrayView         View;
        typedef Array<T>          NoView;

        ArrayView(void *storage, T *data,
                  int length, int stride=1, int firstIndex=0);

        ArrayView(const ArrayView &rhs);

        ~ArrayView();

        //-- operators ---------------------------------------------------------

        ArrayView<T> &
        operator=(const ArrayView<T> &rhs);

        ArrayView<T> &
        operator=(const Array<T> &rhs);

        ArrayView<T> &
        operator=(const ConstArrayView<T> &rhs);

        ListInitializer<ArrayView<T> >
        operator=(const T &value);

        const T &
        operator()(int index) const;

        T &
        operator()(int index);

        //-- methods -----------------------------------------------------------

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

        const void *
        storage() const;

        void
        resize(int length, int firstIndex=0);
        
        void
        resizeOrClear(int length, int firstIndex=0);

        ConstArrayView<T>
        view(int from, int to, int stride=1, int firstViewIndex=0) const;

        ArrayView<T>
        view(int from, int to, int stride=1, int firstViewIndex=0);
        
        void
        shiftIndexTo(int firstIndex);

    private:
        void *_storage;
        T *_data;
        int _length, _stride, _firstIndex;
};


template <typename T>
class Array
{
    public:
        typedef T                 ElementType;
        typedef ConstArrayView<T> ConstView;
        typedef ArrayView<T>      View;
        typedef Array             NoView;

        Array();

        Array(int length, int firstIndex=0);

        Array(const Array<T> &rhs);

        Array(const ArrayView<T> &rhs);

        Array(const ConstArrayView<T> &rhs);

        ~Array();

        //-- operators ---------------------------------------------------------

        Array<T> &
        operator=(const Array<T> &rhs);

        Array<T> &
        operator=(const ArrayView<T> &rhs);

        Array<T> &
        operator=(const ConstArrayView<T> &rhs);

        ListInitializer<Array<T> >
        operator=(const T &value);

        const T &
        operator()(int index) const;

        T &
        operator()(int index);

        //-- methods -----------------------------------------------------------

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

        void
        resize(int length, int firstIndex=0);

        void
        resizeOrClear(int length, int firstIndex=0);

        ConstArrayView<T>
        view(int from, int to, int stride=1, int firstViewIndex=0) const;

        ArrayView<T>
        view(int from, int to, int stride=1, int firstViewIndex=0);

        void
        shiftIndexTo(int firstIndex);

    private:
        void
        _allocate();

        void
        _release();

        int _length, _firstIndex;
        T *_data;
};

} // namespace flens

#include <flens/array.tcc>

#endif // FLENS_ARRAY_H
