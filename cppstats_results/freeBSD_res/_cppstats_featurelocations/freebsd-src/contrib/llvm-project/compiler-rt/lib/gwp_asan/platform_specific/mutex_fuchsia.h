







#if defined(__Fuchsia__)
#if !defined(GWP_ASAN_MUTEX_FUCHSIA_H_)
#define GWP_ASAN_MUTEX_FUCHSIA_H_

#include <lib/sync/mutex.h>

namespace gwp_asan {
class PlatformMutex {
protected:
sync_mutex_t Mu = {};
};
}

#endif
#endif
