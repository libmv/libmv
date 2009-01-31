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

#include "libmv/image/array_nd.h"
#include "libmv/image/non_maximal_suppression.h"
#include "testing/testing.h"

using libmv::Array3Df;

namespace {

TEST(IsLocalMax3D, AllZerosAreNotMaxima) {
  Array3Df A(5, 5, 5);
  A.Fill(0.0);
  for (int x = 0; x < 5; ++x) {
    for (int y = 0; y < 5; ++y) {
      for (int z = 0; z < 5; ++z) {
        EXPECT_FALSE(IsLocalMax3D(A, 3, x, y, z));
      }
    }
  }
}

TEST(IsLocalMax3D, CompeteingMaxesInOneBlock) {
  Array3Df A(5, 5, 5);
  A.Fill(0.0);
  A(2, 2, 2) = 1.0;  // This is max within the 3x3x3 block.
  A(3, 3, 3) = 3.0;  // But the real max, here, is in another block.

  EXPECT_FALSE(IsLocalMax3D(A, 3, 2, 2, 2));
  EXPECT_TRUE(IsLocalMax3D(A, 3, 3, 3, 3));
}

TEST(IsLocalMax2D, AllZerosAreNotMaxima) {
  Array3Df A(5, 5);
  A.Fill(0.0);
  for (int x = 0; x < 5; ++x) {
    for (int y = 0; y < 5; ++y) {
      EXPECT_FALSE(IsLocalMax2D(A, 3, x, y));
    }
  }
}

TEST(IsLocalMax2D, CompeteingMaxesInOneBlock) {
  Array3Df A(5, 5);
  A.Fill(0.0);
  A(2, 2) = 1.0;  // This is max within the 3x3 block.
  A(3, 3) = 3.0;  // But the real max, here, is in another block.

  EXPECT_FALSE(IsLocalMax2D(A, 3, 2, 2));
  EXPECT_TRUE(IsLocalMax2D(A, 3, 3, 3));
}
struct StoreResults {
 public:
  void operator() (int x, int y, int z, float value) {
    (void) value;
    results.push_back(x);
    results.push_back(y);
    results.push_back(z);
  }
  std::vector<int> results;
};

// TODO(keir): These tests rely on the order of the maxima; rejig them to be
// independ of returned order.
TEST(FindLocalMaxima3D, EasyCase) {
  Array3Df A(3, 3, 3);
  A.Fill(0.0);
  A(1, 1, 1) = 1.0;

  StoreResults results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(1, results.results[0]);
  EXPECT_EQ(1, results.results[1]);
  EXPECT_EQ(1, results.results[2]);
  EXPECT_EQ(3, results.results.size());
}

TEST(FindLocalMaxima3D, CompeteingMaxesInOneBlock) {
  Array3Df A(5, 5, 5);
  A.Fill(0.0);
  A(2, 2, 2) = 1.0;  // This is max within the 3x3x3 block.
  A(3, 3, 3) = 3.0;  // But the real max, here, is in another block.

  StoreResults results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(3, results.results[0]);
  EXPECT_EQ(3, results.results[1]);
  EXPECT_EQ(3, results.results[2]);
  EXPECT_EQ(3, results.results.size());
}

TEST(FindLocalMaxima3D, MultipleMaxes) {
  Array3Df A(20, 20, 20);
  A.Fill(0.0);
  A(2, 2, 2) = 1.0;  // This is max within the 3x3x3 block.
  A(3, 3, 3) = 3.0;  // But the real max, here, is in another block.

  A(15, 13, 12) = 1.0;

  StoreResults results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(3, results.results[0]);
  EXPECT_EQ(3, results.results[1]);
  EXPECT_EQ(3, results.results[2]);
  EXPECT_EQ(15, results.results[3]);
  EXPECT_EQ(13, results.results[4]);
  EXPECT_EQ(12, results.results[5]);
  EXPECT_EQ(6, results.results.size());
}

TEST(FindLocalMaxima3D, MultipleMaxesInSameBlock) {
  Array3Df A(20, 20, 20);
  A.Fill(0.0);
  A(0, 0, 0) = 1.0;  // This is a local max within a 3x3x3 block.
  A(2, 2, 2) = 3.0;  // And so is this (but the radii are close).

  StoreResults results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(0, results.results[0]);
  EXPECT_EQ(0, results.results[1]);
  EXPECT_EQ(0, results.results[2]);
  EXPECT_EQ(2, results.results[3]);
  EXPECT_EQ(2, results.results[4]);
  EXPECT_EQ(2, results.results[5]);
  EXPECT_EQ(6, results.results.size());
}
}  // namespace
