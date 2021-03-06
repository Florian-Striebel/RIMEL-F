







#if !defined(SCUDO_TSD_H_)
#define SCUDO_TSD_H_

#include "atomic_helpers.h"
#include "common.h"
#include "mutex.h"

#include <limits.h>
#include <pthread.h>


#if !defined(PTHREAD_DESTRUCTOR_ITERATIONS)
#define PTHREAD_DESTRUCTOR_ITERATIONS 4
#endif

namespace scudo {

template <class Allocator> struct alignas(SCUDO_CACHE_LINE_SIZE) TSD {
typename Allocator::CacheT Cache;
typename Allocator::QuarantineCacheT QuarantineCache;
using ThisT = TSD<Allocator>;
u8 DestructorIterations = 0;

void init(Allocator *Instance) {
DCHECK_EQ(DestructorIterations, 0U);
DCHECK(isAligned(reinterpret_cast<uptr>(this), alignof(ThisT)));
Instance->initCache(&Cache);
DestructorIterations = PTHREAD_DESTRUCTOR_ITERATIONS;
}

void commitBack(Allocator *Instance) { Instance->commitBack(this); }

inline bool tryLock() {
if (Mutex.tryLock()) {
atomic_store_relaxed(&Precedence, 0);
return true;
}
if (atomic_load_relaxed(&Precedence) == 0)
atomic_store_relaxed(
&Precedence,
static_cast<uptr>(getMonotonicTime() >> FIRST_32_SECOND_64(16, 0)));
return false;
}
inline void lock() {
atomic_store_relaxed(&Precedence, 0);
Mutex.lock();
}
inline void unlock() { Mutex.unlock(); }
inline uptr getPrecedence() { return atomic_load_relaxed(&Precedence); }

private:
HybridMutex Mutex;
atomic_uptr Precedence = {};
};

}

#endif
