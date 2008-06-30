#include <flens/id.h>

namespace flens {

Id::Id(int dim)
    : _dim(dim)
{
}

int
Id::dim() const
{
    return _dim;
}

} // namespace flens







