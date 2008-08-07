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
// Calculate the focal length from a fundamental matrix. Requires the image to
// have a 0,0 principal point. In practice that means the F should be
// normalized before being passed to these routines.

#ifndef LIBMV_MULTIVIEW_FUNDAMENTAL_H_
#define LIBMV_MULTIVIEW_FUNDAMENTAL_H_

#include <vector>

#include "libmv/numeric/numeric.h"

namespace libmv {

void FundamentalFromProjections(const Mat34 &P1, const Mat34 &P2, Mat3 *F);
void P_From_KRt(const Mat3 &K, const Mat3 &R, const Vec3 &t, Mat34 *P);
void KRt_From_P(const Mat34 &P, Mat3 *K, Mat3 *R, Vec3 *t);

// TODO(pau) These two functions should go in a more general header.
void PreconditionerFromPoints(const Mat &points, Mat3 *T);

void ApplyTransformationToPoints(const Mat &points,
                                 const Mat3 &T,
                                 Mat *normalized_points);

void FundamentalFromCorrespondencesLinear(const Mat &x1,
                                          const Mat &x2,
                                          Mat3 *F);

void EnforceFundamentalRank2Constraint(Mat3 *F);

void FundamentalFromCorrespondences8Point(const Mat &x1,
                                          const Mat &x2,
                                          Mat3 *F);

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_FOCAL_FROM_FUNDAMENTAL_H_
