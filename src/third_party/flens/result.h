#ifndef FLENS_RESULT_H
#define FLENS_RESULT_H 1

namespace flens {

template <typename A>
struct Result
{
    typedef A Type;
};

/*
 * Result of a matrix or vector:
 * 1) get derived matrix/vector type
 * 2) get result type of derived type
 */

template <typename Impl>
class Matrix;

template <typename Impl>
class Vector;

template <typename I>
struct Result<Matrix<I> >
{
    typedef typename TypeInfo<Matrix<I> >::Impl Impl;
    typedef typename Result<Impl>::Type Type;
};

template <typename I>
struct Result<Vector<I> >
{
    typedef typename TypeInfo<Vector<I> >::Impl Impl;
    typedef typename Result<Impl>::Type Type;
};

/*
 * Result of closures:
 * 1) get result of arguments
 * 2) get result of underlying operation
 */

template <typename Op, typename L, typename R>
class MatrixClosure;

template <typename Op, typename L, typename R>
class VectorClosure;

template <typename Op, typename A, typename B>
struct Result<MatrixClosure<Op, A, B> >
{
    typedef typename Result<A>::Type L;
    typedef typename Result<B>::Type R;

    typedef typename Result<MatrixClosure<Op, L, R> >::Type Type;
};

template <typename Op, typename A, typename B>
struct Result<VectorClosure<Op, A, B> >
{
    typedef typename Result<A>::Type L;
    typedef typename Result<B>::Type R;

    typedef typename Result<VectorClosure<Op, L, R> >::Type Type;
};

} // namespace flens

#endif // FLENS_RESULT_H
