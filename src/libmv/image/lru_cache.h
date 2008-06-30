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
//
// A simple LRU cache. It uses a fair amount of space per-item, so do not use
// this to store many small items. Instead, use this for heavy-weight objects
// like images.

#ifndef LIBMV_IMAGE_LRU_CACHE_H_
#define LIBMV_IMAGE_LRU_CACHE_H_

#include <map>
#include <list>
#include <cassert>

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
  void Enqueue(const K &key) {
    assert(!Contains(key));
    queue_.push_front(key);
    queue_positions_[key] = queue_.begin();
  }
  void Remove(const K &key) {
    assert(Contains(key));
    queue_.erase(queue_positions_[key]);
    queue_positions_.erase(queue_positions_.find(key));
  }
  void Dequeue(K *key) {
    assert(queue_.size() > 0);
    typename Queue::iterator last = queue_.end();
    --last;  // Necessary because end is one past the last item.
    *key = *last;
    Remove(*key);
  }
  bool Contains(const K &key) {
    return queue_positions_.find(key) != queue_positions_.end();
  }
  void Clear() {
    queue_positions_.clear();
    queue_.clear();
  }
  int Size() {
    assert(queue_.size() == queue_positions_.size());
    return queue_.size();
  }
 private:
  typedef list<K> Queue;
  typedef map<const K, typename Queue::iterator> Map;
  Queue queue_;
  Map queue_positions_;
};

}  // namespace lru_cache

template<typename K, typename V> class Cache;

// TODO(keir): Document O(...) performance for operations.
template<typename K, typename V>
class LRUCache : public Cache<K, V> {
 public:
  LRUCache(int max_size)
    : max_size_(max_size),
      size_(0) {}

  void Pin(const K &key) {
    CachedItem *item = &(items_[key]);
    if (item->use_count == 0) {
      unpinned_items_.Remove(key);
    } 
    item->use_count++;
  }

  virtual void Unpin(const K &key) {
    CachedItem *item = &(items_[key]);
    if (item->use_count > 0) {
      if (item->use_count == 1) {
        unpinned_items_.Enqueue(key);
      } 
      item->use_count--;
    }
  }

  virtual void MassUnpin() {
    for (typename CacheMap::iterator it = items_.begin();
        it != items_.end(); ++it) {
      Unpin(it->first);
    }
  }

  virtual bool FetchAndPin(const K &key, V *value) {
    if (!ContainsKey(key)) {
      return false;
    } 
    Pin(key);
    *value = items_[key].value;
    return true;
  }

  virtual void StoreAndPinSized(const K &key, V value, const int size) {
    CachedItem new_item;
    new_item.size = size;
    size_ += size;
    new_item.value = value;
    new_item.use_count = 1;
    items_[key] = new_item;
    DeleteUnpinnedItemsIfNecessary();
  }

  virtual bool ContainsKey(const K &key) {
    return items_.find(key) != items_.end();
  }

  virtual const int MaxSize() {
    return max_size_;
  }

  virtual const int Size() {
    return size_;
  }

  virtual void SetMaxSize(const int max_size) {
    // TODO(keir): Evict items if max_size has shrunk smaller than what's
    // currently in the cache.
    max_size_ = max_size;
  }

  virtual ~LRUCache() {}

 private:
  void DeleteUnpinnedItemsIfNecessary() {
    while (unpinned_items_.Size() &&
           size_ > max_size_) {
      K key_to_remove;
      unpinned_items_.Dequeue(&key_to_remove);
      CachedItem *item = &(items_[key_to_remove]);
      size_ -= item->size;
      items_.erase(items_.find(key_to_remove));
    }
  }

  lru_cache::SetQueue<K> unpinned_items_;
  struct CachedItem {
    V value;
    int use_count;
    int size;
  };
  typedef map<const K, CachedItem> CacheMap;
  CacheMap items_;

  int max_size_;
  int size_;
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_LRU_CACHE_H_
