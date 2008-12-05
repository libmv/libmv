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

#include <cmath>
#include <cstdio>
#include <algorithm>
#include <set>
#include <vector>

#include "libmv/multiview/robust_estimation.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

Model::~Model() {}
Fitter::~Fitter() {}
RobustFitter::~RobustFitter() {}

// From the following papers:
//
// Chum, O. and Matas. J.: Optimal Randomized RANSAC, PAMI, August 2008
// http://cmp.felk.cvut.cz/~chum/papers/chum-pami08.pdf
//
// Capel, D.P., An Effective Bail-out Test for RANSAC Consensus Scoring, BMVC05.
// http://www.comp.leeds.ac.uk/bmvc2008/proceedings/2005/papers/224/Capel224.pdf
//
// The bailout technique used is the one from Capel's paper. The technique in
// the Chum paper is considerably more involved to implement for only marginal
// gains compared to the one in the Capel paper.

// From equation (8) in the Capel paper.
double SigmaApprox(int num_inliers, int num_sampled, int num_total) {
  double inlier_ratio = num_inliers;
  inlier_ratio /= num_total;  // e hat in equation (8).
  return num_sampled * inlier_ratio * (1 - inlier_ratio) *
    (num_total - num_sampled) / (num_total - 1);
}

// THIS CODE TOTALLY NOT WORKING!
Model *RobustFitter::Fit(Fitter *fitter, int total_samples) {
  int iteration = 0;
  int max_iterations = 10000;  // A big number.
  int min_samples = fitter->MinimumSamples();
  int best_num_inliers = 0;
  best_cost_ = 1e300;  // Another big number.
  double best_inlier_ratio = 0;

  while (iteration < max_iterations) {
    std::vector<int> samples;
    PickSubset(min_samples, total_samples, &samples);

    std::vector<Model *> models;
    fitter->Fit(samples, &models);

    std::vector<int> inliers;
    for (int i = 0; i < models.size(); ++i) {

      // Compute costs for each fit, possibly bailing early if the model looks
      // like it does not promise to beat the current best.
      int inliers = 0;
      double cost = 0;
      for (int j = 0; j < total_samples; ++j) {
        double cost_j;
        bool is_inlier;
        models[i]->Cost(j, &cost_j, &is_inlier);
        cost += cost_j;
        inliers += is_inlier;

        // Skip fancy early-break for now.
        continue;

        // TODO(keir): Tune this knob!
        const double zconf = +2.3;  // This is a knob in the paper based on confidence.
        double sigma = sqrt(SigmaApprox(best_num_inliers, j+1, total_samples));
        int min_inliers_to_continue =
            floor((j+1) * best_inlier_ratio - zconf * sigma);
        if (inliers < min_inliers_to_continue) {
          cost = 1e300;
          printf("aborted at %d of %d samples. sigma=%g, min_inl=%d\n",
                  j, total_samples, sigma, min_inliers_to_continue);
          break;
        }
      }

      if (cost < best_cost_) {
        best_cost_ = cost;
        inlier_samples_.clear();
        inlier_samples_.insert(inlier_samples_.begin(),
                               samples.begin(),
                               samples.end());
        best_inlier_ratio = inliers;
        best_inlier_ratio /= total_samples;
        best_num_inliers = inliers;
        best_model_ = models[i];
        models[i] = NULL;
        printf("new best score %g with %d of %d samples inlying.\n",
            best_cost_, best_num_inliers, total_samples);
        // TODO(keir): Add refinement (Lo-RANSAC) here.
      }
    }

    for (int i = 0; i < models.size(); ++i) {
      delete models[i];
    }

    const double desired_certainty = 0.05;
    double needed_iterations = log(desired_certainty)
                              / log(1 - pow(best_inlier_ratio, min_samples));
    if (needed_iterations > 1000) {
      max_iterations = 1000;
    } else {
      max_iterations = needed_iterations;
    }

    printf("num=%g, denom=%g\n",
           log(desired_certainty),
           log(1 - pow(best_inlier_ratio, min_samples)));

    printf("max_iterations=%d, best_inlier_ratio=%g\n", max_iterations, best_inlier_ratio);

    iteration++;

  }

  return best_model_;
}

void RobustFitter::PickSubset(int num_samples,
                                 int total,
                                 std::vector<int> *samples) {
  std::set<int> sample_set;
  while (sample_set.size() < num_samples) {
    sample_set.insert(rand() % total);
  }
  samples->insert(samples->begin(), sample_set.begin(), sample_set.end());
}

} // namespace libmv
