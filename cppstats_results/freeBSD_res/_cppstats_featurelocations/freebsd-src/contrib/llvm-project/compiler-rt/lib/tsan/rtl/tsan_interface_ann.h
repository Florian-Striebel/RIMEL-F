











#if !defined(TSAN_INTERFACE_ANN_H)
#define TSAN_INTERFACE_ANN_H

#include <sanitizer_common/sanitizer_internal_defs.h>




#if defined(__cplusplus)
extern "C" {
#endif

SANITIZER_INTERFACE_ATTRIBUTE void __tsan_acquire(void *addr);
SANITIZER_INTERFACE_ATTRIBUTE void __tsan_release(void *addr);

#if defined(__cplusplus)
}
#endif

#endif
