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

#include <cassert>

#include "libmv/image/tuple.h"

using namespace std;
using libmv::Tuple;

namespace libmv {

/// A multidimensional array class.
template <typename T,int N>
class ArrayND {
 public:
  /// Type for the multidimensional indices.
  typedef Tuple<int,N> Index;

  /// Create an empty array.
  ArrayND() : data_(NULL) {
    Resize(Index(0));
  }

  /// Create an array with the shape.
  ArrayND(const Index &shape) : data_(NULL) {
    Resize(shape);
  }
  
  /// Create an array with the shape.
  ArrayND(int *shape) : data_(NULL) {
    Resize(shape);
  }
  
  /// Copy constructor copies pixel data.
  ArrayND(const ArrayND<T, N> &b) : data_(NULL) {
    ResizeLike(b);
    memcpy(Data(), b.Data(), sizeof(T) * Size());
  }

  /// Crate a 1D array of lenght s0.
  ArrayND(int s0) : data_(NULL) {
    Resize(s0);
  }

  /// Crate a 2D array of shape (s0,s1).
  ArrayND(int s0, int s1) : data_(NULL) {
    Resize(s0,s1);
  }

  /// Crate a 3D array of shape (s0,s1,s2).
  ArrayND(int s0, int s1, int s2) : data_(NULL) {
    Resize(s0,s1,s2);
  }

  /// Destructor deletes pixel data.
  ~ArrayND() {
    delete [] data_;
  }
 
  /// Assignation copies pixel data.
  ArrayND &operator=(const ArrayND<T, N> &b) {
    ResizeLike(b);
    memcpy(Data(), b.Data(), sizeof(T) * Size());
    return *this;
  }

  const Index &Shapes() const {
    return shape_;
  }

  const Index &Strides() const {
    return strides_;
  }

  /// Create an array of shape s.
  void Resize(const Index &shape) {
    Resize(shape.Data());
  }

  template<typename D>
  void ResizeLike(const ArrayND<D,N> &other) {
    // TODO(keir): Don't reallocate memory if sizes don't change.

    shape_.Reset(other.Shapes());
    strides_ = other.Strides();
    delete [] data_;
    if (Size()>0)
      data_ = new T[Size()];
    else
      data_ = NULL;
  }

  /// Resizes the array to shape s.  All data is lost.
  void Resize(const int *shape) {
    // TODO(keir): Don't reallocate memory if sizes don't change.

    shape_.Reset(shape);
    strides_(N-1) = 1;
    for (int i = N - 1; i > 0; --i)
      strides_(i-1) = strides_(i) * shape_(i);

    delete [] data_;
    if (Size()>0)
      data_ = new T[Size()];
    else
      data_ = NULL;
  }


  /// Resize a 1D array to lenght s0.
  void Resize(int s0) {
    assert(N == 1);
    int shape[] = {s0};
    Resize(shape);
  }

  /// Resize a 2D array to shape (s0,s1).
  void Resize(int s0, int s1) {
    assert(N == 2);
    int shape[] = {s0,s1};
    Resize(shape);
  }

  /// Resize a 3D array to shape (s0,s1,s2).
  void Resize(int s0, int s1, int s2) {
    assert(N == 3);
    int shape[] = {s0,s1,s2};
    Resize(shape);
  }

  template<typename D>
  void CopyFrom(const ArrayND<D,N> &other) {
    ResizeLike(other);
    T *data = Data();
    const D *other_data = other.Data();
    for (int i = 0; i < Size(); ++i) {
      data[i] = T(other_data[i]);
    }
  }

  void Fill(T value) {
    for (int i = 0; i < Size(); ++i) {
      Data()[i] = value;
    }
  }

  /// Return the lenght of an axis.
  int Shape(int axis) const {
    return shape_(axis);
  }

  /// Return the distance between neighboring elements along axis.
  int Stride(int axis) const {
    return strides_(axis);
  }

  /// Return the number of elements of the array.
  int Size() const {
    int size = 1;
    for (int i = 0; i < N; ++i)
      size *= Shape(i);
    return size;
  }

  /// Pointer to the first element of the array.
  T *Data() { return data_; }

  /// Constant pointer to the first element of the array.
  const T *Data() const { return data_; }

  /// Distance between the first element and the element at position index.
  int Offset(const Index &index) const {
    int offset = 0;
    for (int i = 0; i < N; ++i)
      offset += index(i) * Stride(i);
    return offset;
  }

  /// 1D specialization.
  int Offset(int i0) const {
    assert(N == 1);
    return i0 * Stride(0);
  }
  
  /// 2D specialization.
  int Offset(int i0, int i1) const {
    assert(N == 2);
    return i0 * Stride(0) + i1 * Stride(1);
  }
  
  /// 3D specialization.
  int Offset(int i0, int i1, int i2) const {
    assert(N == 3);
    return i0 * Stride(0) + i1 * Stride(1) + i2 * Stride(2);
  }
  
  /// Return a reference to the element at position index.
  T &operator()(const Index &index) {
    // TODO(pau) Boundary checking in debug mode.
    return *( Data() + Offset(index) );
  }

  /// 1D specialization.
  T &operator()(int i0) {
    return *( Data() + Offset(i0) );
  }

  /// 2D specialization.
  T &operator()(int i0, int i1) {
    assert(0 <= i0 && i0 < Shape(0));
    assert(0 <= i1 && i1 < Shape(1));
    return *( Data() + Offset(i0,i1) );
  }

  /// 3D specialization.
  T &operator()(int i0, int i1, int i2) {
    return *( Data() + Offset(i0,i1,i2) );
  }

  /// Return a constant reference to the element at position index.
  const T &operator()(const Index &index) const {
    return *( Data() + Offset(index) );
  }

  /// 1D specialization.
  const T &operator()(int i0) const {
    return *( Data() + Offset(i0) );
  }

  /// 2D specialization.
  const T &operator()(int i0, int i1) const {
    assert(0 <= i0 && i0 < Shape(0));
    assert(0 <= i1 && i1 < Shape(1));
    return *( Data() + Offset(i0,i1) );
  }

  /// 3D specialization.
  const T &operator()(int i0, int i1, int i2) const {
    return *( Data() + Offset(i0,i1,i2) );
  }

  /// True if index is inside array.
  bool Contains(const Index &index) const {
    for (int i = 0; i < N; ++i)
      if (index(i) < 0 || index(i) >= Shape(i))
        return false;
    return true;
  }
  
 protected:
  /// The number of element in each dimension.
  Index shape_;

  /// How to jump to neighbors in each dimension.
  Index strides_;

  /// Pointer to the first element of the array.
  T *data_;
};

// TODO(keir): make this work for N != 3.
template <typename TA, typename TB, typename TC>
void MultiplyElements(const ArrayND<TA, 3> a,
                      const ArrayND<TB, 3> b,
                      ArrayND<TC, 3> *c) {
  c->ResizeLike(a);
  assert(a.Shape(0) == b.Shape(0));
  assert(a.Shape(1) == b.Shape(1));
  assert(a.Shape(2) == b.Shape(2));
  for (int i = 0; i < a.Shape(0); ++i) {
    for (int j = 0; j < a.Shape(1); ++j) {
      for (int k = 0; k < a.Shape(2); ++k) {
        (*c)(i, j, k) = TC(a(i, j, k) * b(i, j, k));
      }
    }
  }
}


}  // namespace libmv

#endif  // LIBMV_IMAGE_ARRAY_ND_H
