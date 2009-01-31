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

namespace libmv {
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

// TODO(keir): These tests rely on the order of the maxima; rejig them to be
// independ of returned order.
TEST(FindLocalMaxima3D, EasyCase) {
  Array3Df A(3, 3, 3);
  A.Fill(0.0);
  A(1, 1, 1) = 1.0;

  std::vector<Vec3i> results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(1, results[0][0]);
  EXPECT_EQ(1, results[0][1]);
  EXPECT_EQ(1, results[0][2]);
  EXPECT_EQ(1, results.size());
}

TEST(FindLocalMaxima3D, CompeteingMaxesInOneBlock) {
  Array3Df A(5, 5, 5);
  A.Fill(0.0);
  A(2, 2, 2) = 1.0;  // This is max within the 3x3x3 block.
  A(3, 3, 3) = 3.0;  // But the real max, here, is in another block.

  std::vector<Vec3i> results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(3, results[0][0]);
  EXPECT_EQ(3, results[0][1]);
  EXPECT_EQ(3, results[0][2]);
  EXPECT_EQ(1, results.size());
}

TEST(FindLocalMaxima3D, MultipleMaxes) {
  Array3Df A(20, 20, 20);
  A.Fill(0.0);
  A(2, 2, 2) = 1.0;  // This is max within the 3x3x3 block.
  A(3, 3, 3) = 3.0;  // But the real max, here, is in another block.

  A(15, 13, 12) = 1.0;

  std::vector<Vec3i> results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(3, results[0][0]);
  EXPECT_EQ(3, results[0][1]);
  EXPECT_EQ(3, results[0][2]);
  EXPECT_EQ(15, results[1][0]);
  EXPECT_EQ(13, results[1][1]);
  EXPECT_EQ(12, results[1][2]);
  EXPECT_EQ(2, results.size());
}

TEST(FindLocalMaxima3D, MultipleMaxesInSameBlock) {
  Array3Df A(20, 20, 20);
  A.Fill(0.0);
  A(0, 0, 0) = 1.0;  // This is a local max within a 3x3x3 block.
  A(2, 2, 2) = 3.0;  // And so is this (but the radii are close).

  std::vector<Vec3i> results;
  FindLocalMaxima3D(A, 3, &results);
  EXPECT_EQ(0, results[0][0]);
  EXPECT_EQ(0, results[0][1]);
  EXPECT_EQ(0, results[0][2]);
  EXPECT_EQ(2, results[1][0]);
  EXPECT_EQ(2, results[1][1]);
  EXPECT_EQ(2, results[1][2]);
  EXPECT_EQ(2, results.size());
}
}  // namespace libmv
}  // namespace
