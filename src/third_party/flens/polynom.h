//TODO: COMPLETE REVIEW
#ifndef FLENS_POLYNOM_H
#define FLENS_POLYNOM_H 1

#include <flens/array.h>
#include <flens/densevector.h>

namespace flens {

template <typename T>
class Polynom
{
    public:
        Polynom(int n=0);

        Polynom(const DenseVector<Array<T> > &coefficients);

        const T &
        operator()(int n) const;

        T &
        operator()(int n);

        Polynom<T> &
        operator+=(const Polynom<T> &rhs);

        int
        degree() const;

    private:
        DenseVector<Array<T> > _coefficients;
};

template <typename T>
Polynom<T>
operator*(const Polynom<T> &lhs, const Polynom<T> &rhs);

template <typename S, typename T>
Polynom<T>
operator*(const S &lhs, const Polynom<T> &rhs);

template <typename T>
Polynom<T>
pow(const Polynom<T> &p, int n);

} // namespace flens

#include <flens/polynom.tcc>

#endif // FLENS_POLYNOM_H








