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

#ifndef LIBMV_MULTIVIEW_ROBUST_ESTIMATION_H_
#define LIBMV_MULTIVIEW_ROBUST_ESTIMATION_H_

#include <set>
#include <vector>

#include "libmv/logging/logging.h"
#include "libmv/numeric/numeric.h"

namespace libmv {

// A random subset of the integers [0, total), in random order.
// Note that this can behave badly if num_samples is close to total; runtime
// could be unlimited!
// TODO(keir): Implement smarter O(nlogn) method for this which is independent
// of total size. Surprisingly tricky!
// Consider: http://stackoverflow.com/questions/136474/best-way-to-pick-a-random-subset-from-a-collection
VecXi PickSubset(int num_samples, int total) {
  int i = 0;
  std::set<int> sample_set;
  VecXi samples(num_samples);
  while (sample_set.size() < num_samples) {
    int sample_set_size = sample_set.size();
    int random_value_in_range = rand() % total;
    sample_set.insert(random_value_in_range);
    if (sample_set_size != sample_set.size()) {
      samples[i++] = random_value_in_range;
    }
  }
  return samples;
}

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
// TODO(keir): This is unused below for now; add the early bailout!
double SigmaApprox(int num_inliers, int num_sampled, int num_total) {
  double inlier_ratio = num_inliers;
  inlier_ratio /= num_total;  // e hat in equation (8).
  return num_sampled * inlier_ratio * (1 - inlier_ratio) *
    (num_total - num_sampled) / (num_total - 1);
}

template<typename Model, typename TMat, typename Fitter, typename Classifier,
         typename CostFunction>
Model Estimate(TMat &samples, Fitter fitter, Classifier classifier,
               CostFunction cost_function, std::vector<int> *inliers = NULL) {
  uint iteration = 0;
  uint max_iterations = 100;
  const uint really_max_iterations = 1000;
  int min_samples = fitter.MinimumSamples();
  int total_samples = samples.cols();

  int best_num_inliers = 0;
  double best_cost = HUGE_VAL;
  double best_inlier_ratio = 0.0;
  Model best_model;

  for (iteration = 0;
       iteration < max_iterations &&
       iteration < really_max_iterations; ++iteration) {
    VecXi subset_indices = PickSubset(min_samples, total_samples);
    TMat subset = ExtractColumns(samples, subset_indices);
    VLOG(2) << "Random subset: (" << subset_indices.size() << ")\n"
             << subset_indices;

    std::vector<Model> models;
    fitter.Fit(subset, &models);
    VLOG(2) << "Fitted subset; found " << models.size() << " model(s).";

    for (int i = 0; i < models.size(); ++i) {
      // Compute costs for each fit, possibly bailing early if the model looks
      // like it does not promise to beat the current best.
      int num_inliers = 0;
      double inlier_error = 0;
      double cost = 0;
      for (int j = 0; j < total_samples; ++j) {
        double error_j = models[i].Error(samples.col(j));
        double cost_j = cost_function.Cost(error_j);
        bool is_inlier = classifier.IsInlier(error_j);
        cost += cost_j;
        num_inliers += is_inlier ? 1 : 0;
        inlier_error += is_inlier ? error_j : 0;
        VLOG(4) << "error_j=" << error_j
                << " cost_j=" << cost_j
                << " is_inlier=" << is_inlier;
        // TODO(keir): Add hypergometric early-breakout.
      }
      VLOG(5) << "Fit cost: " << cost << ", number of inliers: " << num_inliers;

      if (cost < best_cost) {
        best_cost = cost;
        best_inlier_ratio = num_inliers / float(total_samples);
        best_num_inliers = num_inliers;
        best_model = models[i];
        VLOG(2) << "New best cost: " << best_cost << " with "
                << best_num_inliers << " inlying of "
                << total_samples << " total samples. Inlier error:"
                << inlier_error;
        // TODO(keir): Add refinement (Lo-RANSAC) here.
      }
    }

    // TODO(keir): This is a knob that likely should be exposed.
    const double desired_certainty = 0.03;
    double needed_iterations = log(desired_certainty)
                             / log(1 - pow(best_inlier_ratio, min_samples));
    max_iterations = static_cast<uint>(needed_iterations);

    VLOG(2) << "Max iterations needed given best inlier ratio: "
            << max_iterations << "; best inlier ratio: " << best_inlier_ratio;
  }

  // Compute inliers for best_model.
  if (inliers) {
    for (int j = 0; j < total_samples; ++j) {
      double error_j = best_model.Error(samples.col(j));
      if (classifier.IsInlier(error_j)) {
        inliers->push_back(j);
      }
    }
  }

  return best_model;
}

struct ThresholdClassifier {
  ThresholdClassifier(double threshold) : threshold_(threshold) {}
  bool IsInlier(double error) {
    return error < threshold_;
  }
  double threshold_;
};

struct MLECost {
  MLECost(double threshold) : threshold_(threshold) {}
  double Cost(double error) {
    return error < threshold_ ? error : threshold_;
  }
  double threshold_;
};

} // namespace libmv

#endif  // LIBMV_MULTIVIEW_ROBUST_ESTIMATION_H_
