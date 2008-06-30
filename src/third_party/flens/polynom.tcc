#include <flens/underscore.h>

//TODO: COMPLETE REVIEW
namespace flens {

template <typename T>
Polynom<T>::Polynom(int n)
    : _coefficients(DenseVector<Array<T> >(_(0,n)))
{
}

template <typename T>
Polynom<T>::Polynom(const DenseVector<Array<T> > &coefficients)
    : _coefficients(coefficients)
{
}


template <typename T>
const T &
Polynom<T>::operator()(int n) const
{
    assert(0<=n);
    assert(n<=this->degree());
    return _coefficients(n);
}

template <typename T>
T &
Polynom<T>::operator()(int n)
{
    assert(0<=n);
    assert(n<=this->degree());
    return _coefficients(n);
}

template <typename T>
int
Polynom<T>::degree() const
{
    return _coefficients.lastIndex();
}

template <typename T>
Polynom<T> &
Polynom<T>::operator+=(const Polynom<T> &rhs)
{
    if (this->degree()>=rhs.degree()) {
        for (int i=0; i<=rhs.degree(); ++i) {
            _coefficients(i) += rhs._coefficients(i);
        }
    } else {
        DenseVector<Array<T> > tmp = rhs._coefficients;
        for (int i=0; i<=this->degree(); ++i) {
            tmp(i) += _coefficients(i);
        }
        this->_coefficients = tmp;
    }
    return *this;
}

template <typename T>
Polynom<T>
operator*(const Polynom<T> &lhs, const Polynom<T> &rhs)
{
    int degree = lhs.degree() + rhs.degree();

    Polynom<T> res(degree);
    for (int i=0; i<=lhs.degree(); i++) {
        for (int j=0; j<=rhs.degree(); j++) {
            res(i+j) += lhs(i)*rhs(j);
        }
    }
    return res;
}

template <typename S, typename T>
Polynom<T>
operator*(const S &lhs, const Polynom<T> &rhs)
{
    Polynom<T> res(rhs);
    for (int i=0; i<=rhs.degree(); ++i) {
        res(i) *= lhs;
    }
    return res;
}

template <typename T>
Polynom<T>
pow(const Polynom<T> &p, int n)
{
    if (!n) {
        Polynom<T> res(0);
        res(0) = 1.;
        return res;
    }
    Polynom<T> res(p);
    for (int k=1; k<n; ++k) {
        res = res*p;
    }
    return res;
}

template <typename T>
std::ostream &
operator<<(std::ostream &out, const Polynom<T> &rhs)
{
    out << "(";
    for (int i=0; i<=rhs.degree(); ++i) {
        out << rhs(i) << " ";
    }
    out << ")";
    return out;
}

} // namespace flens








