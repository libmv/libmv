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

#ifndef LIBMV_NUMERIC_DERIVATIVE_H
#define LIBMV_NUMERIC_DERIVATIVE_H

#include <cmath>

#include "libmv/numeric/numeric.h"

namespace libmv {

// Numeric derivative of a function.

template<typename Function, typename Parameters>
Matrix<typename Function::MatrixType::RealScalar,
       Function::MatrixType::RowsAtCompileTime,
       Parameters::RowsAtCompileTime>
Jacobian(const Function &f, const Parameters &x) {
  typedef typename Parameters::RealScalar Scalar;
  typedef typename Function::MatrixType FMatrixType;
  typedef Matrix<typename Function::MatrixType::RealScalar,
                 Function::MatrixType::RowsAtCompileTime,
                 Parameters::RowsAtCompileTime> JMatrixType;

  // Empirically determined constant.
  Parameters eps = x.cwise().abs() * Scalar(1e-4);
  // To handle cases where a paremeter is exactly zero, instead use the mean
  // eps for the other dimensions.
  Scalar mean_eps = eps.sum() / eps.rows();
  if (mean_eps == Scalar(0)) {
    mean_eps = 1e-8; // ~sqrt(machine precision).
  }
  // TODO(keir): Elimininate this needless function evaluation.
  FMatrixType fx = f(x);
  int rows = fx.rows();
  int cols = x.rows();
  JMatrixType jacobian(rows, cols);
  Parameters x_plus_delta = x;
  for (int c = 0; c < cols; ++c) {
    if (eps(c) == Scalar(0)) {
      eps(c) = mean_eps;
    }
    x_plus_delta(c) = x(c) + eps(c);
    jacobian.col(c) = f(x_plus_delta);
    x_plus_delta(c) = x(c) - eps(c);
    jacobian.col(c) -= f(x_plus_delta);
    x_plus_delta(c) = x(c);
    jacobian.col(c) = jacobian.col(c) / (2*eps(c));
  }
  return jacobian;
}

}  // namespace libmv

#endif  // LIBMV_NUMERIC_DERIVATIVE_H
