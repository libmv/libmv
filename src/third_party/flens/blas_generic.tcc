#include <cassert>
#include <iostream>

namespace flens {

template <typename T>
void
copy(int N, const T *x, int incX, T *y, int incY)
{
    for (int i=0; i<N; ++i, x+=incX, y+=incY) {
        (*y) = (*x);
    }
}

template <typename T>
void
scal(int N, T alpha, T *x, int incX)
{
    for (int i=0; i<N; ++i, x+=incX) {
        (*x) *= alpha;
    }
}

template <typename T>
void
axpy(int N, T alpha, const T *x, int incX, T *y, int incY)
{
    for (int i=0; i<N; ++i, x+=incX, y+=incY) {
        (*y) += alpha * (*x);
    }
}

template <typename T>
T
dot(int N, const T *x, int incX, const T *y, int incY)
{
    T result = T(0);
    for (int i=0; i<N; ++i, x+=incX, y+=incY) {
        result += (*x) * (*y);
    }
    return result;
}

template <typename T>
T
nrm2(int N, const T *x, int incX)
{
    T result = T(0);
    for (int i=0; i<N; ++i, x+=incX) {
        result += (*x) * (*x);
    }
    return sqrt(result);
}

template <typename T>
int
amax(int N, const T *X, int incX)
{
    int pos = 0;
    const T *max = X;
    for (int i=1; i<N; ++i) {
        if (absolute(*(X+=incX))>absolute(*max)) {
            max = X;
            pos = i;
        }
    }
    return pos;
}

//--- BLAS 2 -------------------------------------------------------------------

template <typename T>
void
gemv(StorageOrder order, Transpose trans,
     int m, int n, T alpha, const T *A, int lda,
     const T *x, int incX,
     T beta, T *y, int incY)
{
    if ((trans==NoTrans) && (order==ColMajor)) {
        for (int i=0; i<m; ++i) {
            y[i*incY] = alpha*dot(n, A+i, lda, x, incX) + beta*y[i*incY];
        }
        return;
    }
    if((trans==Trans) && (order==ColMajor)) {
        for (int i=0; i<m; ++i) {
            y[i*incY] = alpha*dot(n, A+i, 1, x, incX) + beta*y[i*incY];
        }
        return;
    }
    assert(0);
}


//--- BLAS 3 -------------------------------------------------------------------

template <typename T>
void
gemm(StorageOrder Order,
     Transpose TransA, Transpose TransB,
     int m, int n, int k, T alpha,
     const T *a, int lda,
     const T *b, int ldb,
     T beta, T *c, int ldc)
{
    int i1, i2, i3, nota, notb, i, j, jb, jc, l, la;
    T temp;

    nota = (TransA==NoTrans);
    notb = (TransB==NoTrans);

    if (m == 0 || n == 0 || (alpha == 0. || k == 0) && beta == 1.) {
        return;
    }
    
    if (alpha==0.) {
        if (beta==0.) {
            i1 = n;
            for (j=0; j<i1; ++j) {
                jc = j*ldc;
                i2 = m;
                for(i = 0; i < i2; ++i){
                    c[i + jc] = 0.;
                }
            }
        } else {
            i1 = n;
            for (j=0;j<i1; ++j) {
                jc = j*ldc;
                i2 = m;
                for(i = 0; i < i2; ++i){
                    c[i + jc] = beta * c[i + jc];
                }
            }
        }
        return;
    }

    if (!a) {
        if (notb) {   // C := alpha*B + beta*C. 
            i1 = n;
            for (j=0; j<i1; ++j) {
                jb = j*ldb;
                jc = j*ldc;
                i2 = m;
                for(i=0; i<i2; ++i){
                    c[i + jc] = alpha*b[i+jb] + beta*c[i+jc];
                }
            }
        } else {   // C := alpha*B' + beta*C. 
            i1 = n;
            for (j=0; j<i1; ++j){
                jc = j*ldc;
                i2 = m;
                for (i=0; i<i2; ++i){
                    c[i + jc] = alpha*b[j+i*ldb] + beta*c[i+jc];
                }
            }
        }
        return;
    }

    if (notb) {
        if (nota) {
            //          Form  C := alpha*A*B + beta*C. 
            i1 = n;
            for (j=0; j<i1; ++j) {
                jc = j*ldc;
                if(beta == 0.) {
                    i2 = m;
                    for(i=0; i<i2; ++i) {
                        c[i + jc] = 0.;
                    }
                } else if (beta!=1.) {
                    i2 = m;
                    for (i=0; i<i2; ++i) {
                        c[i + jc] = beta * c[i + jc];
                    }
                }
                i2 = k;
                for(l=0; l<i2; ++l) {
                    la = l*lda;
                    if (b[l + j*ldb]!=0.) {
                        temp = alpha * b[l + j*ldb];
                        i3 = m;
                        for (i=0; i<i3; ++i){
                            c[i + jc] += temp * a[i + la];
                        }
                    }
                }
            }
        } else {
            //          Form  C := alpha*A'*B + beta*C 
            i1 = n;
            for (j=0; j<i1; ++j) {
                jc = j*ldc;
                i2 = m;
                for (i=0; i<i2; ++i) {
                    temp = 0.;
                    i3 = k;
                    for(l=0; l<i3; ++l) {
                        temp += a[l + i*lda] * b[l + j*ldb];
                    }
                    if (beta==0.) {
                        c[i + jc] = alpha * temp;
                    } else {
                        c[i + jc] = alpha * temp + beta * c[i + jc];
                    }
                }
            }
        }
    } else {
        if (nota) {
            //          Form  C := alpha*A*B' + beta*C 
            i1 = n;
            for (j=0; j<i1; ++j){
                jc = j*ldc;
                if (beta==0.) {
                    i2 = m;
                    for(i=0; i<i2; ++i) {
                        c[i + jc] = 0.;
                    }
                } else if(beta != 1.) {
                    i2 = m;
                    for (i=0; i<i2; ++i) {
                        c[i + jc] = beta * c[i + jc];
                    }
                }
                i2 = k;
                for (l=0; l<i2; ++l) {
                    if (b[j + l*ldb]!=0.) {
                        temp = alpha * b[j + l*ldb];
                        i3 = m;
                        for (i=0; i<i3; ++i) {
                            c[i + jc] += temp * a[i + l*lda];
                        }
                    }
                }
            }
        } else {
            //          Form  C := alpha*A'*B' + beta*C 
            i1 = n;
            for (j=0; j<i1; ++j) {
                jc = j*ldc;
                i2 = m;
                for (i=0; i<i2; ++i) {
                    temp = 0.;
                    i3 = k;
                    for (l=0; l<i3; ++l) {
                        temp += a[l + i*lda] * b[j + l*ldb];
                    }
                    if (beta==0.) {
                        c[i + jc] = alpha * temp;
                    } else {
                        c[i + jc] = alpha * temp + beta * c[i + jc];
                    }
                }
            }
        }
    }
}

} // namespace flens
