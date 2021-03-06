












#if !defined(DFSAN_PLATFORM_H)
#define DFSAN_PLATFORM_H

#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_platform.h"

namespace __dfsan {

using __sanitizer::uptr;




struct MappingDesc {
uptr start;
uptr end;
enum Type { INVALID, APP, SHADOW, ORIGIN } type;
const char *name;
};

#if SANITIZER_LINUX && SANITIZER_WORDSIZE == 64






const MappingDesc kMemoryLayout[] = {
{0x000000000000ULL, 0x010000000000ULL, MappingDesc::APP, "app-1"},
{0x010000000000ULL, 0x100000000000ULL, MappingDesc::SHADOW, "shadow-2"},
{0x100000000000ULL, 0x110000000000ULL, MappingDesc::INVALID, "invalid"},
{0x110000000000ULL, 0x200000000000ULL, MappingDesc::ORIGIN, "origin-2"},
{0x200000000000ULL, 0x300000000000ULL, MappingDesc::SHADOW, "shadow-3"},
{0x300000000000ULL, 0x400000000000ULL, MappingDesc::ORIGIN, "origin-3"},
{0x400000000000ULL, 0x500000000000ULL, MappingDesc::INVALID, "invalid"},
{0x500000000000ULL, 0x510000000000ULL, MappingDesc::SHADOW, "shadow-1"},
{0x510000000000ULL, 0x600000000000ULL, MappingDesc::APP, "app-2"},
{0x600000000000ULL, 0x610000000000ULL, MappingDesc::ORIGIN, "origin-1"},
{0x610000000000ULL, 0x700000000000ULL, MappingDesc::INVALID, "invalid"},
{0x700000000000ULL, 0x800000000000ULL, MappingDesc::APP, "app-3"}};
#define MEM_TO_SHADOW(mem) (((uptr)(mem)) ^ 0x500000000000ULL)
#define SHADOW_TO_ORIGIN(mem) (((uptr)(mem)) + 0x100000000000ULL)

#else
#error "Unsupported platform"
#endif

const uptr kMemoryLayoutSize = sizeof(kMemoryLayout) / sizeof(kMemoryLayout[0]);

#define MEM_TO_ORIGIN(mem) (SHADOW_TO_ORIGIN(MEM_TO_SHADOW((mem))))

#if !defined(__clang__)
__attribute__((optimize("unroll-loops")))
#endif
inline bool
addr_is_type(uptr addr, MappingDesc::Type mapping_type) {


#if defined(__clang__)
#pragma unroll
#endif
for (unsigned i = 0; i < kMemoryLayoutSize; ++i)
if (kMemoryLayout[i].type == mapping_type &&
addr >= kMemoryLayout[i].start && addr < kMemoryLayout[i].end)
return true;
return false;
}

#define MEM_IS_APP(mem) addr_is_type((uptr)(mem), MappingDesc::APP)
#define MEM_IS_SHADOW(mem) addr_is_type((uptr)(mem), MappingDesc::SHADOW)
#define MEM_IS_ORIGIN(mem) addr_is_type((uptr)(mem), MappingDesc::ORIGIN)

}

#endif
