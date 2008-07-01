// Copyright (c) 2007, 2008 libmv authors.
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

#ifndef LIBMV_IMAGE_TUPLE_H
#define LIBMV_IMAGE_TUPLE_H

#include <algorithm>

namespace libmv {

/// A vector of elements with fixed lenght and deep copy semantics.
template <typename T,int N>
class Tuple {
 public:
  /// Custruct a tuple with unitialized data.
  Tuple() {}

  /// Custruct and initialize the tuple to a constant value.
  Tuple(T initial_value) { Reset(initial_value); }
  
  /// Custruct and initialize the tuple from an array of values.
  template <typename D>
  Tuple(D *values) { Reset(values); }

  /// Copy constructor.  Does a deep copy.
  template <typename D>
  Tuple(const Tuple<D,N> &b) { Reset(b); }

  /// Assign operator.  Does a deep copy.
  template <typename D> 
  Tuple& operator=(const Tuple<D,N>& b) {
    Reset(b);
    return *this;
  }

  /// Sets the tuple values from another tuple.
  template <typename D> 
  void Reset(const Tuple<D,N>& b) { Reset(b.Data()); }

  /// Sets the tuple values from a pointer.
  template <typename D> 
  void Reset(D *values) {
    for(int i=0;i<N;i++) {
      data_[i] = T(values[i]);
    }
  }

  /// Sets the tuple values to a constant value.
  void Reset(T value) {
    for(int i=0;i<N;i++) {
      data_[i] = value;
    }
  }
  
  /// Pointer to the first element of the tuple.
  T *Data() { return &data_[0]; }

  /// Constant pointer to the first element of the tuple.
  const T *Data() const { return &data_[0]; }

  /// Access the i's element of the tuple.
  T &operator()(int i) { return data_[i]; }

  /// Access the i's element of the tuple as a constant value.
  const T &operator()(int i) const { return data_[i]; }

 private:
  /// The tuple of elements.
  T data_[N];
};

}  // namespace libmv


#endif  // LIBMV_IMAGE_TUPLE_H
