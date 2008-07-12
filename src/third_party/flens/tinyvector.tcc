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

// == TinyVector ==============================================================

template <typename A>
TinyVector<A>::TinyVector()
{
}

// -- operators ----------------------------------------------------------------

template <typename A>
ListInitializer<A>
TinyVector<A>::operator=(const T &value)
{
    for (int i = 0; i < length() ; i++) {
      (*this)(i) = value;
    }
    return ListInitializer<A>(_engine._data, value);
}

template <typename A>
TinyVector<A> &
TinyVector<A>::operator=(const TinyVector<A> &rhs)
{
    copy(rhs, *this);
    return *this;
}

template <typename A>
template <typename RHS>
TinyVector<A> &
TinyVector<A>::operator=(const Vector<RHS> &rhs)
{
    copy(rhs.impl(), *this);
    return *this;
}

template <typename A>
template <typename RHS>
TinyVector<A> &
TinyVector<A>::operator+=(const Vector<RHS> &rhs)
{
    axpy(T(1), rhs.impl(), *this);
    return *this;
}

template <typename A>
template <typename RHS>
TinyVector<A> &
TinyVector<A>::operator-=(const Vector<RHS> &rhs)
{
    axpy(T(-1), rhs.impl(), *this);
    return *this;
}

template <typename A>
TinyVector<A> &
TinyVector<A>::operator*=(T alpha) {
    for (int i = 0; i < length(); i++) {
        (*this)(i) *= alpha;
    }
    return *this;
}

template <typename A>
TinyVector<A> &
TinyVector<A>::operator/=(T alpha) {
    for (int i = 0; i < length(); i++) {
        (*this)(i) /= alpha;
    }
    return *this;
}

template <typename A>
const typename TinyVector<A>::T &
TinyVector<A>::operator()(int index) const
{
    return _engine(index);
}

template <typename A>
typename TinyVector<A>::T &
TinyVector<A>::operator()(int index)
{
    return _engine(index);
}

//-- methods -----------------------------------------------------------

template <typename A>
void
TinyVector<A>::resize(int length)
{
    assert(length==this->length());
}

template <typename A>
void
TinyVector<A>::resize(int length, int firstIndex)
{
    assert(length==this->length());
    assert(firstIndex==this->firstIndex());
}

template <typename A>
int
TinyVector<A>::length() const
{
    return _engine.length();
}

template <typename A>
int
TinyVector<A>::firstIndex() const
{
    return _engine.firstIndex();
}

template <typename A>
int
TinyVector<A>::lastIndex() const
{
    return _engine.lastIndex();
}

template <typename A>
int
TinyVector<A>::stride() const
{
    return _engine.stride();
}

template <typename A>
const typename TinyVector<A>::T *
TinyVector<A>::data() const
{
    return _engine.data();
}

template <typename A>
typename TinyVector<A>::T *
TinyVector<A>::data()
{
    return _engine.data();
}

template <typename A>
const A &
TinyVector<A>::engine() const
{
    return _engine;
}

template <typename A>
A &
TinyVector<A>::engine()
{
    return _engine;
}

} // namespace flens
