#ifndef FLENS_HACKSFORGMP_H
#define FLENS_HACKSFORGMP_H 1

#include <memory>

#ifdef GMP
#include <gmp/gmpfrxx.h>
#endif

namespace flens {

template <typename T>
struct Initializer
{
};

//-- Array ---------------------------------------------------------------------

template <typename T>
class Array;

template <typename T>
struct Initializer<Array<T> >
{
    static void
    initialize(Array<T> &a)
    {
        std::uninitialized_fill_n(a.data(), a.length(), T());
    }
};

#ifdef GMP
template <typename T, typename U>
struct Initializer<Array<__gmp_expr<T,U> > >
{
    static void
    initialize(Array<__gmp_expr<T,U> > &a)
    {
        std::uninitialized_fill_n(a.data(), a.length(), __gmp_expr<T,U>(0));
    }
};
#endif // GMP

//-- FullStorage ---------------------------------------------------------------

template <typename T, StorageOrder Order>
class FullStorage;

template <typename T, StorageOrder Order>
struct Initializer<FullStorage<T, Order> >
{
    static void
    initialize(FullStorage<T, Order> &fs)
    {
        std::uninitialized_fill_n(fs.data(), fs.numRows()*fs.numCols(), T());
    }
};

#ifdef GMP
template <typename T, typename U, StorageOrder Order>
struct Initializer<FullStorage<__gmp_expr<T,U>, Order> >
{
    static void
    initialize(FullStorage<__gmp_expr<T,U>, Order> &fs)
    {
        std::uninitialized_fill_n(fs.data(), fs.numRows()*fs.numCols(),
                                 __gmp_expr<T,U>(0));
    }
};
#endif

} // namespace flens

#endif // FLENS_HACKSFORGMP_H
