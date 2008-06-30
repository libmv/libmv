#ifndef FLENS_BLAS_GENERIC_H
#define FLENS_BLAS_GENERIC_H 1

namespace flens {

//--- BLAS 2 -------------------------------------------------------------------

template <typename T>
    void
    copy(int N, const T *x, int incX, T *y, int incY);

template <typename T>
    void
    scal(int N, T alpha, T *X, int incX);

template <typename T>
    void
    axpy(int N, T alpha, const T *x, int incX, T *y, int incY);

template <typename T>
    T
    dot(int N, const T *x, int incX, const T *Y, int incY);
    
template <typename T>
    int
    amax(int N, const T *X, int incX);
    
template <typename T>
    T
    nrm2(int N, const T *X, int incX);

//--- BLAS 2 -------------------------------------------------------------------

template <typename T>
    void
    gemv(StorageOrder order, Transpose trans,
         int m, int n, T alpha, const T *A, int lda,
         const T *x, int incX,
         T beta, T *y, int incY);

//--- BLAS 3 -------------------------------------------------------------------

template <typename T>
    void
    gemm(StorageOrder Order, Transpose TransA, Transpose TransB,
         int m, int n, int k, T alpha,
         const T *a, int lda, const T *b, int ldb,
         T beta, T *c, int ldc);

} // namespace flens

#include <flens/blas_generic.tcc>

#endif // FLENS_BLAS_GENERIC_H
