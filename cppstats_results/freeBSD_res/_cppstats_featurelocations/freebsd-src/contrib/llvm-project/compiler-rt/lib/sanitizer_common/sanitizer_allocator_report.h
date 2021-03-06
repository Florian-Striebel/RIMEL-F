












#if !defined(SANITIZER_ALLOCATOR_REPORT_H)
#define SANITIZER_ALLOCATOR_REPORT_H

#include "sanitizer_internal_defs.h"
#include "sanitizer_stacktrace.h"

namespace __sanitizer {

void NORETURN ReportCallocOverflow(uptr count, uptr size,
const StackTrace *stack);
void NORETURN ReportReallocArrayOverflow(uptr count, uptr size,
const StackTrace *stack);
void NORETURN ReportPvallocOverflow(uptr size, const StackTrace *stack);
void NORETURN ReportInvalidAllocationAlignment(uptr alignment,
const StackTrace *stack);
void NORETURN ReportInvalidAlignedAllocAlignment(uptr size, uptr alignment,
const StackTrace *stack);
void NORETURN ReportInvalidPosixMemalignAlignment(uptr alignment,
const StackTrace *stack);
void NORETURN ReportAllocationSizeTooBig(uptr user_size, uptr max_size,
const StackTrace *stack);
void NORETURN ReportOutOfMemory(uptr requested_size, const StackTrace *stack);
void NORETURN ReportRssLimitExceeded(const StackTrace *stack);

}

#endif
