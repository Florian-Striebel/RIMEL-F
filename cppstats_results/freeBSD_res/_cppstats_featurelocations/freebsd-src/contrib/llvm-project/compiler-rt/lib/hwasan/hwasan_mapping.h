












#if !defined(HWASAN_MAPPING_H)
#define HWASAN_MAPPING_H

#include "sanitizer_common/sanitizer_internal_defs.h"
#include "hwasan_interface_internal.h"


















constexpr uptr kShadowScale = 4;
constexpr uptr kShadowAlignment = 1ULL << kShadowScale;

namespace __hwasan {

extern uptr kLowMemStart;
extern uptr kLowMemEnd;
extern uptr kLowShadowEnd;
extern uptr kLowShadowStart;
extern uptr kHighShadowStart;
extern uptr kHighShadowEnd;
extern uptr kHighMemStart;
extern uptr kHighMemEnd;

inline uptr GetShadowOffset() {
return SANITIZER_FUCHSIA ? 0 : __hwasan_shadow_memory_dynamic_address;
}
inline uptr MemToShadow(uptr untagged_addr) {
return (untagged_addr >> kShadowScale) + GetShadowOffset();
}
inline uptr ShadowToMem(uptr shadow_addr) {
return (shadow_addr - GetShadowOffset()) << kShadowScale;
}
inline uptr MemToShadowSize(uptr size) {
return size >> kShadowScale;
}

bool MemIsApp(uptr p);

inline bool MemIsShadow(uptr p) {
return (kLowShadowStart <= p && p <= kLowShadowEnd) ||
(kHighShadowStart <= p && p <= kHighShadowEnd);
}

uptr GetAliasRegionStart();

}

#endif
