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

#ifndef FLENS_DENSEVECTOR_H
#define FLENS_DENSEVECTOR_H 1

#include <flens/listinitializer.h>
#include <flens/matvec.h>
#include <flens/range.h>
#include <flens/traits.h>

namespace flens {

// == DenseVector ==============================================================

template <typename A>
class DenseVector
    : public Vector<DenseVector<A> >
{
    public:
        typedef typename DenseVector<A>::ElementType   T;

        typedef typename A::ConstView                  ConstAView;
        typedef typename A::View                       AView;
        typedef typename A::NoView                     ANoView;

        typedef DenseVector<ConstAView>                ConstView;
        typedef DenseVector<AView>                     View;
        typedef DenseVector<ANoView>                   NoView;

        DenseVector();

        DenseVector(const A &engine);

        DenseVector(int length, int firstIndex = 1);

        explicit
        DenseVector(const Range r);

        DenseVector(const DenseVector<A> &rhs);

        template <typename E>
            DenseVector(const DenseVector<E> &rhs);

        template <typename E>
            DenseVector(const Vector<E> &rhs);

        // -- operators --------------------------------------------------------

        ListInitializerSwitch<DenseVector<A> >
        operator=(const T &value);

        DenseVector<A> &
        operator=(const DenseVector<A> &rhs);

        template <typename RHS>
            DenseVector<A> &
            operator=(const Vector<RHS> &rhs);

        DenseVector<A> &
        operator=(const Range &r);

        template <typename RHS>
            DenseVector<A> &
            operator+=(const Vector<RHS> &rhs);

        template <typename RHS>
            DenseVector<A> &
            operator-=(const Vector<RHS> &rhs);

        DenseVector<A> &
        operator+=(T alpha);

        DenseVector<A> &
        operator-=(T alpha);

        DenseVector<A> &
        operator*=(T alpha);

        DenseVector<A> &
        operator/=(T alpha);

        const T &
        operator()(int index) const;

        T &
        operator()(int index);

        ConstView
        operator()(const Range &r) const;

        ConstView
        operator()(const Range &r, int startIndex) const;
        
        View
        operator()(const Range &r);

        View
        operator()(const Range &r, int startIndex);
        
        ConstView
        operator()(int firstIndex, int stride, int lastIndex) const;

        ConstView
        operator()(int firstIndex, int stride, int lastIndex, int startIndex) const;
        
        View
        operator()(int firstIndex, int stride, int lastIndex);

        View
        operator()(int firstIndex, int stride, int lastIndex, int startIndex);
        
        // -- methods ----------------------------------------------------------
        int
        firstIndex() const;

        int
        lastIndex() const;

        int
        length() const;

        int
        stride() const;

        Range
        range() const;

        const T *
        data() const;

        T *
        data();

        const T *
        begin() const;

        T *
        begin();

        const T *
        end() const;

        T *
        end();

        const A &
        engine() const;

        A &
        engine();

        void
        resize(int length, int firstIndex=1);

        void
        resize(const Range &r);

        void
        resizeOrClear(const Range &r);

        void
        resizeOrClear(int length, int firstIndex=1);

        void
        shiftIndexTo(int firstIndex);
        
        void
        shiftIndex(int amount);

    private:
        A _engine;
};

template <typename A>
struct TypeInfo<DenseVector<A> >
{
    typedef DenseVector<A> Impl;
    typedef typename A::ElementType ElementType;
};

} // namespace flens

#include <flens/densevector.tcc>

#endif // FLENS_DENSEVECTOR_H
