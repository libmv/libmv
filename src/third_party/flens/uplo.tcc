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

template <typename I>
typename Matrix<I>::Impl::ConstTriangularView
upper(const Matrix<I> &A)
{
    return A.impl().upper();
}

template <typename I>
typename Matrix<I>::Impl::TriangularView
upper(Matrix<I> &A)
{
    return A.impl().upper();
}

template <typename I>
typename Matrix<I>::Impl::ConstTriangularView
upperUnit(const Matrix<I> &A)
{
    return A.impl().upperUnit();
}

template <typename I>
typename Matrix<I>::Impl::TriangularView
upperUnit(Matrix<I> &A)
{
    return A.impl().upperUnit();
}

template <typename I>
typename Matrix<I>::Impl::ConstTriangularView
lower(const Matrix<I> &A)
{
    return A.impl().lower();
}

template <typename I>
typename Matrix<I>::Impl::TriangularView
lower(Matrix<I> &A)
{
    return A.impl().lower();
}

template <typename I>
typename Matrix<I>::Impl::ConstTriangularView
lowerUnit(const Matrix<I> &A)
{
    return A.impl().lowerUnit();
}

template <typename I>
typename Matrix<I>::Impl::TriangularView
lowerUnit(Matrix<I> &A)
{
    return A.impl().lowerUnit();
}

} // namespace flens
