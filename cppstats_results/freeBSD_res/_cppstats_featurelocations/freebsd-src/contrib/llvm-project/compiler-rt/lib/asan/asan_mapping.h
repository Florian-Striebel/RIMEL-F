











#if !defined(ASAN_MAPPING_H)
#define ASAN_MAPPING_H

#include "asan_internal.h"









































































































































#if defined(ASAN_SHADOW_SCALE)
static const u64 kDefaultShadowScale = ASAN_SHADOW_SCALE;
#else
static const u64 kDefaultShadowScale = 3;
#endif
static const u64 kDefaultShadowSentinel = ~(uptr)0;
static const u64 kDefaultShadowOffset32 = 1ULL << 29;
static const u64 kDefaultShadowOffset64 = 1ULL << 44;
static const u64 kDefaultShort64bitShadowOffset =
0x7FFFFFFF & (~0xFFFULL << kDefaultShadowScale);
static const u64 kAArch64_ShadowOffset64 = 1ULL << 36;
static const u64 kRiscv64_ShadowOffset64 = 0xd55550000;
static const u64 kMIPS32_ShadowOffset32 = 0x0aaa0000;
static const u64 kMIPS64_ShadowOffset64 = 1ULL << 37;
static const u64 kPPC64_ShadowOffset64 = 1ULL << 44;
static const u64 kSystemZ_ShadowOffset64 = 1ULL << 52;
static const u64 kSPARC64_ShadowOffset64 = 1ULL << 43;
static const u64 kFreeBSD_ShadowOffset32 = 1ULL << 30;
static const u64 kFreeBSD_ShadowOffset64 = 1ULL << 46;
static const u64 kNetBSD_ShadowOffset32 = 1ULL << 30;
static const u64 kNetBSD_ShadowOffset64 = 1ULL << 46;
static const u64 kWindowsShadowOffset32 = 3ULL << 28;

#define SHADOW_SCALE kDefaultShadowScale

#if SANITIZER_FUCHSIA
#define SHADOW_OFFSET (0)
#elif SANITIZER_WORDSIZE == 32
#if SANITIZER_ANDROID
#define SHADOW_OFFSET __asan_shadow_memory_dynamic_address
#elif defined(__mips__)
#define SHADOW_OFFSET kMIPS32_ShadowOffset32
#elif SANITIZER_FREEBSD
#define SHADOW_OFFSET kFreeBSD_ShadowOffset32
#elif SANITIZER_NETBSD
#define SHADOW_OFFSET kNetBSD_ShadowOffset32
#elif SANITIZER_WINDOWS
#define SHADOW_OFFSET kWindowsShadowOffset32
#elif SANITIZER_IOS
#define SHADOW_OFFSET __asan_shadow_memory_dynamic_address
#else
#define SHADOW_OFFSET kDefaultShadowOffset32
#endif
#else
#if SANITIZER_IOS
#define SHADOW_OFFSET __asan_shadow_memory_dynamic_address
#elif SANITIZER_MAC && defined(__aarch64__)
#define SHADOW_OFFSET __asan_shadow_memory_dynamic_address
#elif SANITIZER_RISCV64
#define SHADOW_OFFSET kRiscv64_ShadowOffset64
#elif defined(__aarch64__)
#define SHADOW_OFFSET kAArch64_ShadowOffset64
#elif defined(__powerpc64__)
#define SHADOW_OFFSET kPPC64_ShadowOffset64
#elif defined(__s390x__)
#define SHADOW_OFFSET kSystemZ_ShadowOffset64
#elif SANITIZER_FREEBSD
#define SHADOW_OFFSET kFreeBSD_ShadowOffset64
#elif SANITIZER_NETBSD
#define SHADOW_OFFSET kNetBSD_ShadowOffset64
#elif SANITIZER_MAC
#define SHADOW_OFFSET kDefaultShadowOffset64
#elif defined(__mips64)
#define SHADOW_OFFSET kMIPS64_ShadowOffset64
#elif defined(__sparc__)
#define SHADOW_OFFSET kSPARC64_ShadowOffset64
#elif SANITIZER_WINDOWS64
#define SHADOW_OFFSET __asan_shadow_memory_dynamic_address
#else
#define SHADOW_OFFSET kDefaultShort64bitShadowOffset
#endif
#endif

#if SANITIZER_ANDROID && defined(__arm__)
#define ASAN_PREMAP_SHADOW 1
#else
#define ASAN_PREMAP_SHADOW 0
#endif

#define SHADOW_GRANULARITY (1ULL << SHADOW_SCALE)

#define DO_ASAN_MAPPING_PROFILE 0

#if DO_ASAN_MAPPING_PROFILE
#define PROFILE_ASAN_MAPPING() AsanMappingProfile[__LINE__]++;
#else
#define PROFILE_ASAN_MAPPING()
#endif



#define ASAN_FIXED_MAPPING 0

namespace __asan {

extern uptr AsanMappingProfile[];

#if ASAN_FIXED_MAPPING



static uptr kHighMemEnd = 0x7fffffffffffULL;
static uptr kMidMemBeg = 0x3000000000ULL;
static uptr kMidMemEnd = 0x4fffffffffULL;
#else
extern uptr kHighMemEnd, kMidMemBeg, kMidMemEnd;
#endif

}

