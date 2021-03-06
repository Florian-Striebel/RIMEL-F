











#if !defined(MEMPROF_MAPPING_H)
#define MEMPROF_MAPPING_H

#include "memprof_internal.h"

static const u64 kDefaultShadowScale = 3;
#define SHADOW_SCALE kDefaultShadowScale

#define SHADOW_OFFSET __memprof_shadow_memory_dynamic_address

#define SHADOW_GRANULARITY (1ULL << SHADOW_SCALE)
#define MEMPROF_ALIGNMENT 32

namespace __memprof {

extern uptr kHighMemEnd;

}

#define SHADOW_ENTRY_SIZE 8


#define MEM_GRANULARITY 64ULL

#define SHADOW_MASK ~(MEM_GRANULARITY - 1)

#define MEM_TO_SHADOW(mem) ((((mem) & SHADOW_MASK) >> SHADOW_SCALE) + (SHADOW_OFFSET))


#define kLowMemBeg 0
#define kLowMemEnd (SHADOW_OFFSET ? SHADOW_OFFSET - 1 : 0)

#define kLowShadowBeg SHADOW_OFFSET
#define kLowShadowEnd (MEM_TO_SHADOW(kLowMemEnd) + SHADOW_ENTRY_SIZE - 1)

#define kHighMemBeg (MEM_TO_SHADOW(kHighMemEnd) + 1 + SHADOW_ENTRY_SIZE - 1)

#define kHighShadowBeg MEM_TO_SHADOW(kHighMemBeg)
#define kHighShadowEnd (MEM_TO_SHADOW(kHighMemEnd) + SHADOW_ENTRY_SIZE - 1)



#define kZeroBaseShadowStart 0
#define kZeroBaseMaxShadowStart (1 << 18)

#define kShadowGapBeg (kLowShadowEnd ? kLowShadowEnd + 1 : kZeroBaseShadowStart)
#define kShadowGapEnd (kHighShadowBeg - 1)

namespace __memprof {

inline uptr MemToShadowSize(uptr size) { return size >> SHADOW_SCALE; }
inline bool AddrIsInLowMem(uptr a) { return a <= kLowMemEnd; }

inline bool AddrIsInLowShadow(uptr a) {
return a >= kLowShadowBeg && a <= kLowShadowEnd;
}

inline bool AddrIsInHighMem(uptr a) {
return kHighMemBeg && a >= kHighMemBeg && a <= kHighMemEnd;
}

inline bool AddrIsInHighShadow(uptr a) {
return kHighMemBeg && a >= kHighShadowBeg && a <= kHighShadowEnd;
}

inline bool AddrIsInShadowGap(uptr a) {


if (SHADOW_OFFSET == 0)
return a <= kShadowGapEnd;
return a >= kShadowGapBeg && a <= kShadowGapEnd;
}

inline bool AddrIsInMem(uptr a) {
return AddrIsInLowMem(a) || AddrIsInHighMem(a) ||
(flags()->protect_shadow_gap == 0 && AddrIsInShadowGap(a));
}

inline uptr MemToShadow(uptr p) {
CHECK(AddrIsInMem(p));
return MEM_TO_SHADOW(p);
}

inline bool AddrIsInShadow(uptr a) {
return AddrIsInLowShadow(a) || AddrIsInHighShadow(a);
}

inline bool AddrIsAlignedByGranularity(uptr a) {
return (a & (SHADOW_GRANULARITY - 1)) == 0;
}

inline void RecordAccess(uptr a) {

CHECK_EQ(SHADOW_ENTRY_SIZE, 8);
u64 *shadow_address = (u64 *)MEM_TO_SHADOW(a);
(*shadow_address)++;
}

}

#endif
