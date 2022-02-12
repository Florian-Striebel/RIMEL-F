











#if !defined(SCUDO_ERRORS_H_)
#define SCUDO_ERRORS_H_

#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __scudo {

void NORETURN reportCallocOverflow(uptr Count, uptr Size);
void NORETURN reportPvallocOverflow(uptr Size);
void NORETURN reportAllocationAlignmentTooBig(uptr Alignment,
uptr MaxAlignment);
void NORETURN reportAllocationAlignmentNotPowerOfTwo(uptr Alignment);
void NORETURN reportInvalidPosixMemalignAlignment(uptr Alignment);
void NORETURN reportInvalidAlignedAllocAlignment(uptr Size, uptr Alignment);
void NORETURN reportAllocationSizeTooBig(uptr UserSize, uptr TotalSize,
uptr MaxSize);
void NORETURN reportRssLimitExceeded();
void NORETURN reportOutOfMemory(uptr RequestedSize);

}

#endif
