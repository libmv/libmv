// Copyright 2008 Keir Mierle, all rights reserved.
//
// Tests for the LRU cache.

#include "libmv/image/lru_cache.h"
#include "testing/testing.h"

namespace {

typedef libmv::LRUCache<int, int> TestCache;

TEST(NullOnEmptyKey) {
  TestCache cache(10);
  int result;
  bool succeeded = cache.FetchAndPin(4, &result);
  Check(!succeeded);
}

TEST(StoreAndRetreiveOneItem) {
  TestCache cache(10);
  int result;
  cache.StoreAndPin(4, 40);
  bool succeeded = cache.FetchAndPin(4, &result);
  Equals(succeeded, true);
  Equals(result, 40);
}

TEST(SizeIncreasesWithAddedItems) {
  TestCache cache(10);
  Equals(cache.Size(), 0);
  cache.StoreAndPin(4, 40);
  Equals(cache.Size(), 1);
  cache.StoreAndPin(5, 40);
  Equals(cache.Size(), 2);
}

TEST(MaxSizeExceededWhenItemsPinned) {
  TestCache cache(3);
  cache.StoreAndPin(4, 40);
  cache.StoreAndPin(5, 50);
  cache.StoreAndPin(6, 60);
  Equals(cache.Size(), 3);
  cache.StoreAndPin(7, 70);
  Equals(cache.Size(), 4);
}

TEST(MaxSizeNotExceededWhenItemsUnpinned) {
  TestCache cache(3);
  cache.StoreAndPin(4, 40);
  cache.StoreAndPin(5, 50);
  cache.StoreAndPin(6, 60);
  Equals(cache.Size(), 3);
  cache.MassUnpin();
  cache.StoreAndPin(7, 70);
  Equals(cache.Size(), 3);
  cache.StoreAndPin(8, 80);
  Equals(cache.Size(), 3);
  cache.StoreAndPin(9, 90);
  Equals(cache.Size(), 3);
}

TEST() {
  TestCache cache(3);
  cache.StoreAndPin(4, 40);
  cache.StoreAndPin(5, 50);
  cache.StoreAndPin(6, 60);
  Equals(cache.Size(), 3);
  cache.MassUnpin();
  cache.StoreAndPin(7, 70);
  Equals(cache.Size(), 3);
  cache.StoreAndPin(8, 80);
  Equals(cache.Size(), 3);
  cache.StoreAndPin(9, 90);
  Equals(cache.Size(), 3);
}

}  // namespace
