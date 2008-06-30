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

#include <flens/range.h>

namespace flens {

Range::Range()
    : _firstIndex(1), _stride(1), _lastIndex(0), _length(0)
{
}

Range::Range(int firstIndex, int lastIndex)
    : _firstIndex(firstIndex), _stride(1),
      _lastIndex(lastIndex), _length(lastIndex-firstIndex+1)
{
    assert(_firstIndex<=_lastIndex);
}

Range::Range(int firstIndex, int stride, int lastIndex)
    : _firstIndex(firstIndex), _stride(stride),
      _lastIndex(lastIndex), _length(_lastIndex-_firstIndex+1)
{
    assert(_firstIndex<=lastIndex);
    assert(_stride>0);
}

Range
Range::operator()(int firstIndex, int lastIndex) const
{
    return Range(firstIndex, lastIndex);
}

Range
Range::operator()(int firstIndex, int stride, int lastIndex) const
{
    return Range(firstIndex, stride, lastIndex);
}

Range &
Range::operator+=(int shift)
{
	_firstIndex += shift;
	_lastIndex += shift;
	return *this;
}

int
Range::firstIndex() const
{
    return _firstIndex;
}

int
Range::stride() const
{
    return _stride;
}

int
Range::lastIndex() const
{
    return _lastIndex;
}

int
Range::length() const
{
    return _length;
}

int
Range::numTicks() const
{
    return (_lastIndex-_firstIndex)/_stride+1;
}

//------------------------------------------------------------------------------

std::ostream &
operator<<(std::ostream &out, const Range &rhs)
{
      out << "["<< rhs.firstIndex() << "," << rhs.lastIndex() << "]";
      return out;
}

bool
operator==(const Range &lhs, const Range &rhs)
{
    return (lhs.firstIndex()==rhs.firstIndex()) 
        && (lhs.lastIndex()==rhs.lastIndex());
}

Range
intersect(const Range &a, const Range &b)
{
	return Range(std::max(a.firstIndex(), b.firstIndex()),
                 std::min(a.lastIndex(),  b.lastIndex()));
}

} // namespace flens
