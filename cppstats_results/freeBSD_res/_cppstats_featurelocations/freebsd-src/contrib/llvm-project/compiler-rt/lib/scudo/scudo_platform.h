












#if !defined(SCUDO_PLATFORM_H_)
#define SCUDO_PLATFORM_H_

#include "sanitizer_common/sanitizer_allocator.h"

#if !SANITIZER_LINUX && !SANITIZER_FUCHSIA
#error "The Scudo hardened allocator is not supported on this platform."
#endif

#define SCUDO_TSD_EXCLUSIVE_SUPPORTED (!SANITIZER_ANDROID && !SANITIZER_FUCHSIA)

#if !defined(SCUDO_TSD_EXCLUSIVE)

#if SANITIZER_ANDROID || SANITIZER_FUCHSIA

#define SCUDO_TSD_EXCLUSIVE 0
#elif SANITIZER_LINUX && !SANITIZER_ANDROID

#define SCUDO_TSD_EXCLUSIVE 1
#else
#error "No default TSD model defined for this platform."
#endif
#endif


#if SCUDO_TSD_EXCLUSIVE && !SCUDO_TSD_EXCLUSIVE_SUPPORTED
#error "The exclusive TSD model is not supported on this platform."
#endif


#if !defined(SCUDO_SHARED_TSD_POOL_SIZE)
#if SANITIZER_ANDROID
#define SCUDO_SHARED_TSD_POOL_SIZE 2U
#else
#define SCUDO_SHARED_TSD_POOL_SIZE 32U
#endif
#endif


#if !defined(SCUDO_CAN_USE_PUBLIC_INTERFACE)
#define SCUDO_CAN_USE_PUBLIC_INTERFACE 1
#endif



#if !defined(SCUDO_CAN_USE_HOOKS)
#if SANITIZER_FUCHSIA
#define SCUDO_CAN_USE_HOOKS 1
#else
#define SCUDO_CAN_USE_HOOKS 0
#endif
#endif

namespace __scudo {

#if SANITIZER_CAN_USE_ALLOCATOR64
#if defined(__aarch64__) && SANITIZER_ANDROID
const uptr AllocatorSize = 0x4000000000ULL;
#elif defined(__aarch64__)
const uptr AllocatorSize = 0x10000000000ULL;
#else
const uptr AllocatorSize = 0x40000000000ULL;
#endif
#else
const uptr RegionSizeLog = SANITIZER_ANDROID ? 19 : 20;
#endif

#if !defined(SCUDO_SIZE_CLASS_MAP)
#define SCUDO_SIZE_CLASS_MAP Dense
#endif

#define SIZE_CLASS_MAP_TYPE SIZE_CLASS_MAP_TYPE_(SCUDO_SIZE_CLASS_MAP)
#define SIZE_CLASS_MAP_TYPE_(T) SIZE_CLASS_MAP_TYPE__(T)
#define SIZE_CLASS_MAP_TYPE__(T) T##SizeClassMap

typedef SIZE_CLASS_MAP_TYPE SizeClassMap;

}

#endif
