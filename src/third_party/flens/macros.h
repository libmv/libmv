#include <cassert>

#define ADDRESS(x) reinterpret_cast<const void *>(&x)

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif //ASSERT
