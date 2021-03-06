











#if !defined(ASAN_INTERNAL_H)
#define ASAN_INTERNAL_H

#include "asan_flags.h"
#include "asan_interface_internal.h"
#include "sanitizer_common/sanitizer_common.h"
#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_stacktrace.h"
#include "sanitizer_common/sanitizer_libc.h"

#if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
#error "The AddressSanitizer run-time should not be"
" instrumented by AddressSanitizer"
#endif




#if !defined(ASAN_HAS_EXCEPTIONS)
#define ASAN_HAS_EXCEPTIONS 1
#endif



#if !defined(ASAN_LOW_MEMORY)
#if SANITIZER_IOS || SANITIZER_ANDROID
#define ASAN_LOW_MEMORY 1
#else
#define ASAN_LOW_MEMORY 0
#endif
#endif

#if !defined(ASAN_DYNAMIC)
#if defined(PIC)
#define ASAN_DYNAMIC 1
#else
#define ASAN_DYNAMIC 0
#endif
#endif





namespace __asan {

class AsanThread;
using __sanitizer::StackTrace;

void AsanInitFromRtl();


void InitializePlatformExceptionHandlers();


bool IsSystemHeapAddress(uptr addr);


void PrintAddressSpaceLayout();
void NORETURN ShowStatsAndAbort();


void InitializeShadowMemory();


void ReplaceSystemMalloc();


uptr FindDynamicShadowStart();
void *AsanDoesNotSupportStaticLinkage();
void AsanCheckDynamicRTPrereqs();
void AsanCheckIncompatibleRT();



bool PlatformUnpoisonStacks();





void UnpoisonStack(uptr bottom, uptr top, const char *type);


AsanThread *CreateMainThread();





typedef void (*globals_op_fptr)(__asan_global *, uptr);
void AsanApplyToGlobals(globals_op_fptr op, const void *needle);

void AsanOnDeadlySignal(int, void *siginfo, void *context);

void ReadContextStack(void *context, uptr *stack, uptr *ssize);
void StopInitOrderChecking();


void AsanTSDInit(void (*destructor)(void *tsd));
void *AsanTSDGet();
void AsanTSDSet(void *tsd);
void PlatformTSDDtor(void *tsd);

void AppendToErrorMessageBuffer(const char *buffer);

void *AsanDlSymNext(const char *sym);




bool HandleDlopenInit();



#define ASAN_MALLOC_HOOK(ptr, size) do { if (&__sanitizer_malloc_hook) __sanitizer_malloc_hook(ptr, size); RunMallocHooks(ptr, size); } while (false)




#define ASAN_FREE_HOOK(ptr) do { if (&__sanitizer_free_hook) __sanitizer_free_hook(ptr); RunFreeHooks(ptr); } while (false)




#define ASAN_ON_ERROR() if (&__asan_on_error) __asan_on_error()


extern int asan_inited;

extern bool asan_init_is_running;
extern void (*death_callback)(void);

const int kAsanHeapLeftRedzoneMagic = 0xfa;
const int kAsanHeapFreeMagic = 0xfd;
const int kAsanStackLeftRedzoneMagic = 0xf1;
const int kAsanStackMidRedzoneMagic = 0xf2;
const int kAsanStackRightRedzoneMagic = 0xf3;
const int kAsanStackAfterReturnMagic = 0xf5;
const int kAsanInitializationOrderMagic = 0xf6;
const int kAsanUserPoisonedMemoryMagic = 0xf7;
const int kAsanContiguousContainerOOBMagic = 0xfc;
const int kAsanStackUseAfterScopeMagic = 0xf8;
const int kAsanGlobalRedzoneMagic = 0xf9;
const int kAsanInternalHeapMagic = 0xfe;
const int kAsanArrayCookieMagic = 0xac;
const int kAsanIntraObjectRedzone = 0xbb;
const int kAsanAllocaLeftMagic = 0xca;
const int kAsanAllocaRightMagic = 0xcb;

static const uptr kCurrentStackFrameMagic = 0x41B58AB3;
static const uptr kRetiredStackFrameMagic = 0x45E0360E;

}

#endif
