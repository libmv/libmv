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
// Random stuff for experimentation.

#include "libmv/multiview/test_data_sets.h"
#include "libmv/tools/tool.h"

DEFINE_int32(views, 3, "Number of views to generate.");

using namespace libmv;

int main(int argc, char **argv) {
  libmv::Init("", &argc, &argv);

  int nviews = FLAGS_views;
  int npoints = 8;
  NViewDataSet d = NRealisticCamerasFull(nviews, npoints);

  LOG(INFO) << "Points:" << "\n" << d.X;
  for (int i = 0; i < nviews; ++i) {
    LOG(INFO) << "Camera #" << i << ", center="
              << d.C[i].transpose() << "\n" << d.x[i];
  }
  return 0;
}
