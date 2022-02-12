







#if defined(__Fuchsia__)
#if !defined(GWP_ASAN_GUARDED_POOL_ALLOCATOR_FUCHSIA_H_)
#define GWP_ASAN_GUARDED_POOL_ALLOCATOR_FUCHSIA_H_

#include <zircon/types.h>

namespace gwp_asan {
struct PlatformSpecificMapData {
zx_handle_t Vmar;
};
}

#endif
#endif
