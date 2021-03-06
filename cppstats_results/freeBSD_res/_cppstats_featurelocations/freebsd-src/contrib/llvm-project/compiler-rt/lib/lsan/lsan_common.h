












#if !defined(LSAN_COMMON_H)
#define LSAN_COMMON_H

#include "sanitizer_common/sanitizer_allocator.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_platform.h"
#include "sanitizer_common/sanitizer_stoptheworld.h"
#include "sanitizer_common/sanitizer_symbolizer.h"











#if SANITIZER_ANDROID && (__ANDROID_API__ < 28 || defined(__arm__))
#define CAN_SANITIZE_LEAKS 0
#elif (SANITIZER_LINUX || SANITIZER_MAC) && (SANITIZER_WORDSIZE == 64) && (defined(__x86_64__) || defined(__mips64) || defined(__aarch64__) || defined(__powerpc64__) || defined(__s390x__))


#define CAN_SANITIZE_LEAKS 1
#elif defined(__i386__) && (SANITIZER_LINUX || SANITIZER_MAC)
#define CAN_SANITIZE_LEAKS 1
#elif defined(__arm__) && SANITIZER_LINUX
#define CAN_SANITIZE_LEAKS 1
#elif SANITIZER_RISCV64 && SANITIZER_LINUX
#define CAN_SANITIZE_LEAKS 1
#elif SANITIZER_NETBSD || SANITIZER_FUCHSIA
#define CAN_SANITIZE_LEAKS 1
#else
#define CAN_SANITIZE_LEAKS 0
#endif

namespace __sanitizer {
class FlagParser;
class ThreadRegistry;
class ThreadContextBase;
struct DTLS;
}

namespace __lsan {


enum ChunkTag {
kDirectlyLeaked = 0,
kIndirectlyLeaked = 1,
kReachable = 2,
kIgnored = 3
};

struct Flags {
#define LSAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "lsan_flags.inc"
#undef LSAN_FLAG

void SetDefaults();
uptr pointer_alignment() const {
return use_unaligned ? 1 : sizeof(uptr);
}
};

extern Flags lsan_flags;
inline Flags *flags() { return &lsan_flags; }
void RegisterLsanFlags(FlagParser *parser, Flags *f);

struct Leak {
u32 id;
uptr hit_count;
uptr total_size;
u32 stack_trace_id;
bool is_directly_leaked;
bool is_suppressed;
};

struct LeakedObject {
u32 leak_id;
uptr addr;
uptr size;
};


class LeakReport {
public:
LeakReport() {}
void AddLeakedChunk(uptr chunk, u32 stack_trace_id, uptr leaked_size,
ChunkTag tag);
void ReportTopLeaks(uptr max_leaks);
void PrintSummary();
uptr ApplySuppressions();
uptr UnsuppressedLeakCount();
uptr IndirectUnsuppressedLeakCount();

private:
void PrintReportForLeak(uptr index);
void PrintLeakedObjectsForLeak(uptr index);

u32 next_id_ = 0;
InternalMmapVector<Leak> leaks_;
InternalMmapVector<LeakedObject> leaked_objects_;
};

typedef InternalMmapVector<uptr> Frontier;


void InitializePlatformSpecificModules();
void ProcessGlobalRegions(Frontier *frontier);
void ProcessPlatformSpecificAllocations(Frontier *frontier);

struct RootRegion {
uptr begin;
uptr size;
};





struct CheckForLeaksParam {
Frontier frontier;
LeakReport leak_report;
bool success = false;
};

InternalMmapVector<RootRegion> const *GetRootRegions();
void ScanRootRegion(Frontier *frontier, RootRegion const &region,
uptr region_begin, uptr region_end, bool is_readable);
void ForEachExtraStackRangeCb(uptr begin, uptr end, void* arg);
void GetAdditionalThreadContextPtrs(ThreadContextBase *tctx, void *ptrs);


void LockStuffAndStopTheWorld(StopTheWorldCallback callback,
CheckForLeaksParam* argument);

void ScanRangeForPointers(uptr begin, uptr end,
Frontier *frontier,
const char *region_type, ChunkTag tag);
void ScanGlobalRange(uptr begin, uptr end, Frontier *frontier);

enum IgnoreObjectResult {
kIgnoreObjectSuccess,
kIgnoreObjectAlreadyIgnored,
kIgnoreObjectInvalid
};


const char *MaybeCallLsanDefaultOptions();
void InitCommonLsan();
void DoLeakCheck();
void DoRecoverableLeakCheckVoid();
void DisableCounterUnderflow();
bool DisabledInThisThread();


void DisableInThisThread();
void EnableInThisThread();


struct ScopedInterceptorDisabler {
ScopedInterceptorDisabler() { DisableInThisThread(); }
~ScopedInterceptorDisabler() { EnableInThisThread(); }
};



static inline bool IsItaniumABIArrayCookie(uptr chunk_beg, uptr chunk_size,
uptr addr) {
return chunk_size == sizeof(uptr) && chunk_beg + chunk_size == addr &&
*reinterpret_cast<uptr *>(chunk_beg) == 0;
}






static inline bool IsARMABIArrayCookie(uptr chunk_beg, uptr chunk_size,
uptr addr) {
return chunk_size == 2 * sizeof(uptr) && chunk_beg + chunk_size == addr &&
*reinterpret_cast<uptr *>(chunk_beg + sizeof(uptr)) == 0;
}





inline bool IsSpecialCaseOfOperatorNew0(uptr chunk_beg, uptr chunk_size,
uptr addr) {
#if defined(__arm__)
return IsARMABIArrayCookie(chunk_beg, chunk_size, addr);
#else
return IsItaniumABIArrayCookie(chunk_beg, chunk_size, addr);
#endif
}



void ForEachChunk(ForEachChunkCallback callback, void *arg);

void GetAllocatorGlobalRange(uptr *begin, uptr *end);

void LockAllocator();
void UnlockAllocator();

bool WordIsPoisoned(uptr addr);

void LockThreadRegistry() NO_THREAD_SAFETY_ANALYSIS;
void UnlockThreadRegistry() NO_THREAD_SAFETY_ANALYSIS;
ThreadRegistry *GetThreadRegistryLocked();
bool GetThreadRangesLocked(tid_t os_id, uptr *stack_begin, uptr *stack_end,
uptr *tls_begin, uptr *tls_end, uptr *cache_begin,
uptr *cache_end, DTLS **dtls);
void GetAllThreadAllocatorCachesLocked(InternalMmapVector<uptr> *caches);
void ForEachExtraStackRange(tid_t os_id, RangeIteratorCallback callback,
void *arg);






void EnsureMainThreadIDIsCorrect();


uptr PointsIntoChunk(void *p);

uptr GetUserBegin(uptr chunk);

IgnoreObjectResult IgnoreObjectLocked(const void *p);


LoadedModule *GetLinker();


bool HasReportedLeaks();


void HandleLeaks();


class LsanMetadata {
public:

explicit LsanMetadata(uptr chunk);
bool allocated() const;
ChunkTag tag() const;
void set_tag(ChunkTag value);
uptr requested_size() const;
u32 stack_trace_id() const;
private:
void *metadata_;
};

}

extern "C" {
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
const char *__lsan_default_options();

SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
int __lsan_is_turned_off();

SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
const char *__lsan_default_suppressions();
}

#endif
