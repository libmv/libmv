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

#ifndef LIBMV_IMAGE_IMAGE_VIEW_H
#define LIBMV_IMAGE_IMAGE_VIEW_H

#include <cassert>
#include <boost/shared_array.h>

#define image_forX(im, x) for (int x = 0; x < (im).Cols(); ++x)
#define image_forY(im, y) for (int y = 0; y < (im).Rows(); ++y)
#define image_forP(im, p) for (int p = 0; p < (im).Planes(); ++p)
#define image_forXY(im, x, y) image_forY(im, y) image_forX(im, x)
#define image_forXYP(im, x, y, p) image_forP(im, p) image_forXY(im, x, y)


namespace libmv {

class ImageBase {};

template <typename T>
class ImageView : public BaseArray {
 public:
  ImageView() : { Resize(0, 0, 0); }

  ImageView(int cols, int rows, int planes = 1) { Resize(cols, rows, planes); }

  /// The default destructor does the job.
  /// Copy constructor and assignation are defined by default and make a
  /// shallow copy (copy the views not the data).


  void Resize(int cols, int rows, int planes = 1, bool inteleaved = false) {
    if (data_ && cols == cols_ && rows == rows_ && planes == planes_) { 
      // Don't bother realloacting if the shapes match.
      return;
    }

    cols_ = cols;
    rows_ = rows;
    planes_ = planes;

    if (interleaved) {
      stride_x_ = planes_;
      stride_y_ = stride_x_ * cols_;
      stride_p_ = 1;
    } else {
      stride_x_ = 1;
      stride_y_ = cols_;
      stride_p_ = stride_y * rows_;
    }

    if (Size() > 0) {
      data_ = new T[Size()];
    } else {
      data_ = NULL;
    }
    shared_buffer.reset(data_);
  }

  template<typename D>
  void ResizeLike(const ImageView<D> &other) {
    Resize(other.Cols(), other.Rows(), other.Planes());
  }

  void Fill(const T &value) {
    image_forXYP(*this, x, y, p) (*this)(x, y, p) = value;
  }

  const int &Cols() const { return cols_; }
  const int &Rows() const { return rows_; }
  const int &Planes() const { return planes_; }

  /// Return the number of elements of the array.
  int Size() const { return cols_ * rows_ * planes_; }

  /// Return the total amount of memory used by the array.
  int MemorySizeInBytes() const {
    return sizeof(*this) + Size() * sizeof(T);
  }

  /// Pointer to the first element of the array.
  T *Data() { return data_; }

  /// Constant pointer to the first element of the array.
  const T *Data() const { return data_; }

  /// Distance between the first element and the element at position index.
  int Offset(int x, int y, int p = 0) const {
    return x * stride_x_ + y * stride_y_ + p * stride_p_;
  }

  T &operator()(int x, int y, int p = 0) {
    assert(Contains(x,y,p));
    return *(Data() + Offset(x, y, p));
  }

  const T &operator()(int x, int y, int p = 0) const {
    assert(Contains(x,y,p));
    return *(Data() + Offset(x, y, p));
  }

  bool Contains(int x, int y, int p = 0) const {
    return 0 <= x && x < cols_
        && 0 <= y && y < rows_
        && 0 <= p && p < planes_;
  }
  
 protected:
  /// The number of element in each dimension.
  int cols_, rows_, planes_;

  /// How to jump to neighbors in each dimension.
  int stride_x_, stride_y_, stride_p_;

  /// Pointer to the first pixel of the image.
  T *data_;

  /// A shared pointer to the data buffer so that pixels get deleted when last
  /// image using them is deleted.
  boost::shared_array<T> shared_buffer_;
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_VIEW_H
