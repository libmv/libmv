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

#ifndef LIBMV_IMAGE_ARRAY_ND_H
#define LIBMV_IMAGE_ARRAY_ND_H

#include <boost/shared_array.hpp>
#include "libmv/image/tuple.h"

using libmv::Tuple;

namespace libmv {

/// A multidimensional array class with shallow copy semantics.
template <typename T,int N>
class ArrayND
{
 public:
  /// Type for the multidimensional indices.
  typedef Tuple<int,N> Index;


  /// Create an empty array.
  ArrayND() {
    Reset(Index(0));
  }

  /// Create an array of shape s.
  ArrayND(const Index &s) {
    Reset(s);
  }
  
  /// Create an array of shape s.
  ArrayND(int *s) {
    Reset(s);
  }
  
  /// Create an array of shape s.
  void Reset(const Index &s) {
    Reset(s.Data());
  }

  /// Resets the array to shape s.  All data is lost.
  void Reset(int *s) {
    shape_.reset(s);
    strides_[N-1] = 1;
    for(int i=N-1; i>0; i--)
      strides_[i-1] = strides_[i] * shape_[i];

    if(Size()>0)
      data_ = new T[Size()];
    else
      data_ = NULL;
    data_buffer_.reset(data_);
  }

  /// Resets an array to have the same shape as another.  All data is lost.
  void ResetLike(const ArrayND &b) {
    reset(b.shape_);
  }

  /// Efficiently swap two arrays.  No data copies are done.
  void Swap( ArrayND &im ) {
    ArrayND tmp = im;
    im = *this;
    *this = tmp;
  }
  
  /// Return a conversion of this array to type D.
  /// Assumes *this is contiguous.
  template<typename D>
  ArrayND<D,N> AsType() const {
    ArrayND<D,N> res(shape_);
    for(int i=0; i<Size(); i++)
      res[i] = D((*this)[i]);
    return res;
  }

  /// Return a deep copy of the array.  Data is copied.
  ArrayND DeepCopy() { return AsType<T>(); }
  
  /// Return the lenght of an axis.
  int Shape(int axis) {
    return shape_[axis];
  }

  /// Return the distance between neighboring elements along axis.
  int Stride(int axis) {
    return strides_[axis];
  }

  /// Return the number of elements of the array.
  int Size() const {
    int s = 1;
    for(int i=0; i<N; i++)
      s *= Shape(i);
    return s;
  }

  /// Pointer to the first element of the array.
  T *Data() { return data_; }

  /// Constant pointer to the first element of the array.
  const T *Data() const { return data_; }

  /// Distance between the first element and the element at position index.
  int Offset(const Index &index) const {
    int o = 0;
    for(int i=0; i<N; i++)
      o += index[i] * Stride(i);
    return o;
  }
  
  /// Return a reference to the element at position index.
  T &operator()(const Index &index) {
    return *( Data() + Offset(index) );
  }

  /// Return a constant reference to the element at position index.
  const T &operator()(const Index &index) const {
    return *( Data() + Offset(index) );
  }
  
  /// Check whether index is inside the array.
  bool contains(const Index &index) const {
    for(int i=0; i<N; i++)
      if(index[i] < 0 || index[i] >= Shape(i))
        return false;
    return true;
  }
  
 private:
  /// The number of element in each dimension.
  Tuple<int,N> shape_;

  /// How to jump to neighbors in each dimension.
  Tuple<int,N> strides_;

  /// Pointer to the first element of the array.
  T *data_;

  /// A shared pointer to the data.
  /// Data will be deleted when last array using it is deleted.
  boost::shared_array<T> data_buffer_;
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_ARRAY_ND_H
