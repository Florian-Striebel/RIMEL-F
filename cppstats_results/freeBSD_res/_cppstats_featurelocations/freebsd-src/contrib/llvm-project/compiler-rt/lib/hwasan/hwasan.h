












#if !defined(HWASAN_H)
#define HWASAN_H

#include "hwasan_flags.h"
#include "hwasan_interface_internal.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_flags.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_stacktrace.h"
#include "ubsan/ubsan_platform.h"

#if !defined(HWASAN_CONTAINS_UBSAN)
#define HWASAN_CONTAINS_UBSAN CAN_SANITIZE_UB
#endif

#if !defined(HWASAN_WITH_INTERCEPTORS)
#define HWASAN_WITH_INTERCEPTORS 0
#endif

#if !defined(HWASAN_REPLACE_OPERATORS_NEW_AND_DELETE)
#define HWASAN_REPLACE_OPERATORS_NEW_AND_DELETE HWASAN_WITH_INTERCEPTORS
#endif

typedef u8 tag_t;

#if defined(HWASAN_ALIASING_MODE)
#if !defined(__x86_64__)
#error Aliasing mode is only supported on x86_64
#endif

constexpr unsigned kAddressTagShift = 39;
constexpr unsigned kTagBits = 3;







constexpr unsigned kTaggableRegionCheckShift =
__sanitizer::Max(kAddressTagShift + kTagBits + 1U, 44U);
#elif defined(__x86_64__)

constexpr unsigned kAddressTagShift = 57;
constexpr unsigned kTagBits = 6;
#else


constexpr unsigned kAddressTagShift = 56;
constexpr unsigned kTagBits = 8;
#endif


constexpr uptr kTagMask = (1UL << kTagBits) - 1;


constexpr uptr kAddressTagMask = kTagMask << kAddressTagShift;



const unsigned kShadowBaseAlignment = 32;

const unsigned kRecordAddrBaseTagShift = 3;
const unsigned kRecordFPShift = 48;
const unsigned kRecordFPLShift = 4;
const unsigned kRecordFPModulus = 1 << (64 - kRecordFPShift + kRecordFPLShift);

static inline tag_t GetTagFromPointer(uptr p) {
return (p >> kAddressTagShift) & kTagMask;
}

static inline uptr UntagAddr(uptr tagged_addr) {
return tagged_addr & ~kAddressTagMask;
}

static inline void *UntagPtr(const void *tagged_ptr) {
return reinterpret_cast<void *>(
UntagAddr(reinterpret_cast<uptr>(tagged_ptr)));
}

static inline uptr AddTagToPointer(uptr p, tag_t tag) {
return (p & ~kAddressTagMask) | ((uptr)tag << kAddressTagShift);
}

namespace __hwasan {

extern int hwasan_inited;
extern bool hwasan_init_is_running;
extern int hwasan_report_count;

bool InitShadow();
void InitializeOsSupport();
void InitThreads();
void InitializeInterceptors();

void HwasanAllocatorInit();

void *hwasan_malloc(uptr size, StackTrace *stack);
void *hwasan_calloc(uptr nmemb, uptr size, StackTrace *stack);
void *hwasan_realloc(void *ptr, uptr size, StackTrace *stack);
void *hwasan_reallocarray(void *ptr, uptr nmemb, uptr size, StackTrace *stack);
void *hwasan_valloc(uptr size, StackTrace *stack);
void *hwasan_pvalloc(uptr size, StackTrace *stack);
void *hwasan_aligned_alloc(uptr alignment, uptr size, StackTrace *stack);
void *hwasan_memalign(uptr alignment, uptr size, StackTrace *stack);
int hwasan_posix_memalign(void **memptr, uptr alignment, uptr size,
StackTrace *stack);
void hwasan_free(void *ptr, StackTrace *stack);

void InstallAtExitHandler();

#define GET_MALLOC_STACK_TRACE BufferedStackTrace stack; if (hwasan_inited) stack.Unwind(StackTrace::GetCurrentPc(), GET_CURRENT_FRAME(), nullptr, common_flags()->fast_unwind_on_malloc, common_flags()->malloc_context_size)






#define GET_FATAL_STACK_TRACE_PC_BP(pc, bp) BufferedStackTrace stack; if (hwasan_inited) stack.Unwind(pc, bp, nullptr, common_flags()->fast_unwind_on_fatal)




void HwasanTSDInit();
void HwasanTSDThreadInit();
void HwasanAtExit();

void HwasanOnDeadlySignal(int signo, void *info, void *context);

void UpdateMemoryUsage();

void AppendToErrorMessageBuffer(const char *buffer);

void AndroidTestTlsSlot();



struct AccessInfo {
uptr addr;
uptr size;
bool is_store;
bool is_load;
bool recover;
};



void HandleTagMismatch(AccessInfo ai, uptr pc, uptr frame, void *uc,
uptr *registers_frame = nullptr);



void HwasanTagMismatch(uptr addr, uptr access_info, uptr *registers_frame,
size_t outsize);

}

#define HWASAN_MALLOC_HOOK(ptr, size) do { if (&__sanitizer_malloc_hook) { __sanitizer_malloc_hook(ptr, size); } RunMallocHooks(ptr, size); } while (false)






#define HWASAN_FREE_HOOK(ptr) do { if (&__sanitizer_free_hook) { __sanitizer_free_hook(ptr); } RunFreeHooks(ptr); } while (false)







#if HWASAN_WITH_INTERCEPTORS && defined(__aarch64__)

typedef unsigned long __hw_sigset_t;



typedef unsigned long long __hw_register_buf[22];
struct __hw_jmp_buf_struct {




__hw_register_buf __jmpbuf;
int __mask_was_saved;
__hw_sigset_t __saved_mask;
};
typedef struct __hw_jmp_buf_struct __hw_jmp_buf[1];
typedef struct __hw_jmp_buf_struct __hw_sigjmp_buf[1];
#endif

#define ENSURE_HWASAN_INITED() do { CHECK(!hwasan_init_is_running); if (!hwasan_inited) { __hwasan_init(); } } while (0)







#endif
