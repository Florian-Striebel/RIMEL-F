













#if !defined(SCUDO_TSD_H_)
#define SCUDO_TSD_H_

#include "scudo_allocator.h"
#include "scudo_utils.h"

#include <pthread.h>

namespace __scudo {

struct ALIGNED(SANITIZER_CACHE_LINE_SIZE) ScudoTSD {
AllocatorCacheT Cache;
uptr QuarantineCachePlaceHolder[4];

void init();
void commitBack();

inline bool tryLock() TRY_ACQUIRE(true, Mutex) {
if (Mutex.TryLock()) {
atomic_store_relaxed(&Precedence, 0);
return true;
}
if (atomic_load_relaxed(&Precedence) == 0)
atomic_store_relaxed(&Precedence, static_cast<uptr>(
MonotonicNanoTime() >> FIRST_32_SECOND_64(16, 0)));
return false;
}

inline void lock() ACQUIRE(Mutex) {
atomic_store_relaxed(&Precedence, 0);
Mutex.Lock();
}

inline void unlock() RELEASE(Mutex) { Mutex.Unlock(); }

inline uptr getPrecedence() { return atomic_load_relaxed(&Precedence); }

private:
StaticSpinMutex Mutex;
atomic_uintptr_t Precedence;
};

void initThread(bool MinimalInit);


#include "scudo_tsd_exclusive.inc"
#include "scudo_tsd_shared.inc"

}

#endif
