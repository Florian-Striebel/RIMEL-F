







#if defined(__unix__)
#if !defined(GWP_ASAN_MUTEX_POSIX_H_)
#define GWP_ASAN_MUTEX_POSIX_H_

#include <pthread.h>

namespace gwp_asan {
class PlatformMutex {
protected:
pthread_mutex_t Mu = PTHREAD_MUTEX_INITIALIZER;
};
}

#endif
#endif
