











#if !defined(__has_include)
#define __has_include(inc) 0
#endif

#if __has_include(<stdatomic.h>)

#include <stdatomic.h>
#undef atomic_flag_test_and_set_explicit
_Bool atomic_flag_test_and_set_explicit(volatile atomic_flag *object,
memory_order order) {
return __c11_atomic_exchange(&(object)->_Value, 1, order);
}

#endif
