// Copyright (c) 2009 libmv authors.
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

#ifndef LIBMV_DETECTOR_FAST_DETECTOR_H
#define LIBMV_DETECTOR_FAST_DETECTOR_H

namespace libmv {
namespace detector {

class Detector;

// Wrapper around the FAST corner detector [1].
// [1] Machine learning for high-speed corner detection,
//  E. Rosten and T. Drummond, ECCV 2006

/**
 * Creates a detector that uses the FAST detection algorithm. The
 * implementation never returns detection data.
 *
 * \param size      The size of features to detect in pixels {9,10,11,12}.
 * \param threshold Threshold for detecting features (barrier). See the FAST
 *                  paper for details [1].
 * \param bRotationInvariant Tell if orientation of detected features must
 *                            be estimated.
 */
Detector *CreateFastDetector(int size = 9, int threshold = 30,
                              bool bRotationInvariant = false);

/**
 * Creates a detector that uses the FAST detection algorithm. The
 * implementation never returns detection data.
 * Optimized for near real-time application. It extract FAST 9 sized keypoint
 * and keep the N strongest one.
 *
 * \param threshold Threshold for detecting features (barrier). See the FAST
 *                  paper for details [1].
 * \param bRotationInvariant Tell if orientation of detected features must
 *                            be estimated.
 * \param nKeypointMax How many points could be detected.
 *                     ( nbReturnedPoint will be <= nKeypointMax).
 */
Detector *CreateFastDetectorLimited(int threshold = 30,
                              bool bRotationInvariant = false,
                              int nKeypointMax = 256);

}  // namespace detector
}  // namespace libmv

#endif  // LIBMV_DETECTOR_FAST_DETECTOR_H
