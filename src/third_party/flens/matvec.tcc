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

// == Vector ===================================================================

template <typename Impl>
Vector<Impl>::~Vector()
{
}

template <typename Impl>
const typename TypeInfo<Impl>::Impl &
Vector<Impl>::impl() const
{
    typedef const typename TypeInfo<Impl>::Impl &TI;
    return static_cast<TI>(static_cast<const Impl &>(*this));
}

template <typename Impl>
typename TypeInfo<Impl>::Impl &
Vector<Impl>::impl()
{
    typedef typename TypeInfo<Impl>::Impl &TI;
    //return static_cast<TI>(static_cast<Impl &>(*this));
    return static_cast<TI>(*this);
}

// == Matrix ===================================================================

template <typename Impl>
Matrix<Impl>::~Matrix()
{
}

template <typename Impl>
const typename TypeInfo<Impl>::Impl &
Matrix<Impl>::impl() const
{
    typedef const typename TypeInfo<Impl>::Impl &TI;
    //return static_cast<TI>(static_cast<const Impl &>(*this));
    return static_cast<TI>(*this);
}

template <typename Impl>
typename TypeInfo<Impl>::Impl &
Matrix<Impl>::impl()
{
    typedef typename TypeInfo<Impl>::Impl &TI;
    return static_cast<TI>(static_cast<Impl &>(*this));
}

} // namespace flens
