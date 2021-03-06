











#if !defined(MEMPROF_INTERNAL_H)
#define MEMPROF_INTERNAL_H

#include "memprof_flags.h"
#include "memprof_interface_internal.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_libc.h"
#include "sanitizer_common/sanitizer_stacktrace.h"

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#error "The MemProfiler run-time should not be instrumented by MemProfiler"
#endif




#if !defined(MEMPROF_HAS_EXCEPTIONS)
#define MEMPROF_HAS_EXCEPTIONS 1
#endif

#if !defined(MEMPROF_DYNAMIC)
#if defined(PIC)
#define MEMPROF_DYNAMIC 1
#else
#define MEMPROF_DYNAMIC 0
#endif
#endif





namespace __memprof {

class MemprofThread;
using __sanitizer::StackTrace;

void MemprofInitFromRtl();


void PrintAddressSpaceLayout();


void InitializeShadowMemory();


void ReplaceSystemMalloc();


uptr FindDynamicShadowStart();
void *MemprofDoesNotSupportStaticLinkage();


MemprofThread *CreateMainThread();

void ReadContextStack(void *context, uptr *stack, uptr *ssize);


void TSDInit(void (*destructor)(void *tsd));
void *TSDGet();
void TSDSet(void *tsd);
void PlatformTSDDtor(void *tsd);

void *MemprofDlSymNext(const char *sym);



#define MEMPROF_MALLOC_HOOK(ptr, size) do { if (&__sanitizer_malloc_hook) __sanitizer_malloc_hook(ptr, size); RunMallocHooks(ptr, size); } while (false)





#define MEMPROF_FREE_HOOK(ptr) do { if (&__sanitizer_free_hook) __sanitizer_free_hook(ptr); RunFreeHooks(ptr); } while (false)






extern int memprof_inited;
extern int memprof_timestamp_inited;
extern int memprof_init_done;

extern bool memprof_init_is_running;
extern void (*death_callback)(void);
extern long memprof_init_timestamp_s;

}

#endif
