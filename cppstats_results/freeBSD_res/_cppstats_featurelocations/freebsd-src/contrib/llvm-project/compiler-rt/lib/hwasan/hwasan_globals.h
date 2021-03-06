












#if !defined(HWASAN_GLOBALS_H)
#define HWASAN_GLOBALS_H

#include <link.h>

#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __hwasan {


struct hwasan_global {


uptr size() const { return info & 0xffffff; }

uptr addr() const { return reinterpret_cast<uintptr_t>(this) + gv_relptr; }

u8 tag() const { return info >> 24; };



s32 gv_relptr;
u32 info;
};




ArrayRef<const hwasan_global> HwasanGlobalsFor(ElfW(Addr) base,
const ElfW(Phdr) * phdr,
ElfW(Half) phnum);

}

#endif
