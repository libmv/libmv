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

#ifndef FLENS_MATVEC_H
#define FLENS_MATVEC_H 1

#include <flens/traits.h>

namespace flens {

// == TypeInfo Trait ===========================================================

template <typename A>
struct TypeInfo
{
    typedef A Impl;
    typedef typename Impl::ElementType ElementType;
};

#define DefaultTypeInfo(Type, T) class Type; \
                                 \
                                 template <> \
                                 struct TypeInfo<Type> \
                                 { \
                                     typedef Type Impl; \
                                     typedef T ElementType; \
                                 };

// == Vector ===================================================================

template <typename I>
class Vector
{
    public:
        typedef typename TypeInfo<I>::ElementType     ElementType;
        typedef const typename Ref<ElementType>::Type ConstElementTypeRef;
        typedef typename Ref<ElementType>::Type       ElementTypeRef;
        typedef typename TypeInfo<I>::Impl            Impl;

        virtual
        ~Vector();

        const Impl &
        impl() const;

        Impl &
        impl();
};

template <typename I>
struct TypeInfo<Vector<I> >
{
    typedef typename TypeInfo<I>::ElementType ElementType;
    typedef typename TypeInfo<I>::Impl        Impl;
};

// == Matrix ===================================================================

template <typename I>
class Matrix
{
    public:
        typedef typename TypeInfo<I>::ElementType     ElementType;
        typedef const typename Ref<ElementType>::Type ConstElementTypeRef;
        typedef typename Ref<ElementType>::Type       ElementTypeRef;
        typedef typename TypeInfo<I>::Impl            Impl;

        virtual
        ~Matrix();

        const Impl &
        impl() const;

        Impl &
        impl();
};

template <typename I>
struct TypeInfo<Matrix<I> >
{
    typedef typename TypeInfo<I>::ElementType ElementType;
    typedef typename TypeInfo<I>::Impl        Impl;
};

// == GeneralMatrix ============================================================

template <typename I>
class GeneralMatrix
    : public Matrix<GeneralMatrix<I> >
{
};

template <typename I>
struct TypeInfo<GeneralMatrix<I> >
{
    typedef typename TypeInfo<I>::Impl Impl;
    typedef typename TypeInfo<I>::ElementType ElementType;
};

// == TriangularMatrix =========================================================

template <typename I>
class TriangularMatrix
    : public Matrix<TriangularMatrix<I> >
{
};

template <typename I>
struct TypeInfo<TriangularMatrix<I> >
{
    typedef typename TypeInfo<I>::Impl Impl;
    typedef typename TypeInfo<I>::ElementType ElementType;
};

// == SymmetricMatrix ==========================================================

template <typename I>
class SymmetricMatrix
    : public Matrix<SymmetricMatrix<I> >
{
};

template <typename I>
struct TypeInfo<SymmetricMatrix<I> >
{
    typedef typename TypeInfo<I>::Impl Impl;
    typedef typename TypeInfo<I>::ElementType ElementType;
};

} // namespace flens

#include <flens/matvec.tcc>

#endif // FLENS_MATVEC_H
