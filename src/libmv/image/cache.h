// Copyright 2008 Keir Mierle, all rights reserved.
//
// A generic cache interface.
//

#ifndef LIBMV_IMAGE_CACHE_H_
#define LIBMV_IMAGE_CACHE_H_

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
