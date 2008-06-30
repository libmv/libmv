#include <cmath>

namespace flens {
    
template <typename T>
const T
SIGN(const T &a, const T&b)
{
    if (b>=0) {
        if (a>=0) {
            return a;
        } else { 
            return -a;
        }
    } else {
        if (a>=0) {
            return -a;
        } else { 
            return a;
        }
    }
}

template <typename MX, typename VX, typename VY>
int
qrf(GeMatrix<MX> &A, DenseVector<VX> &c,
    DenseVector<VY> &d, bool &singular)
{
    typedef typename MX::ElementType T;
    int n = A.numRows();
    c.resize(n,0);
    d.resize(n,0);
    singular = false;
    for (int k=0; k<n-1; ++k) {
        T scale = 0.;
        for (int i=k; i<n; ++i) {
            T tmp = absolute(A(i,k));
            scale = max(scale, tmp);
        }
        if (scale==0.) {
            singular = true;
            c(k) = d(k) = 0.;
        } else {
            for (int i=k; i<n; ++i) {
                A(i,k) /= scale;
            }
            T sum;
            int i;
            for (sum=0., i=k; i<n; ++i) {
                sum += A(i,k)*A(i,k);
            }
            T tmp = sqrt(sum);
            T sigma = SIGN(tmp, A(k,k));
            A(k,k) += sigma;
            c(k) = sigma*A(k,k);
            d(k) = -scale*sigma;
            for (int j=k+1; j<n; ++j) {
                for (sum=0., i=k; i<n; ++i) {
                    sum += A(i,k) * A(i,j);
                }
                T tau = sum / c(k);
                for (i=k; i<n; ++i) {
                    A(i,j) -= tau*A(i,k);
                }
            }
        }
    }
    d(n-1) = A(n-1,n-1);
    if (d(n-1)==0.) {
        singular = true;
    }
    return singular;
}

template <typename MX, typename VX, typename VY>
void
rsolv(GeMatrix<MX> &A,
      DenseVector<VX> &d, DenseVector<VY> &b)
{
    typedef typename MX::ElementType T;

    int n = A.numRows();
    b(n-1) /= d(n-1);
    for (int i=n-2; i>=0; --i) {
        T sum;
        int j;
        for (sum=0.,j=i+1; j<n; ++j) {
            sum += A(i,j)*b(j);
        }
        b(i) = (b(i)-sum) / d(i);
    }
}


template <typename MX, typename VX, typename VY, typename VZ>
void
qrsolv(GeMatrix<MX> &A, DenseVector<VX> &c,
       DenseVector<VY> &d, DenseVector<VZ> &b)
{
    typedef typename MX::ElementType T;
    int n = A.numRows();
    for (int j=0; j<n-1; ++j) {
        T sum;
        int i;
        for (sum=0., i=j; i<n; ++i) {
            sum += A(i,j) * b(i);
        }
        T tau = sum / c(j);
        for (i=j; i<n; ++i) {
            b(i) -= tau*A(i,j);
        }
    }
    rsolv(A,d,b);
}

template <typename T>
void
qrinv(GeMatrix<FullStorage<T,ColMajor> > &A, 
      GeMatrix<FullStorage<T,ColMajor> > &InvA)
{
    bool singular;
    DenseVector<Array<T> > c, d;
    qrf(A,c,d,singular);
    assert(!singular);
    int n=A.numRows();
    InvA.resize(n,n,0,0);
    DenseVector<Array<T> > b(n,0);
    for (int i=0; i<n; ++i) {
        b(i) = 1.;
        qrsolv(A,c,d,b);
        InvA(_,i) = b;
        b = T(0.);
    }
}

template <typename T>
const T
SQR(const T a)
{
    return a*a;
}

template <typename T>
T
pythag(const T a, const T b)
{
    T absa, absb;
    absa = absolute(a);
    absb = absolute(b);
    if (absa>absb) {
        return absa*sqrt(1+SQR(T(absb/absa)));
    } else {
        return (absb == T(0) ? T(0) : absb*sqrt(1+SQR(T(absa/absb))));
    }
}

template <typename T, StorageOrder Order>
void
svdGMP(const GeMatrix<FullStorage<T,Order> > &A,
    DenseVector<Array<T> > &s,
    GeMatrix<FullStorage<T,Order> > &U,
    GeMatrix<FullStorage<T,Order> > &V)
{
    bool flag;
    int i, its, j, jj, k, l, nm;
    T anorm, c, f, g, h, tmp, scale, x, y, z;
    
    U = A;
    int m = U.numRows(),
        n = U.numCols();
    s.resize(std::min(m,n));
    V.resize(n,n);
    DenseVector<Array<T> > rv1(n);
    g = scale = anorm = 0;
    for (i=1; i<=n; ++i) {
        l = i+1;
        rv1(i) = scale*g;
        g = tmp = scale = 0;
        if (i<=m) {
            for (k=i; k<=m; ++k) {
                scale += absolute(U(k,i));
            }
            if (scale!=0) {
                for (k=i; k<=m; ++k) {
                    U(k,i) /= scale;
                    tmp += U(k,i) * U(k,i);
                }
                f = U(i,i);
                g = -SIGN(T(sqrt(tmp)),f);
                h = f*g-tmp;
                U(i,i) = f-g;
                for (j=l; j<=n; ++j) {
                    for (tmp=0, k=i; k<=m; ++k) {
                        tmp += U(k,i) * U(k,j);
                    }
                    f = tmp/h;
                    for (k=i; k<=m; ++k) {
                        U(k,j) += f*U(k,i); 
                    }
                }
                for (k=i; k<=m; ++k) {
                    U(k,i) *= scale;
                }
            }
        }
        s(i) = scale *g;
        g = tmp = scale = 0;
        if ( (i<=m) && (i!=n) ) {
            for (k=l; k<=n; ++k) {
                scale += absolute(U(i,k));
            }
            if (scale!=0) {
                for (k=l; k<=n; ++k) {
                    U(i,k) /= scale;
                    tmp += U(i,k) * U(i,k);
                }
                f = U(i,l);
                g = -SIGN(T(sqrt(tmp)),f);
                h = f*g-tmp;
                U(i,l) = f-g;
                for (k=l; k<=n; ++k) {
                    rv1(k) = U(i,k) / h;
                }
                for (j=l; j<=m; ++j) {
                    for (tmp=0,k=l; k<=n; ++k) {
                        tmp += U(j,k) * U(i,k);
                    }
                    for (k=l; k<=n; ++k) {
                        U(j,k) += tmp*rv1(k); 
                    }
                }
                for (k=l; k<=n; ++k) {
                    U(i,k) *= scale;
                }
            }
        }
        anorm = std::max(anorm,T(absolute(s(i))+absolute(rv1(i))));
    }
    for (i=n; i>=1; --i) {
        if (i<n) {
            if (g!=0) {
                for (j=l; j<=n; ++j) {
                    V(j,i) = (U(i,j)/U(i,l)) / g;
                }
                for (j=l; j<=n; ++j) {
                    for (tmp=0, k=l; k<=n; ++k) {
                        tmp += U(i,k)*V(k,j);
                    }
                    for (k=l; k<=n; ++k) {
                        V(k,j) += tmp*V(k,i);
                    }
                }
            }
            for (j=l; j<=n; ++j) {
                V(i,j) = V(j,i) = 0;
            }
        }
        V(i,i) = 1;
        g = rv1(i);
        l = i;
    }
    for (i=std::min(m,n); i>=1; --i) {
        l = i+1;
        g = s(i);
        for (j=l; j<=n; ++j) {
            U(i,j) = 0;
        }
        if (g!=0) {
            g = 1/g;
            for (j=l; j<=n; ++j) {
                for (tmp=0, k=l; k<=m; ++k) {
                    tmp += U(k,i) * U(k,j);
                }
                f = (tmp/U(i,i))*g;
                for (k=i; k<=m; ++k) {
                    U(k,j) += f * U(k,i);
                }
            }
            for (j=i; j<=m; ++j) {
                U(j,i) *= g;
            }
        } else {
            for (j=i; j<=m; ++j) {
                U(j,i) = 0;
            }
        }
        ++U(i,i);
    }
    for (k=n; k>=1; --k) {
        for (its=1; its<=30; ++its) {
            flag = true;
            for (l=k; l>=1; --l) {
                nm = l-1;
                if (absolute(rv1(l))+anorm==anorm) {
                    flag = false;
                    break;
                }
                if (absolute(s(nm))+anorm==anorm) {
                    break;
                }
            }
            if (flag) {
                c = 0;
                tmp = 1;
                for (i=l; i<=k; ++i) {
                    f = tmp*rv1(i);
                    rv1(i) = c * rv1(i);
                    if (absolute(f)+anorm==anorm) {
                        break;
                    }
                    g = s(i);
                    h = pythag(f,g);
                    s(i) = h;
                    h = 1./h;
                    c = g*h;
                    tmp = -f*h;
                    for (j=1; j<=m; ++j) {
                        y = U(j,nm);
                        z = U(j,i);
                        U(j,nm) = y*c + z*tmp;
                        U(j,i) = z*c - y*tmp;
                    }
                }
            }
            z = s(k);
            if (l==k) {
                if (z<0) {
                    s(k) = -z;
                    for (j=1; j<=n; ++j) {
                        V(j,k) = -V(j,k);
                    }
                }
                break;
            }
            if (its==30) {
                std::cerr << "no convergence in 30 SVD-iterations." << std::endl;
                //exit(0);
//                return;
            }
            x = s(l);
            nm = k-1;
            y = s(nm);
            g = rv1(nm);
            h = rv1(k);
            f = ((y-z)*(y+z)+(g-h)*(g+h)) / (2*h*y);
            g = pythag(f,T(1.));
            f = ((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
            c = tmp = 1;
            for (j=l; j<=nm; ++j) {
                i = j+1;
                g = rv1(i);
                y = s(i);
                h = tmp*g;
                g = c*g;
                z = pythag(f,h);
                rv1(j) = z;
                c = f/z;
                tmp = h/z;
                f = x*c+g*tmp;
                g = g*c-x*tmp;
                h = y*tmp;
                y *= c;
                for (jj=1; jj<=n; ++jj) {
                    x = V(jj,j);
                    z = V(jj,i);
                    V(jj,j) = x*c+z*tmp;
                    V(jj,i) = z*c-x*tmp;
                }
                z = pythag(f,h);
                s(j) = z;
                if (z!=0) {
                    z = 1/z;
                    c = f*z;
                    tmp = h*z;
                }
                f = c*g+tmp*y;
                x = c*y-tmp*g;
                for (jj=1; jj<=m; ++jj) {
                    y = U(jj,j);
                    z = U(jj,i);
                    U(jj,j) = y*c + z*tmp;
                    U(jj,i) = z*c - y*tmp;
                }
            }
            rv1(l) = 0;
            rv1(k) = f;
            s(k) = x;
        }
    }
    // TODO: incorporate transpose in algorithm.
    GeMatrix<FullStorage<T,ColMajor> > Dummy;
    Dummy = transpose(V);
    V = Dummy;
}

} // namespace flens
