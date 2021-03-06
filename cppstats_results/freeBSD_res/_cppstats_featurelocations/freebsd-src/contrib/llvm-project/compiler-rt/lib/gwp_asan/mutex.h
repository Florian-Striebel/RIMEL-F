







#if !defined(GWP_ASAN_MUTEX_H_)
#define GWP_ASAN_MUTEX_H_

#include "gwp_asan/platform_specific/mutex_fuchsia.h"
#include "gwp_asan/platform_specific/mutex_posix.h"

namespace gwp_asan {
class Mutex final : PlatformMutex {
public:
constexpr Mutex() = default;
~Mutex() = default;
Mutex(const Mutex &) = delete;
Mutex &operator=(const Mutex &) = delete;

void lock();

bool tryLock();

void unlock();
};

class ScopedLock {
public:
explicit ScopedLock(Mutex &Mx) : Mu(Mx) { Mu.lock(); }
~ScopedLock() { Mu.unlock(); }
ScopedLock(const ScopedLock &) = delete;
ScopedLock &operator=(const ScopedLock &) = delete;

private:
Mutex &Mu;
};
}

#endif
