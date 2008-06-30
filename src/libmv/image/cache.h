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

#ifndef LIBMV_IMAGE_CACHE_H_
#define LIBMV_IMAGE_CACHE_H_

// A generic cache interface.

namespace libmv {

// Cache key / value pairs.  Pinned objects count toward maximum size
// allowance, but do not prevent new pinned objects from being added (even if
// maximum memory is exceeded). The units of size are deliberately unspecified.
template<typename K, typename V>
class Cache {
 public:
  virtual bool FetchAndPin(const K &key, V *value) = 0;
  virtual void StoreAndPin(const K &key, V value) {
    StoreAndPinSized(key, value, 1);
  }
  virtual void StoreAndPinSized(const K &key, V value, const int size) = 0;
  virtual void Unpin(const K &key) = 0;
  virtual void MassUnpin() = 0;
  virtual bool ContainsKey(const K &key) = 0;
  virtual void SetMaxSize(const int size) = 0;
  virtual const int MaxSize() = 0;
  virtual const int Size() = 0;
  virtual ~Cache() {}
};
}  // namespace libmv

#endif  // LIBMV_IMAGE_CACHE_H_
