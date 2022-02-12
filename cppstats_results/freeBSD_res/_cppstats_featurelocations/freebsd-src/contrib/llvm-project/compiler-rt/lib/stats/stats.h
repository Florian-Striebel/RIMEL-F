











#if !defined(SANITIZER_STATS_STATS_H)
#define SANITIZER_STATS_STATS_H

#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __sanitizer {




enum { kKindBits = 3 };

struct StatInfo {
uptr addr;
uptr data;
};

struct StatModule {
StatModule *next;
u32 size;
StatInfo infos[1];
};

inline uptr CountFromData(uptr data) {
return data & ((1ull << (sizeof(uptr) * 8 - kKindBits)) - 1);
}

}

#endif
