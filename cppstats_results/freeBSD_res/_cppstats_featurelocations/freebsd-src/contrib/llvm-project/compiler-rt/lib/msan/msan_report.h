













#if !defined(MSAN_REPORT_H)
#define MSAN_REPORT_H

#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_stacktrace.h"

namespace __msan {

void ReportUMR(StackTrace *stack, u32 origin);
void ReportExpectedUMRNotFound(StackTrace *stack);
void ReportStats();
void ReportAtExitStatistics();
void DescribeMemoryRange(const void *x, uptr size);
void ReportUMRInsideAddressRange(const char *what, const void *start, uptr size,
uptr offset);

}

#endif
