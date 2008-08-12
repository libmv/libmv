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

#ifndef LIBMV_MULTIVIEW_FUNDAMENTAL_H_
#define LIBMV_MULTIVIEW_FUNDAMENTAL_H_

#include <vector>

#include "libmv/numeric/numeric.h"

namespace libmv {

void FundamentalFromProjections(const Mat34 &P1, const Mat34 &P2, Mat3 *F);

// TODO(pau) These two functions should go in a more general header.
void PreconditionerFromPoints(const Mat &points, Mat3 *T);

void ApplyTransformationToPoints(const Mat &points,
                                 const Mat3 &T,
                                 Mat *normalized_points);

double FundamentalFromCorrespondencesLinear(const Mat &x1,
                                            const Mat &x2,
                                            Mat3 *F);

void EnforceFundamentalRank2Constraint(Mat3 *F);

double FundamentalFromCorrespondences8Point(const Mat &x1,
                                            const Mat &x2,
                                            Mat3 *F);

void NormalizeFundamental(const Mat3 F, Mat3 *F_normalized);

// Compute the relative camera motion between two cameras.
// Given the motion parameters of two cameras, computes the motion parameters
// of the second one assuming the first one to be at the origin.
// If T1 and T2 are the camera motions, the computed relative motion is
// 
//      T = T2 T1^{-1}
//
void RelativeCameraMotion(const Mat3 &R1,
                          const Vec3 &t1,
                          const Mat3 &R2,
                          const Vec3 &t2,
                          Mat3 *R,
                          Vec3 *t);

void EssentialFromFundamental(const Mat3 &F,
                              const Mat3 &K1,
                              const Mat3 &K2,
                              Mat3 *E);

void EssentialFromRt(const Mat3 &R1,
                     const Vec3 &t1,
                     const Mat3 &R2,
                     const Vec3 &t2,
                     Mat3 *E);


} // namespace libmv

#endif  // LIBMV_MULTIVIEW_FUNDAMENTAL_H_
