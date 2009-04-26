// Copyright (c) 2009 libmv authors.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#ifndef LIBMV_OPTIMIZE_JET_H_
#define LIBMV_OPTIMIZE_JET_H_

#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

// TODO(keir): Consider wrapping some of these operations with eigen arrays to
// exploit eigen's vectorization.

namespace libmv {

// Poor man's forward mode automatic differentaition.
template<int N, typename T = double>
struct Jet {
  Jet() {}

  // Constant constructor; for things like 1.0.
  template<typename Tin>
  explicit Jet(Tin x0) {
    x = T(x0);
    for (int i = 0; i < N; ++i) {
      d[i] = T(0.0);
    }
  }

  // Constructor for variables. This only works for first derivatives!
  template<typename Tin>
  Jet(Tin x0, int independent) {
    x = T(x0);
    for (int i = 0; i < N; ++i) {
      d[i] = T(0.0);
    }
    d[independent] = T(1.0);
  }

  // Operators: + * /

  template<typename OtherJet>
  Jet<N, T> operator*(const OtherJet &other) const {
    Jet<N, T> res;
    res.x = x * T(other.x);
    for (int i = 0; i < N; ++i) {
      res.d[i] = other.d[i] * x + d[i] * other.x;
    }
    return res;
  }

  template<typename OtherJet>
  Jet<N, T> operator+(const OtherJet &other) const {
    Jet<N, T> res;
    res.x = x + other.x;
    for (int i = 0; i < N; ++i) {
      res.d[i] = d[i] + other.d[i];
    }
    return res;
  }

  template<typename OtherJet>
  Jet<N, T> operator/(const OtherJet &other) const {
    Jet<N, T> res;
    res.x = x / other.x;
    for (int i = 0; i < N; ++i) {
      res.d[i] = (other.x * d[i] - x * other.d[i]) / (other.x * other.x);
    }
    return res;
  }

  T x;
  T d[N];
};

}  // namespace libmv
#endif  // LIBMV_OPTIMIZE_RESIDUAL_H_