#if defined(__sparc__) && SANITIZER_WORDSIZE == 64
#include "asan_mapping_sparc64.h"
#else
#define MEM_TO_SHADOW(mem) (((mem) >> SHADOW_SCALE) + (SHADOW_OFFSET))

#define kLowMemBeg 0
#define kLowMemEnd (SHADOW_OFFSET ? SHADOW_OFFSET - 1 : 0)

#define kLowShadowBeg SHADOW_OFFSET
#define kLowShadowEnd MEM_TO_SHADOW(kLowMemEnd)

#define kHighMemBeg (MEM_TO_SHADOW(kHighMemEnd) + 1)

#define kHighShadowBeg MEM_TO_SHADOW(kHighMemBeg)
#define kHighShadowEnd MEM_TO_SHADOW(kHighMemEnd)

#define kMidShadowBeg MEM_TO_SHADOW(kMidMemBeg)
#define kMidShadowEnd MEM_TO_SHADOW(kMidMemEnd)



#define kZeroBaseShadowStart 0
#define kZeroBaseMaxShadowStart (1 << 18)

#define kShadowGapBeg (kLowShadowEnd ? kLowShadowEnd + 1 : kZeroBaseShadowStart)

#define kShadowGapEnd ((kMidMemBeg ? kMidShadowBeg : kHighShadowBeg) - 1)

#define kShadowGap2Beg (kMidMemBeg ? kMidShadowEnd + 1 : 0)
#define kShadowGap2End (kMidMemBeg ? kMidMemBeg - 1 : 0)

#define kShadowGap3Beg (kMidMemBeg ? kMidMemEnd + 1 : 0)
#define kShadowGap3End (kMidMemBeg ? kHighShadowBeg - 1 : 0)

namespace __asan {

static inline bool AddrIsInLowMem(uptr a) {
PROFILE_ASAN_MAPPING();
return a <= kLowMemEnd;
}

static inline bool AddrIsInLowShadow(uptr a) {
PROFILE_ASAN_MAPPING();
return a >= kLowShadowBeg && a <= kLowShadowEnd;
}

static inline bool AddrIsInMidMem(uptr a) {
PROFILE_ASAN_MAPPING();
return kMidMemBeg && a >= kMidMemBeg && a <= kMidMemEnd;
}

static inline bool AddrIsInMidShadow(uptr a) {
PROFILE_ASAN_MAPPING();
return kMidMemBeg && a >= kMidShadowBeg && a <= kMidShadowEnd;
}

static inline bool AddrIsInHighMem(uptr a) {
PROFILE_ASAN_MAPPING();
return kHighMemBeg && a >= kHighMemBeg && a <= kHighMemEnd;
}

static inline bool AddrIsInHighShadow(uptr a) {
PROFILE_ASAN_MAPPING();
return kHighMemBeg && a >= kHighShadowBeg && a <= kHighShadowEnd;
}

static inline bool AddrIsInShadowGap(uptr a) {
PROFILE_ASAN_MAPPING();
if (kMidMemBeg) {
if (a <= kShadowGapEnd)
return SHADOW_OFFSET == 0 || a >= kShadowGapBeg;
return (a >= kShadowGap2Beg && a <= kShadowGap2End) ||
(a >= kShadowGap3Beg && a <= kShadowGap3End);
}


if (SHADOW_OFFSET == 0)
return a <= kShadowGapEnd;
return a >= kShadowGapBeg && a <= kShadowGapEnd;
}

}

#endif

namespace __asan {

static inline uptr MemToShadowSize(uptr size) { return size >> SHADOW_SCALE; }

static inline bool AddrIsInMem(uptr a) {
PROFILE_ASAN_MAPPING();
return AddrIsInLowMem(a) || AddrIsInMidMem(a) || AddrIsInHighMem(a) ||
(flags()->protect_shadow_gap == 0 && AddrIsInShadowGap(a));
}

static inline uptr MemToShadow(uptr p) {
PROFILE_ASAN_MAPPING();
CHECK(AddrIsInMem(p));
return MEM_TO_SHADOW(p);
}

static inline bool AddrIsInShadow(uptr a) {
PROFILE_ASAN_MAPPING();
return AddrIsInLowShadow(a) || AddrIsInMidShadow(a) || AddrIsInHighShadow(a);
}

static inline bool AddrIsAlignedByGranularity(uptr a) {
PROFILE_ASAN_MAPPING();
return (a & (SHADOW_GRANULARITY - 1)) == 0;
}

static inline bool AddressIsPoisoned(uptr a) {
PROFILE_ASAN_MAPPING();
const uptr kAccessSize = 1;
u8 *shadow_address = (u8*)MEM_TO_SHADOW(a);
s8 shadow_value = *shadow_address;
if (shadow_value) {
u8 last_accessed_byte = (a & (SHADOW_GRANULARITY - 1))
+ kAccessSize - 1;
return (last_accessed_byte >= shadow_value);
}
return false;
}


static const uptr kAsanMappingProfileSize = __LINE__;

}

#endif
