







#if !defined(SCUDO_CHECKS_H_)
#define SCUDO_CHECKS_H_

#include "common.h"

#include <errno.h>

#if !defined(__has_builtin)
#define __has_builtin(X) 0
#endif

namespace scudo {


inline void *setErrnoOnNull(void *Ptr) {
if (UNLIKELY(!Ptr))
errno = ENOMEM;
return Ptr;
}





inline bool checkAlignedAllocAlignmentAndSize(uptr Alignment, uptr Size) {
return Alignment == 0 || !isPowerOfTwo(Alignment) ||
!isAligned(Size, Alignment);
}



inline bool checkPosixMemalignAlignment(uptr Alignment) {
return Alignment == 0 || !isPowerOfTwo(Alignment) ||
!isAligned(Alignment, sizeof(void *));
}




inline bool checkForCallocOverflow(uptr Size, uptr N, uptr *Product) {
#if __has_builtin(__builtin_umull_overflow)
return __builtin_umull_overflow(Size, N, Product);
#else
*Product = Size * N;
if (!Size)
return false;
return (*Product / Size) != N;
#endif
}



inline bool checkForPvallocOverflow(uptr Size, uptr PageSize) {
return roundUpTo(Size, PageSize) < Size;
}

}

#endif
