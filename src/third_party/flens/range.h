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

#ifndef FLENS_RANGE_H
#define FLENS_RANGE_H 1

#include <iostream>

namespace flens {

template <typename T>
    class DenseVector;

class Range
{
    public:
        Range();

        Range(int firstIndex, int lastIndex);

        Range(int firstIndex, int stride, int lastIndex);

        Range
        operator()(int firstIndex, int lastIndex) const;

        Range
        operator()(int firstIndex, int stride, int lastIndex) const;

        // THINKABOUT: linspace and ranges with arbitrary stride like _(.0,.1,1.)
//        template <typename T>
//            operator const DenseVector<T>() const;
		
		Range &
		operator+=(int shift);

        int
        firstIndex() const;

        int
        stride() const;

        int
        lastIndex() const;

        int
        length() const;

        int
        numTicks() const;

    private:
        int _firstIndex, _stride, _lastIndex, _length;
};

std::ostream &
operator<<(std::ostream &out, const Range &rhs);

bool
operator==(const Range &lhs, const Range &rhs);

Range
intersect(const Range &a, const Range &b);

} // namespace flens

#include <flens/range.tcc>

#endif // FLENS_RANGE_H
