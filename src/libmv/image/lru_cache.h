// Copyright (c) 2007, 2008, 2009 libmv authors.
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
//
// A simple LRU cache. It uses a fair amount of space per-item, so do not use
// this to store many small items. Instead, use this for heavy-weight objects
// like images.

#ifndef LIBMV_IMAGE_LRU_CACHE_H_
#define LIBMV_IMAGE_LRU_CACHE_H_

#include <map>
#include <list>
#include <cassert>
#include <cstdio>

#include "libmv/image/cache.h"

using std::map;
using std::list;

namespace libmv {
namespace lru_cache {

// A queue which stores unique keys. It allows removal of arbitrary items by
// key (which may be anywhere in the queue), and also containment queries.
template<typename K>
class SetQueue {
 public:
  // O(log n)
  void Enqueue(const K &key) {
    assert(!Contains(key));
    queue_.push_front(key);
    queue_positions_[key] = queue_.begin();
  }
  // O(log n)
  void Remove(const K &key) {
    assert(Contains(key));
    typename Map::iterator it = queue_positions_.find(key);
    queue_.erase(it->second);
    queue_positions_.erase(it);
  }
  // O(1)
  void Dequeue(K *key) {
    assert(!queue_.empty());
    *key = queue_.back();
    Remove(*key);
  }
  // O(log n)
  bool Contains(const K &key) {
    return queue_positions_.find(key) != queue_positions_.end();
  }
  // O(n)
  void Clear() {
    queue_positions_.clear();
    queue_.clear();
  }
  // O(1) in most implementations however may be O(n)
  int Size() {
    assert(queue_.size() == queue_positions_.size());
    return queue_.size();
  }
  // O(1)
  bool Empty() {
    assert(queue_.empty() == queue_positions_.empty());
    return queue_.empty();
  }
 private:
  typedef list<K> Queue;
  typedef map<const K, typename Queue::iterator> Map;
  Queue queue_;
  Map queue_positions_;
};

}  // namespace lru_cache

template<typename K, typename V> class Cache;

template<typename K, typename V>
class LRUCache : public Cache<K, V> {
 public:
  // O(1)
  LRUCache(int max_size)
    : max_size_(max_size),
      size_(0) {}
  // O(n log n)
  void Pin(const K &key) {
    assert(ContainsKey(key));
    CachedItem *item = &(items_[key]);
    if (Pin(key, item))
      DeleteUnpinnedItemsIfNecessary();
  }
  // O(n log n)
  virtual void Unpin(const K &key) {
    assert(ContainsKey(key));
    CachedItem *item = &(items_[key]);
    if (Unpin(key, item))
      DeleteUnpinnedItemsIfNecessary();
  }
  // O(n log n)
  virtual void MassUnpin() {
    bool possible_delete_needed = false;
    for (typename CacheMap::iterator it = items_.begin();
        it != items_.end(); ++it) {
      possible_delete_needed |= Unpin(it->first, &it->second);
    }
    if (possible_delete_needed)
      DeleteUnpinnedItemsIfNecessary();
  }
  // O(n log n)
  virtual bool FetchAndPin(const K &key, V **value) {
    if (!ContainsKey(key)) {
      return false;
    } 
    Pin(key);
    *value = items_[key].ptr;
    return true;
  }
  // O(n log n)
  virtual void StoreAndPinSized(const K &key, V *value, const int size) {
    size_ += size;
    CachedItem new_item;
    new_item.ptr = value;
    new_item.use_count = 1;
    new_item.size = size;
    items_[key] = new_item;
    DeleteUnpinnedItemsIfNecessary();
  }
  // O(1)
  virtual bool ContainsKey(const K &key) {
    return items_.find(key) != items_.end();
  }
  // O(1)
  virtual int MaxSize() const {
    return max_size_;
  }
  // O(1)
  virtual int Size() const {
    return size_;
  }
  // O(n log n)
  virtual void SetMaxSize(const int max_size) {
    max_size_ = max_size;
    DeleteUnpinnedItemsIfNecessary();
  }
  // O(n)
  virtual ~LRUCache() {
    typename CacheMap::iterator it;
    for (it = items_.begin(); it != items_.end(); ++it)
      delete it->second.ptr;
  }

 private:
  // O(n log n)
  void DeleteUnpinnedItemsIfNecessary() {
    while (!unpinned_items_.Empty() &&
           size_ > max_size_) {
      K key_to_remove;
      unpinned_items_.Dequeue(&key_to_remove);
      typename CacheMap::iterator it = items_.find(key_to_remove);
      assert(it != items_.end());
      CachedItem *item = &it->second;
      delete item->ptr;
      size_ -= item->size;
      items_.erase(it);
    }
  }
  
  struct CachedItem {
    V *ptr;
    int use_count;
    int size;
    CachedItem() : ptr(NULL) {}
  };
  // O(log n)
  bool Pin(const K &key, CachedItem *item) {
    assert(ContainsKey(key));
    bool res = false;
    if (item->use_count == 0) {
      unpinned_items_.Remove(key);
      res = true;
    }
    item->use_count++;
    return res;
  }
  // O(log n)
  // Using this function prevents the need to do a lookup for the CachedItem
  // (when not debugging). It returns whether the item was entirely unpinned
  // and hence whether DeleteUnpinnedItemsIfNecessary() should be called.
  bool Unpin(const K &key, CachedItem *item) {
    assert(ContainsKey(key));
    assert(item->use_count > 0);
    assert(item == &(items_[key]));
    bool res = false;
    if (item->use_count == 1) {
      unpinned_items_.Enqueue(key);
      res = true;
    }
    item->use_count--;
    return res;
  }
  
  lru_cache::SetQueue<K> unpinned_items_;
  typedef map<const K, CachedItem> CacheMap;
  CacheMap items_;

  int max_size_;
  int size_;
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_LRU_CACHE_H_
