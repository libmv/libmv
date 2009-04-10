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

#include <vector>

#include "libmv/image/image_pyramid.h"
#include "libmv/image/image_sequence.h"
#include "libmv/image/image_sequence_filters.h"
#include "libmv/image/pyramid_sequence.h"
#include "libmv/image/lru_cache.h"

namespace libmv {

PyramidSequence::~PyramidSequence() {}

class ImageSequenceBackedImagePyramid : public ImagePyramid {
 public:
  virtual ~ImageSequenceBackedImagePyramid() {
    for (size_t i = 0; i < pinned_levels_.size(); ++i) {
      if (pinned_levels_[i]) {
        levels_[i]->Unpin(frame_);
      }
    }
  }

  ImageSequenceBackedImagePyramid(const std::vector<ImageSequence *> levels,
                                  int frame)
      : levels_(levels),
        pinned_levels_(levels.size()),
        frame_(frame) {
    for (size_t i = 0; i < pinned_levels_.size(); ++i) {
      pinned_levels_[i] = 0;
    }
  }

  virtual const Array3Df &Level(int i) {
    pinned_levels_[i] = 1;
    return *levels_[i]->GetFloatImage(frame_);
  }

  virtual int NumLevels() const {
    return levels_.size();
  }

  virtual int MemorySizeInBytes() const {
    //TODO(pau) this is ugly!
    return -1;
  }

 private:
  const std::vector<ImageSequence *> levels_;
  std::vector<int> pinned_levels_;
  int frame_;
};

class ConcretePyramidSequence : public PyramidSequence {
 public:
  virtual ~ConcretePyramidSequence() {
    for (size_t i = 0; i < levels_.size(); ++i) {
      delete downsamples_[i];
    }
    for (size_t i = 0; i < levels_.size(); ++i) {
      delete levels_[i];
    }
    for (int i = 0; i < source_->Length(); ++i) {
      delete constructed_pyramids_[i];
    }
  }

  ConcretePyramidSequence(ImageSequence *source, int levels, double sigma) {
    downsamples_.resize(levels);
    downsamples_[0] = source;
    for (int i = 1; i < levels; ++i) {
      downsamples_[i] = DownsampleSequenceBy2(downsamples_[i-1]);
    }
    levels_.resize(levels);
    for (int i = 0; i < levels; ++i) {
      levels_[i] = BlurSequenceAndTakeDerivatives(downsamples_[i], sigma);
    }
    constructed_pyramids_.resize(source->Length());
    for (int i = 0; i < source->Length(); ++i) {
      constructed_pyramids_[i] = NULL;
    }
  }

  virtual int Length() {
    return source_->Length();
  }

  virtual ImagePyramid *Pyramid(int frame) {
    if (!constructed_pyramids_[frame]) {
      constructed_pyramids_[frame] =
          new ImageSequenceBackedImagePyramid(levels_, frame);
    }
    return constructed_pyramids_[frame];
  }

 private:
  ImageSequence *source_;
  std::vector<ImageSequence *> levels_;
  std::vector<ImageSequence *> downsamples_;
  std::vector<ImagePyramid *> constructed_pyramids_;
};

PyramidSequence *MakePyramidSequence(ImageSequence *source,
                                     int levels,
                                     double sigma) {
  return new ConcretePyramidSequence(source, levels, sigma);
}


/////////////////////////////////////////////////////////////
// This is pau trying things.

class SimpleConcretePyramidSequence : public PyramidSequence {
 public:
  virtual ~SimpleConcretePyramidSequence() {
  }

  SimpleConcretePyramidSequence(ImageSequence *source,
                                 int levels,
                                 double sigma)
    : source_(source), levels_(levels), sigma_(sigma), cache_(10*1024*1024) {
  }

  virtual int Length() {
    return source_->Length();
  }

  virtual ImagePyramid *Pyramid(int frame) {
    ImagePyramid *pyramid;
    if (!cache_.FetchAndPin(frame, &pyramid)) {
      pyramid = MakeImagePyramid(*source_->GetFloatImage(frame),
                                 levels_,
                                 sigma_);
      source_->Unpin(frame);
      cache_.StoreAndPinSized(frame, pyramid, pyramid->MemorySizeInBytes());
    }
    return pyramid;
  }

 private:
  ImageSequence *source_;
  int levels_;
  double sigma_;
  LRUCache<int, ImagePyramid> cache_;
};

PyramidSequence *MakeSimplePyramidSequence(ImageSequence *source,
                                            int levels,
                                            double sigma) {
  return new SimpleConcretePyramidSequence(source, levels, sigma);
}

// End of pau trying things.
/////////////////////////////////////////////////////////////

}  // namespace libmv
