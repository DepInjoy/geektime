#include "Add.h"

#ifdef DECLARE_IMPL_SEPARATION
template<typename T>
T Add(T t1, T t2) {
    return t1 + t2;
}
#endif