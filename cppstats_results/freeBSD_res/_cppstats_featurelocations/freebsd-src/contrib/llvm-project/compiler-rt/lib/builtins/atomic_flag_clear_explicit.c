











#if !defined(__has_include)
#define __has_include(inc) 0
#endif

#if __has_include(<stdatomic.h>)

#include <stdatomic.h>
#undef atomic_flag_clear_explicit
void atomic_flag_clear_explicit(volatile atomic_flag *object,
memory_order order) {
__c11_atomic_store(&(object)->_Value, 0, order);
}

#endif
