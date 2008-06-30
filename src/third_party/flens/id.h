#ifndef FLENS_ID_H
#define FLENS_ID_H 1

#include <flens/matvec.h>

namespace flens {

class Id;

template <>
struct TypeInfo<Id>
{
    typedef Id Impl;
    typedef void ElementType;
};

class Id
    : public Matrix<Id>
{
    public:
        typedef void ElementType;
        typedef void T;

        Id(int dim);

        int
        dim() const;

    private:
        int _dim;
};

} // namespace flens

#endif // FLENS_ID_H







