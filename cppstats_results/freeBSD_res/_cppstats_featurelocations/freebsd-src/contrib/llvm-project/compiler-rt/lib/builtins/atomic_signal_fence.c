











#if !defined(__has_include)
#define __has_include(inc) 0
#endif

#if __has_include(<stdatomic.h>)

#include <stdatomic.h>
#undef atomic_signal_fence
void atomic_signal_fence(memory_order order) {
__c11_atomic_signal_fence(order);
}

#endif
