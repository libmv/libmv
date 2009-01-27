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
//
// A simple implementation of levenberg marquardt.

#ifndef LIBMV_NUMERIC_LEVENBERG_MARQUARDT_H
#define LIBMV_NUMERIC_LEVENBERG_MARQUARDT_H

#include <cmath>

#include "libmv/numeric/numeric.h"
#include "libmv/numeric/function_derivative.h"
#include "third_party/glog/src/glog/logging.h"

namespace libmv {

// TODO(keir): Optional number of iterations, etc.
template<typename Function, typename Parameters>
Parameters LevenburgMarquardt(const Function &f, const Parameters &x0) {
  typedef typename Parameters::RealScalar Scalar;
  typedef typename Function::MatrixType FVec;
  typedef Matrix<typename Function::MatrixType::RealScalar,
                 Function::MatrixType::RowsAtCompileTime,
                 Parameters::RowsAtCompileTime> JMat;
  typedef Matrix<typename JMat::RealScalar, 
                 JMat::ColsAtCompileTime, 
                 JMat::ColsAtCompileTime> AMat;
  Parameters x = x0;
  JMat J = Jacobian(f, x0);
  AMat A = J.transpose() * J;
  FVec error = -f(x);
  Parameters g = J.transpose() * error;
  Scalar u = Scalar(100*A.diagonal().maxCoeff());
  Scalar e1 = Scalar(1e-16);
  Scalar e2 = e1;
  int v = 2;
  int iteration = 0;
  int max_iterations = 100;
  bool stop = g.cwise().abs().maxCoeff() < e2;

  Parameters dx, x_new;
  while (!stop && iteration < max_iterations) {
    iteration++;
    int inner_iteration = 0;
    LOG(INFO) << "iteration: " << iteration;
    LOG(INFO) << "||f(x)||: " << f(x).norm();
    LOG(INFO) << "max(g): " << g.cwise().abs().maxCoeff();
    LOG(INFO) << "u: " << u;
    LOG(INFO) << "v: " << v;
    while (1) {
      // The LDLT variant is failing in weird ways; find out why!
      AMat A_augmented =
          A + u*AMat::Identity(J.cols(), J.cols());
      // TODO(keir): Below line should work.
      //if (!A_augmented.ldlt().solve(g, &dx)) {
      //if (!A_augmented.lu().solve(g, &dx)) {
      if (!A_augmented.svd().solve(g, &dx)) {
        LOG(ERROR) << "Solving failed. A Matrix:\n" << A_augmented;
        LOG(ERROR) << "dx:\n" << g;
        return x;
      }
      LOG(INFO) << "x: " << x.transpose();
      LOG(INFO) << "dx: " << dx.transpose();
      if (dx.norm() <= e2 * x.norm()) {
        stop = true;
        break;
      } 
      x_new = x + dx;
      Scalar rho((error.norm2() - f(x_new).norm2()) / dx.dot(u*dx + g));
      if (rho > 0) {
        // Accept the step. Update gradients and error.
        x = x_new;
        J = Jacobian(f, x);
        A = J.transpose() * J;
        error = -f(x);
        g = J.transpose() * error;
        stop = g.cwise().abs().maxCoeff() < e2;
        Scalar tmp = Scalar(2*rho-1);
        u = u*std::max(1/3., 1 - (tmp*tmp*tmp));
        v = 2;
      } else {
        // Keep going in this direction.
        u *= v;
        v *= 2;
      }
      if (stop or rho > 0 or inner_iteration > 100) {
        break;
      }
    }
  }
  return x;
}

}  // namespace mv

#endif  // LIBMV_NUMERIC_LEVENBERG_MARQUARDT_H
