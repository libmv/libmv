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
// Add tiny matrix multiplication.

namespace flens {

template <typename T, int M, int N>
static inline T TransposedAccess(
  Transpose trans,
  const TinyGeMatrix<FixedSizeArray2D<T, M, N> > &A,
  int i, int j) {
  if (trans == NoTrans) {
    return A(i,j);
  } else if (trans == Trans) {
    return A(j,i);
  } else {
    bool not_implemented_yet = 0;
    (void) not_implemented_yet;
    assert(not_implemented_yet);
  }
  return T(0);
}

// GEMM
// C = alpha *A'*B' + beta*C
template <typename ALPHA, typename BETA,
          typename TA, int MA, int NA,
          typename TB, int MB, int NB,
          typename TC, int MC, int NC>
void
mm(Transpose transA, Transpose transB,
   ALPHA alpha,
   const TinyGeMatrix<FixedSizeArray2D<TA, MA, NA> > &A,
   const TinyGeMatrix<FixedSizeArray2D<TB, MB, NB> > &B,
   BETA beta, TinyGeMatrix<FixedSizeArray2D<TC, MC, NC> > &C) {
  // Conjugates not implemented yet.
  assert(transA == NoTrans || transA == Trans);
  assert(transB == NoTrans || transB == Trans); 
  // A mxn x B nxp = C mxp
  const int M = MC;
  const int N = (transA == NoTrans) ? NA : MA;
  if (transB == NoTrans) {
    assert(N == MB);
  } else {
    assert(N == NB);
  }
  const int P = NC;

#define loop(x, n) for (int x = 0; x < n; ++x)
  loop (i, M) {
    loop (j, P) {
      TC AikBkj = TC(0);
      loop (k, N) { 
        TA Axx = TransposedAccess(transA, A, i, k);
        TB Bxx = TransposedAccess(transB, B, k, j);
        AikBkj += Axx * Bxx;
      }
      C(i, j) = alpha*AikBkj + beta*C(i, j);
    }
  }
#undef loop
}
}  // namespace flens

