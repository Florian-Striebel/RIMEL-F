











#if !defined(TSAN_DEFS_H)
#define TSAN_DEFS_H

#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_libc.h"
#include "sanitizer_common/sanitizer_mutex.h"
#include "ubsan/ubsan_platform.h"


#if !defined(TSAN_NO_HISTORY)
#define TSAN_NO_HISTORY 0
#endif

#if !defined(TSAN_CONTAINS_UBSAN)
#if CAN_SANITIZE_UB && !SANITIZER_GO
#define TSAN_CONTAINS_UBSAN 1
#else
#define TSAN_CONTAINS_UBSAN 0
#endif
#endif

namespace __tsan {

const int kClkBits = 42;
const unsigned kMaxTidReuse = (1 << (64 - kClkBits)) - 1;

struct ClockElem {
u64 epoch : kClkBits;
u64 reused : 64 - kClkBits;
};

struct ClockBlock {
static const uptr kSize = 512;
static const uptr kTableSize = kSize / sizeof(u32);
static const uptr kClockCount = kSize / sizeof(ClockElem);
static const uptr kRefIdx = kTableSize - 1;
static const uptr kBlockIdx = kTableSize - 2;

union {
u32 table[kTableSize];
ClockElem clock[kClockCount];
};

ClockBlock() {
}
};

const int kTidBits = 13;



const unsigned kMaxTid = (1 << kTidBits) - ClockBlock::kClockCount;
#if !SANITIZER_GO
const unsigned kMaxTidInClock = kMaxTid * 2;
#else
const unsigned kMaxTidInClock = kMaxTid;
#endif
const uptr kShadowStackSize = 64 * 1024;


const uptr kShadowCnt = 4;


const uptr kShadowCell = 8;


const uptr kShadowSize = 8;


const uptr kShadowMultiplier = kShadowSize * kShadowCnt / kShadowCell;



const uptr kMetaShadowCell = 8;


const uptr kMetaShadowSize = 4;

#if TSAN_NO_HISTORY
const bool kCollectHistory = false;
#else
const bool kCollectHistory = true;
#endif




#if SANITIZER_DEBUG
void build_consistency_debug();
#else
void build_consistency_release();
#endif

static inline void USED build_consistency() {
#if SANITIZER_DEBUG
build_consistency_debug();
#else
build_consistency_release();
#endif
}

template<typename T>
T min(T a, T b) {
return a < b ? a : b;
}

template<typename T>
T max(T a, T b) {
return a > b ? a : b;
}

template<typename T>
T RoundUp(T p, u64 align) {
DCHECK_EQ(align & (align - 1), 0);
return (T)(((u64)p + align - 1) & ~(align - 1));
}

template<typename T>
T RoundDown(T p, u64 align) {
DCHECK_EQ(align & (align - 1), 0);
return (T)((u64)p & ~(align - 1));
}


template<typename T>
T GetLsb(T v, int bits) {
return (T)((u64)v & ((1ull << bits) - 1));
}

struct MD5Hash {
u64 hash[2];
bool operator==(const MD5Hash &other) const;
};

MD5Hash md5_hash(const void *data, uptr size);

struct Processor;
struct ThreadState;
class ThreadContext;
struct Context;
struct ReportStack;
class ReportDesc;
class RegionAlloc;


struct MBlock {
u64 siz : 48;
u64 tag : 16;
u32 stk;
u16 tid;
};

COMPILER_CHECK(sizeof(MBlock) == 16);

enum ExternalTag : uptr {
kExternalTagNone = 0,
kExternalTagSwiftModifyingAccess = 1,
kExternalTagFirstUserAvailable = 2,
kExternalTagMax = 1024,


};

enum MutexType {
MutexTypeTrace = MutexLastCommon,
MutexTypeReport,
MutexTypeSyncVar,
MutexTypeAnnotations,
MutexTypeAtExit,
MutexTypeFired,
MutexTypeRacy,
MutexTypeGlobalProc,
};

}

#endif
