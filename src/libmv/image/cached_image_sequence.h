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

#ifndef LIBMV_IMAGE_CACHED_IMAGE_SEQUENCE_H_
#define LIBMV_IMAGE_CACHED_IMAGE_SEQUENCE_H_

#include "libmv/image/image.h"
#include "libmv/image/lru_cache.h"
#include "libmv/image/image_sequence.h"

namespace libmv {

// A key for a shared image cache. Typically the void * will be a pointer to
// the image sequence that is using the cache.
typedef std::pair<void *, int> TaggedImageKey;

// A image cache that is shared among many image sequences (or anything that
// produces images).
class ImageCache : public LRUCache<TaggedImageKey, Image *> {
 public:
  typedef LRUCache<TaggedImageKey, Image *> Base;
  ImageCache() : Base(10*1024*1024) {}
  ImageCache(int max_cache_size_in_bytes) : Base(max_cache_size_in_bytes) {}
};

class CachedImageSequence : public ImageSequence {
 public:
  virtual ~CachedImageSequence() {}
  CachedImageSequence(ImageCache *cache)
      : cache_(cache) {}

  virtual Image *GetImage(int i) {
    Image *image;
    TaggedImageKey cache_key(this, i);
    if (!cache_->FetchAndPin(cache_key, &image)) {
      image = LoadImage(i);
      if (!image) {
        return 0;
      }
      cache_->StoreAndPinSized(cache_key, image, image->MemorySizeInBytes());
    }
    return image;
  }

  virtual void Unpin(int i) {
    TaggedImageKey cache_key(this, i);
    cache_->Unpin(cache_key);
  }

  virtual ImageCache *Cache() {
    return cache_;
  }

  // Subclasses must override these. The cached image sequence will take care
  // of storing the generated
  virtual Image *LoadImage(int i) = 0;

 private:
  ImageCache *cache_;
};

}  // namespace libmv

#endif  // LIBMV_IMAGE_CACHED_IMAGE_SEQUENCE_H_
