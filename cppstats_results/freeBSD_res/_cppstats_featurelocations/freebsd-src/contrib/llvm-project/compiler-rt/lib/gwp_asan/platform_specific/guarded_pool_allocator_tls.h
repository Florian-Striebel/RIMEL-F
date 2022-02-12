







#if !defined(GWP_ASAN_GUARDED_POOL_ALLOCATOR_TLS_H_)
#define GWP_ASAN_GUARDED_POOL_ALLOCATOR_TLS_H_

#include "gwp_asan/definitions.h"

#include <stdint.h>

namespace gwp_asan {



struct ThreadLocalPackedVariables {
constexpr ThreadLocalPackedVariables()
: RandomState(0xacd979ce), NextSampleCounter(0), RecursiveGuard(false) {}




uint32_t RandomState;


uint32_t NextSampleCounter : 31;

static const uint32_t NextSampleCounterMask = (1U << 31) - 1;





bool RecursiveGuard : 1;
};
static_assert(sizeof(ThreadLocalPackedVariables) == sizeof(uint64_t),
"thread local data does not fit in a uint64_t");
}

#if defined(GWP_ASAN_PLATFORM_TLS_HEADER)
#include GWP_ASAN_PLATFORM_TLS_HEADER
#else
namespace gwp_asan {
inline ThreadLocalPackedVariables *getThreadLocals() {
alignas(8) static GWP_ASAN_TLS_INITIAL_EXEC ThreadLocalPackedVariables Locals;
return &Locals;
}
}
#endif

#endif
