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

#ifndef FLENS_MATVECCLOSURES_H
#define FLENS_MATVECCLOSURES_H 1

#include <flens/macros.h>
#include <flens/matvec.h>
#include <flens/traits.h>
#include <flens/storage.h>

namespace flens {

// == Operation types ==========================================================
struct OpAdd {};              // x+y, A+B
struct OpSub {};              // x-y, A-B
struct OpTrans {};            // A^T
struct OpConjTrans {};        // A^H
struct OpMult {};             // alpha*x, alpha*A, x^T*y, A*x, A*B

//== Wrapper for scalar values =================================================
template <typename T>
class Scalar
{
    public:
        typedef T ElementType;

        Scalar(T value);

        operator T() const;
    private:
        T _value;
};

template <typename T>
struct Ref<Scalar<T> >
{
    typedef typename Ref<T>::Type Type;
};

//== VectorClosure =============================================================
template <typename Op, typename L, typename R>
class VectorClosure
    : public Vector<VectorClosure<Op, L, R> >
{
    public:
        VectorClosure(typename Ref<L>::Type l,
                      typename Ref<R>::Type r);

        typename Ref<L>::Type
        left() const;

        typename Ref<R>::Type
        right() const;

    private:
        typename Ref<L>::Type _left;
        typename Ref<R>::Type _right;
};

template <typename Op, typename L, typename R>
struct TypeInfo<VectorClosure<Op, L, R> >
{
    typedef VectorClosure<Op, L, R> Impl;
    typedef typename Promotion<typename L::ElementType,
                               typename R::ElementType>::Type ElementType;
};

//== MatrixClosure =============================================================
template <typename Op, typename L, typename R>
class MatrixClosure
    : public Matrix<MatrixClosure<Op, L, R> >
{
    public:
        MatrixClosure(typename Ref<L>::Type l,
                      typename Ref<R>::Type r);

        typename Ref<L>::Type
        left() const;

        typename Ref<R>::Type
        right() const;

    private:
        typename Ref<L>::Type _left;
        typename Ref<R>::Type _right;
};

template <typename Op, typename L, typename R>
struct TypeInfo<MatrixClosure<Op, L, R> >
{
    typedef MatrixClosure<Op, L, R> Impl;
    typedef typename Promotion<typename L::ElementType,
                               typename R::ElementType>::Type ElementType;
};

//== DebugClosure ==============================================================

struct DebugClosure
{
    template <typename A>
    static bool
    search(const A &any, const void *addr)
    {
        return ADDRESS(any)==addr;
    }

    template <typename Op, typename L, typename R>
    static bool
    search(const VectorClosure<Op, L, R> &closure, const void *addr)
    {
        return search(closure.left(), addr) || search(closure.right(), addr);
    }

    template <typename Op, typename L, typename R>
    static bool
    search(const MatrixClosure<Op, L, R> &closure, const void *addr)
    {
        return search(closure.left(), addr) || search(closure.right(), addr);
    }
};

} // namespace flens

#include <flens/matvecclosures.tcc>

#endif // FLENS_MATVECCLOSURES_H
