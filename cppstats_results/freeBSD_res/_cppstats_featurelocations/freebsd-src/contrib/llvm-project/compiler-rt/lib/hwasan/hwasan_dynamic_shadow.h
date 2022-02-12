













#if !defined(HWASAN_PREMAP_SHADOW_H)
#define HWASAN_PREMAP_SHADOW_H

#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __hwasan {

uptr FindDynamicShadowStart(uptr shadow_size_bytes);
void InitShadowGOT();

}

#endif
