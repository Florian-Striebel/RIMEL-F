







#if !defined(SCUDO_MUTEX_H_)
#define SCUDO_MUTEX_H_

#include "atomic_helpers.h"
#include "common.h"

#include <string.h>

#if SCUDO_FUCHSIA
#include <lib/sync/mutex.h>
#endif

namespace scudo {

class HybridMutex {
public:
bool tryLock();
NOINLINE void lock() {
if (LIKELY(tryLock()))
return;




#if defined(__clang__)
#pragma nounroll
#endif
for (u8 I = 0U; I < NumberOfTries; I++) {
yieldProcessor(NumberOfYields);
if (tryLock())
return;
}
lockSlow();
}
void unlock();

private:
static constexpr u8 NumberOfTries = 8U;
static constexpr u8 NumberOfYields = 8U;

#if SCUDO_LINUX
atomic_u32 M = {};
#elif SCUDO_FUCHSIA
sync_mutex_t M = {};
#endif

void lockSlow();
};

class ScopedLock {
public:
explicit ScopedLock(HybridMutex &M) : Mutex(M) { Mutex.lock(); }
~ScopedLock() { Mutex.unlock(); }

private:
HybridMutex &Mutex;

ScopedLock(const ScopedLock &) = delete;
void operator=(const ScopedLock &) = delete;
};

}

#endif
