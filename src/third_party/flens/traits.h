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

#ifndef FLENS_TRAITS_H
#define FLENS_TRAITS_H 1

#ifdef GMP
#include <gmp/gmpfrxx.h>
#endif

namespace flens {

//== Auxiliary Traits ==========================================================

template <typename A, typename B>
struct IsSame
{
    static const bool value = false;
};

template <typename A>
struct IsSame<A, A>
{
    static const bool value = true;
};

//== Reference traits ==========================================================

template <typename A>
struct Ref
{
    typedef const A &Type;
};

template <>
struct Ref<void>
{
    typedef void Type;
};

template <>
struct Ref<float>
{
    typedef float Type;
};

template <>
struct Ref<double>
{
    typedef double Type;
};

#ifdef GMP
template <typename T, typename U>
struct Ref<__gmp_expr<T,U> >
{
    typedef __gmp_expr<T,U> Type;
};
#endif

//== Promotion traits ==========================================================

template <typename A, typename B>
struct Promotion
{
};

template <typename A>
struct Promotion<A, A>
{
    typedef A Type;
};

template <>
struct Promotion<int, double>
{
    typedef double Type;
};

template <>
struct Promotion<double, int>
{
    typedef double Type;
};

template <>
struct Promotion<double, float>
{
    typedef double Type;
};

} // namespace flens

#endif // TRAITS_H
