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

#ifndef LIBMV_CORRESPONDENCE_FEATURE_SET_H_
#define LIBMV_CORRESPONDENCE_FEATURE_SET_H_

#include <map>

#include "libmv/numeric/numeric.h"
#include "libmv/base/id_generator.h"
#include "libmv/correspondence/feature.h"

namespace libmv {

typedef int FeatureID;


/**
 * Class to store the features of a single frame.
 *
 * Different types of features can be added.  The template function All can be
 * used to iterate over the features of a specific type.  When a feature is
 * inserted a new ID is associated to the feature so that
 * the feature can be retrived later.
 *
 * Internally the class is a thin layer over std::map to be able to deal with
 * differnt features types in the same bag.
 */
class FeatureSet {
  typedef std::map<FeatureID, Feature *> FeatureMap;
  
 public:
  /**
   * Iterate over features, silently skiping any that are not FeatureT or
   * derived from FeatureT.
   */
  template<typename FeatureT = Feature>
  class Iterator {
   public:
    const FeatureID id() const { return r_->first; }
    FeatureT &feature() {
      return *static_cast<FeatureT *>(r_->second);
    }
    operator bool() const { return r_ != end_; }
    void operator++() { ++r_; Skip(); }
    Iterator(FeatureMap::iterator begin, FeatureMap::iterator end)
      : r_(begin), end_(end) { Skip(); }

   private:
    void Skip() {
      while (r_ != end_
             && !dynamic_cast<const FeatureT *>(r_->second)) {
        ++r_;
      }
    }
    FeatureMap::iterator r_;
    FeatureMap::iterator end_;

    friend class FeatureSet;
  };

  /**
   * Destructor destroys all features.
   */
  ~FeatureSet() {
    for (Iterator<Feature> i = All<Feature>(); i; ++i) {
      delete &i.feature();
    }
  }

  /**
   * Iterator over all the features of type FeatureT.
   */
  template<typename FeatureT>
  Iterator<FeatureT> All() {
    return Iterator<FeatureT>(features_.begin(), features_.end());
  }

  /**
   * Add a new a feature of type FeatureT and return an iterator to it.
   */
  template<typename FeatureT>
  Iterator<FeatureT> New() {
    Feature *f = new FeatureT;
    FeatureID id = id_generator_.Generate();
    
    FeatureMap::iterator it =
        features_.insert(FeatureMap::value_type(id, f)).first;
    return Iterator<FeatureT>(it, features_.end());
  }

  /**
   * Insert a copy of an existing features.
   *
   * This function adds a feature to the feature set.  The feature is copied
   * by value since FeatureSet owns its features.  If you want to avoid the
   * copy, use the \ref New method to create the feature directly in the
   * FeatureSet.
   */
  template<typename FeatureT>
  Iterator<FeatureT> Insert(const FeatureT &feature) {
    Feature *f = new FeatureT(feature);
    FeatureID id = id_generator_.Generate();
    
    FeatureMap::iterator it =
        features_.insert(FeatureMap::value_type(id, f)).first;
    return Iterator<FeatureT>(it, features_.end());
  }

  /**
   * Deletes a feature.
   */
  template<typename FeatureT>
  void Delete(Iterator<FeatureT> *it) {
    delete &it->feature();
    features_.erase(it->r_);
  }

  /**
   * Finds a feature given its ID.
   */
  template<typename FeatureT>
  Iterator<FeatureT> Find(FeatureID id) {
    return Iterator<FeatureT>(features_.find(id), features_.end());
  }

 private: 
  FeatureMap features_;   // This map owns the features.
  IdGenerator id_generator_;
};

}  // namespace libmv

#endif  // LIBMV_CORRESPONDENCE_FEATURE_SET_H_
