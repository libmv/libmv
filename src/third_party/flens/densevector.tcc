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

#include <flens/blas_flens.h>
#include <flens/evalclosure.h>
#include <flens/underscore.h>

namespace flens {

// == DenseVector ==============================================================

template <typename A>
DenseVector<A>::DenseVector()
{
}

template <typename A>
DenseVector<A>::DenseVector(const A &engine)
    : _engine(engine)
{
}

template <typename A>
DenseVector<A>::DenseVector(int length, int firstIndex)
    : _engine(length, firstIndex)
{
}

template <typename A>
DenseVector<A>::DenseVector(const Range r)
    : _engine(r.length(), r.firstIndex())
{
    assert(r.stride()==1);
}

template <typename A>
DenseVector<A>::DenseVector(const DenseVector<A> &rhs)
    : Vector<DenseVector<A> >(), _engine(rhs._engine)
{
}

template <typename A>
template <typename E>
DenseVector<A>::DenseVector(const DenseVector<E> &rhs)
    : _engine(rhs.engine())
{
}

template <typename A>
template <typename E>
DenseVector<A>::DenseVector(const Vector<E> &rhs)
{
    copy(rhs.impl(), *this);
}

// -- operators --------------------------------------------------------
template <typename A>
ListInitializerSwitch<DenseVector<A> >
DenseVector<A>::operator=(const typename DenseVector<A>::T &value)
{
    return ListInitializerSwitch<DenseVector<A> >(_engine.data(),
                                                  _engine.stride(),
                                                  _engine.length(),
                                                  value);
}

template <typename A>
DenseVector<A> &
DenseVector<A>::operator=(const DenseVector<A> &rhs)
{
    copy(rhs, *this);
    return *this;
}

template <typename A>
    template <typename RHS>
DenseVector<A> &
DenseVector<A>::operator=(const Vector<RHS> &rhs)
{
    copy(rhs.impl(), *this);
    return *this;
}

template <typename A>
DenseVector<A> &
DenseVector<A>::operator=(const Range &r)
{
    this->resize(r);
    int index = firstIndex();
    for (int i=0; i<length(); ++i) {
        this->operator()(index++) = r.firstIndex()+i*r.stride();
    }
    return *this;
}

template <typename A>
template <typename RHS>
DenseVector<A> &
DenseVector<A>::operator+=(const Vector<RHS> &rhs)
{
    axpy(T(1), rhs.impl(), *this);
    return *this;
}

template <typename A>
    template <typename RHS>
DenseVector<A> &
DenseVector<A>::operator-=(const Vector<RHS> &rhs)
{
    axpy(T(-1), rhs.impl(), *this);
    return *this;
}

template <typename A>
DenseVector<A> &
DenseVector<A>::operator+=(T c)
{
    for (int i=firstIndex(); i<=lastIndex(); ++i) {
        this->operator()(i) += c;
    }
    return *this;
}

template <typename A>
DenseVector<A> &
DenseVector<A>::operator-=(T c)
{
    for (int i=firstIndex(); i<=lastIndex(); ++i) {
        this->operator()(i) -= c;
    }
    return *this;
}

template <typename A>
DenseVector<A> &
DenseVector<A>::operator*=(T alpha)
{
    scal(alpha, *this);
    return *this;
}

template <typename A>
DenseVector<A> &
DenseVector<A>::operator/=(T alpha)
{
    scal(T(1)/alpha, *this);
    return *this;
}

template <typename A>
const typename DenseVector<A>::T &
DenseVector<A>::operator()(int index) const
{
    return _engine(index);
}

template <typename A>
typename DenseVector<A>::T &
DenseVector<A>::operator()(int index)
{
    return _engine(index);
}

template <typename A>
typename DenseVector<A>::ConstView
DenseVector<A>::operator()(const Range &r) const
{
    assert(r.firstIndex()>=this->firstIndex());
    assert(r.lastIndex()<=this->lastIndex());
    return _engine.view(r.firstIndex(), r.lastIndex(), r.stride(),
                        this->firstIndex());
}

template <typename A>
typename DenseVector<A>::ConstView
DenseVector<A>::operator()(const Range &r, int startIndex) const
{
    assert(r.firstIndex()>=this->firstIndex());
    assert(r.lastIndex()<=this->lastIndex());
    return _engine.view(r.firstIndex(), r.lastIndex(), r.stride(),
                        startIndex);
}

template <typename A>
typename DenseVector<A>::View
DenseVector<A>::operator()(const Range &r)
{
    assert(r.firstIndex()>=this->firstIndex());
    assert(r.lastIndex()<=this->lastIndex());
    return _engine.view(r.firstIndex(), r.lastIndex(), r.stride(),
                        this->firstIndex());
}

template <typename A>
typename DenseVector<A>::View
DenseVector<A>::operator()(const Range &r, int startIndex)
{
    assert(r.firstIndex()>=this->firstIndex());
    assert(r.lastIndex()<=this->lastIndex());
    return _engine.view(r.firstIndex(), r.lastIndex(), r.stride(),
                        startIndex);
}

template <typename A>
typename DenseVector<A>::ConstView
DenseVector<A>::operator()(int firstIndex, int stride, int lastIndex) const
{
    assert(firstIndex>=this->firstIndex());
    assert(lastIndex<=this->lastIndex());
    return _engine.view(firstIndex, lastIndex, stride,
                        this->firstIndex());
}

template <typename A>
typename DenseVector<A>::ConstView
DenseVector<A>::operator()(int firstIndex, int stride, int lastIndex, 
                           int startIndex) const
{
    assert(firstIndex>=this->firstIndex());
    assert(lastIndex<=this->lastIndex());
    return _engine.view(firstIndex, lastIndex, stride,
                        startIndex);
}

template <typename A>
typename DenseVector<A>::View
DenseVector<A>::operator()(int firstIndex, int stride, int lastIndex)
{
    assert(firstIndex>=this->firstIndex());
    assert(lastIndex<=this->lastIndex());
    return _engine.view(firstIndex, lastIndex, stride,
                        this->firstIndex());
}

template <typename A>
typename DenseVector<A>::View
DenseVector<A>::operator()(int firstIndex, int stride, int lastIndex,
                           int startIndex)
{
    assert(firstIndex>=this->firstIndex());
    assert(lastIndex<=this->lastIndex());
    return _engine.view(firstIndex, lastIndex, stride,
                        startIndex);
}

template <typename A>
int
DenseVector<A>::firstIndex() const
{
    return _engine.firstIndex();
}

template <typename A>
int
DenseVector<A>::lastIndex() const
{
    return _engine.lastIndex();
}

template <typename A>
int
DenseVector<A>::length() const
{
    return _engine.length();
}

template <typename A>
int
DenseVector<A>::stride() const
{
    return _engine.stride();
}

template <typename A>
Range
DenseVector<A>::range() const
{
    return _(firstIndex(), lastIndex());
}

template <typename A>
const typename DenseVector<A>::T *
DenseVector<A>::data() const
{
    return _engine.data();
}

template <typename A>
typename DenseVector<A>::T *
DenseVector<A>::data()
{
    return _engine.data();
}

template <typename A>
const typename DenseVector<A>::T *
DenseVector<A>::begin() const
{
    return _engine.data();
}

template <typename A>
typename DenseVector<A>::T *
DenseVector<A>::begin()
{
    return _engine.data();
}

template <typename A>
const typename DenseVector<A>::T *
DenseVector<A>::end() const
{
    return &(this->operator()(this->lastIndex()))+1;
}

template <typename A>
typename DenseVector<A>::T *
DenseVector<A>::end()
{
    return &(this->operator()(this->lastIndex()))+1;
}

template <typename A>
const A &
DenseVector<A>::engine() const
{
    return _engine;
}

template <typename A>
A &
DenseVector<A>::engine()
{
    return _engine;
}

template <typename A>
void
DenseVector<A>::resize(int length)
{
    _engine.resize(length, _engine.firstIndex());
}

template <typename A>
void
DenseVector<A>::resize(int length, int firstIndex)
{
    _engine.resize(length, firstIndex);
}

template <typename A>
void
DenseVector<A>::resizeOrClear(const Range &r)
{
    _engine.resizeOrClear(r.length(), r.firstIndex());
}

template <typename A>
void
DenseVector<A>::resizeOrClear(int length, int firstIndex)
{
    _engine.resizeOrClear(length, firstIndex);
}

template <typename A>
void
DenseVector<A>::resize(const Range &r)
{
    _engine.resize(r.length(), r.firstIndex());
}

template <typename A>
void
DenseVector<A>::shiftIndexTo(int firstIndex)
{
        _engine.shiftIndexTo(firstIndex);
}

template <typename A>
void
DenseVector<A>::shiftIndex(int amount)
{
        _engine.shiftIndexTo(firstIndex()+amount);
}

} // namespace flens
