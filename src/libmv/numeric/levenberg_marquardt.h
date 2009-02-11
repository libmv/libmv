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
template<typename Function,
         typename Jacobian = NumericJacobian<Function>,
         typename Solver = Eigen::SVD<
           Matrix<typename Function::FMatrixType::RealScalar, 
                  Function::XMatrixType::RowsAtCompileTime,
                  Function::XMatrixType::RowsAtCompileTime> > >
class LevenbergMarquardt {
 public:
  typedef typename Function::XMatrixType::RealScalar Scalar;
  typedef typename Function::FMatrixType FVec;
  typedef typename Function::XMatrixType Parameters;
  typedef Matrix<typename Function::FMatrixType::RealScalar,
                 Function::FMatrixType::RowsAtCompileTime,
                 Function::XMatrixType::RowsAtCompileTime> JMatrixType;
  typedef Matrix<typename JMatrixType::RealScalar, 
                 JMatrixType::ColsAtCompileTime, 
                 JMatrixType::ColsAtCompileTime> AMatrixType;

  // TODO(keir): Some of these knobs can be derived from each other and
  // removed, instead of requiring the user to set them.
  enum Status {
    NOT_STARTED,
    RUNNING,
    GRADIENT_TOO_SMALL,            // eps > max(J'*f(x))
    RELATIVE_STEP_SIZE_TOO_SMALL,  // eps > ||dx|| / ||x||
    ERROR_TOO_SMALL,               // eps > ||f(x)||
    HIT_MAX_ITERATIONS,
    SINGULAR_NORMAL_EQUATIONS,     // Can't solve J'J*dx = J'e
  };

  LevenbergMarquardt(const Function &f)
      : f_(f), df_(f),
        status_(NOT_STARTED),
        gradient_threshold_(1e-16),
        relative_step_threshold_(1e-16),
        error_threshold_(1e-16) {}

  Status Update(const Parameters &x,
                JMatrixType *J, AMatrixType *A, FVec *error, Parameters *g) {
    *J = df_(x);
    *A = (*J).transpose() * (*J);
    *error = -f_(x);
    *g = (*J).transpose() * *error;
    return g->cwise().abs().maxCoeff() < gradient_threshold_
           ? GRADIENT_TOO_SMALL : RUNNING;
  }

  Parameters operator()(const Parameters &x0) {
    Parameters x;
    JMatrixType J;
    AMatrixType A;
    FVec error;
    Parameters g;

    int max_iterations = 100;  // XXX make parameter.

    x = x0;
    status_ = Update(x, &J, &A, &error, &g);

    Scalar initial_scale_factor = 100;  // XXX make parameter.
    Scalar u = Scalar(initial_scale_factor*A.diagonal().maxCoeff());
    Scalar v = 2;

    Parameters dx, x_new;
    for (int i = 0; status_ == RUNNING && i < max_iterations; ++i) {
      LOG(INFO) << "iteration: " << i;
      LOG(INFO) << "||f(x)||: " << f_(x).norm();
      LOG(INFO) << "max(g): " << g.cwise().abs().maxCoeff();
      LOG(INFO) << "u: " << u;
      LOG(INFO) << "v: " << v;

      AMatrixType A_augmented = A + u*AMatrixType::Identity(J.cols(), J.cols());
      Solver solver(A);
      bool solved = solver.solve(g, &dx);
      if (!solved) LOG(ERROR) << "Failed to solve";
      if (solved && dx.norm() <= gradient_threshold_ * x.norm()) {
          status_ = RELATIVE_STEP_SIZE_TOO_SMALL;
          break;
      } 
      if (solved) {
        x_new = x + dx;
        // Rho is the ratio of the actual reduction in error to the reduction
        // in error that would be obtained if the problem was linear.
        Scalar rho((error.norm2() - f_(x_new).norm2()) / dx.dot(u*dx + g));
        if (rho > 0) {
          // Accept the Gauss-Newton step because the linear model fits well.
          x = x_new;
          status_ = Update(x, &J, &A, &error, &g);
          Scalar tmp = Scalar(2*rho-1);
          u = u*std::max(1/3., 1 - (tmp*tmp*tmp));
          v *= 2;
          continue;
        } 
      } 
      // Reject the update because either the normal equations failed to solve
      // or the local linear model was not good (rho < 0). Instead, increase u
      // to move closer to gradient descent.
      u *= v;
      v *= 2;
    }
    return x;
  }

  Status status() {
    return status_;
  }
 private:
  const Function &f_;
  Jacobian df_;
  Status status_;

  // Solver parameters. These should really be in a parameters class.
  Scalar gradient_threshold_;       // eps > max(J'*f(x))
  Scalar relative_step_threshold_;  // eps > ||dx|| / ||x||
  Scalar error_threshold_;          // eps > ||f(x)||
};

}  // namespace mv

#endif  // LIBMV_NUMERIC_LEVENBERG_MARQUARDT_H
