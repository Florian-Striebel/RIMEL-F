











#if !defined(HWASAN_POISONING_H)
#define HWASAN_POISONING_H

#include "hwasan.h"

namespace __hwasan {
uptr TagMemory(uptr p, uptr size, tag_t tag);
uptr TagMemoryAligned(uptr p, uptr size, tag_t tag);

}

#endif
