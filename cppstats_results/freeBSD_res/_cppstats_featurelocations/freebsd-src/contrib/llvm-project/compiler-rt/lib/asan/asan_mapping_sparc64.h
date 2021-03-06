











#if !defined(ASAN_MAPPING_SPARC64_H)
#define ASAN_MAPPING_SPARC64_H







#define VMA_BITS 52
#define HIGH_BITS (64 - VMA_BITS)




#define MEM_TO_SHADOW(mem) ((((mem) << HIGH_BITS) >> (HIGH_BITS + (SHADOW_SCALE))) + (SHADOW_OFFSET))


#define kLowMemBeg 0
#define kLowMemEnd (SHADOW_OFFSET - 1)

#define kLowShadowBeg SHADOW_OFFSET
#define kLowShadowEnd MEM_TO_SHADOW(kLowMemEnd)




#define kHighMemBeg (-(1ULL << (VMA_BITS - 1)))

#define kHighShadowBeg MEM_TO_SHADOW(kHighMemBeg)
#define kHighShadowEnd MEM_TO_SHADOW(kHighMemEnd)

#define kMidShadowBeg 0
#define kMidShadowEnd 0



#define kZeroBaseShadowStart 0
#define kZeroBaseMaxShadowStart (1 << 18)

#define kShadowGapBeg (kLowShadowEnd + 1)
#define kShadowGapEnd (kHighShadowBeg - 1)

#define kShadowGap2Beg 0
#define kShadowGap2End 0

#define kShadowGap3Beg 0
#define kShadowGap3End 0

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
return false;
}

static inline bool AddrIsInMidShadow(uptr a) {
PROFILE_ASAN_MAPPING();
return false;
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
return a >= kShadowGapBeg && a <= kShadowGapEnd;
}

}

#endif
