










#if !defined(SANITIZER_FUCHSIA_H)
#define SANITIZER_FUCHSIA_H

#include "sanitizer_platform.h"
#if SANITIZER_FUCHSIA

#include "sanitizer_common.h"

#include <zircon/sanitizer.h>
#include <zircon/syscalls/object.h>

namespace __sanitizer {

extern uptr MainThreadStackBase, MainThreadStackSize;
extern sanitizer_shadow_bounds_t ShadowBounds;

struct MemoryMappingLayoutData {
InternalMmapVector<zx_info_maps_t> data;
size_t current;
};

void InitShadowBounds();

}

#endif
#endif
