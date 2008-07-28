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

#include <string>
#include <vector>

#include "libmv/image/image.h"
#include "libmv/image/image_io.h"
#include "libmv/image/image_sequence.h"
#include "libmv/image/lru_cache.h"

namespace libmv {

// TODO(keir): Merge the image caches into one large cache that caches across
// filters. This is tough because we don't have a single image base class that
// we can put in the cache; we have to instead cache per filter.
//
// TODO(keir): There needs to be cache introspection support; for testing, and
// for displaying in a GUI (i.e. what images are loaded in the cache).

template<T>
class CachedImageSequence : ImageSequence<T> {
 public:
  CachedImageSequence(int max_size)
      : cache_(max_size) {}

  virtual Image<T> *LoadPinned(int i) {
    Image<T> *image;
    if (!cache_.FetchAndPin(i, &image)) {
      image = LoadImage(i);
      if (!image) {
        return 0;
      }
      // TODO(keir): Pick a more useful size here...
      cache_.StoreAndPin(i, image);
    }
    return result;
  }
  virtual int size() = 0;
  virtual void Unpin(int i) {
    cache_.Unpin(i);
  }

  virtual Image<T> *LoadImage(int i) = 0;

 private:
  LRUCache<std::string, Image<T> *> cache_;
};

// An image sequence loaded from disk with caching behaviour.
template<T>
class ImageSequenceFromFile : CachedImageSequence<T> {
 public:
  ImageSequenceFromFile(const std::vector<std::string> &image_filenames)
      // TODO(keir): Ick; constant here is nasty. It almost certainly needs to
      // change.
      : CachedImageSequence(4) {
    filenames_ = image_filenames;
  }
  virtual int size() {
    return filenames_.size();
  }
  virtual Image<T> *LoadImage(int i) {
    Image<T> *image = new Image<T>;
    // TODO(keir): Make this handle png, jpg, etc.
    if (!ReadPnm(filenames_[i], image)) {
      delete image;
      return 0;
    }
    return image;
  }

 private:
  std::vector<std::string> filenames_;
};

ByteImageSequence *ByteImageSequenceFromFiles(
    const std::vector<string> &filenames) {
  return new ImageSequenceFromFile<unsigned char>(filenames);
}

}  // namespace libmv

#endif  // LIBMV_IMAGE_IMAGE_SEQUENCE_H_
