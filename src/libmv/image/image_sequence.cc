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

//#include <pair>
#include <string>
#include <vector>
#include <cstdio>

#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_sequence.h"
#include "libmv/image/lru_cache.h"

namespace libmv {

ImageSequence::~ImageSequence() {}
Array3Df *ImageSequence::GetFloatImage(int i) {
  Image *image = GetImage(i);
  assert(image);
  return image->AsArray3Df();
}

// A key for a shared image cache. Typically the void * will be a pointer to
// the image sequence that is using the cache.
typedef std::pair<void *, int> TaggedImageKey;

// A image cache that is shared among many image sequences (or anything that
// produces images).
typedef LRUCache<TaggedImageKey, Image *> ImageCache;

ImageCache *NewImageCache(int max_size) {
  return new ImageCache(max_size);
}

class CachedImageSequence : public ImageSequence {
 public:
  virtual ~CachedImageSequence() {}
  CachedImageSequence(int max_size)
      : cache_(NewImageCache(max_size)) {}
  CachedImageSequence(ImageCache *image_cache)
      : cache_(image_cache) {}

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

  virtual int length() = 0;
  virtual Image *LoadImage(int i) = 0;

 private:
  ImageCache *cache_;
};

// An image sequence loaded from disk with caching behaviour.
class ImageSequenceFromFile : public CachedImageSequence {
 public:
  virtual ~ImageSequenceFromFile() {}
  ImageSequenceFromFile(const std::vector<std::string> &image_filenames)
      // TODO(keir): Ick; constant here is nasty. It almost certainly needs to
      // change.
      : CachedImageSequence(4*1024*1024) {
    filenames_ = image_filenames;
  }
  virtual int length() {
    return filenames_.size();
  }
  virtual Image *LoadImage(int i) {
    Array3Df *image = new Array3Df;
    // TODO(keir): Make this handle png, jpg, etc.
    if (!ReadPnm(filenames_[i].c_str(), image)) {
      delete image;
      // TODO(keir): Better error reporting?
      printf("Failed loading image %d: %s\n", i, filenames_[i].c_str());
      return 0;
    }
    return new Image(image);
  }

 private:
  std::vector<std::string> filenames_;
};

ImageSequence *ImageSequenceFromFiles(const std::vector<string> &filenames) {
  return new ImageSequenceFromFile(filenames);
}

}  // namespace libmv
