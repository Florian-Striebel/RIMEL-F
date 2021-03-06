












#if !defined(SANITIZER_ALLOCATOR_CHECKS_H)
#define SANITIZER_ALLOCATOR_CHECKS_H

#include "sanitizer_internal_defs.h"
#include "sanitizer_common.h"
#include "sanitizer_platform.h"

namespace __sanitizer {





void SetErrnoToENOMEM();


inline void *SetErrnoOnNull(void *ptr) {
if (UNLIKELY(!ptr))
SetErrnoToENOMEM();
return ptr;
}









inline bool CheckAlignedAllocAlignmentAndSize(uptr alignment, uptr size) {
#if SANITIZER_POSIX
return alignment != 0 && IsPowerOfTwo(alignment) &&
(size & (alignment - 1)) == 0;
#else
return alignment != 0 && size % alignment == 0;
#endif
}



inline bool CheckPosixMemalignAlignment(uptr alignment) {
return alignment != 0 && IsPowerOfTwo(alignment) &&
(alignment % sizeof(void *)) == 0;
}


inline bool CheckForCallocOverflow(uptr size, uptr n) {
if (!size)
return false;
uptr max = (uptr)-1L;
return (max / size) < n;
}



inline bool CheckForPvallocOverflow(uptr size, uptr page_size) {
return RoundUpTo(size, page_size) < size;
}

}

#endif
