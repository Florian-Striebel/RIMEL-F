













#if !defined(HWASAN_REPORT_H)
#define HWASAN_REPORT_H

#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_stacktrace.h"

namespace __hwasan {

void ReportStats();
void ReportTagMismatch(StackTrace *stack, uptr addr, uptr access_size,
bool is_store, bool fatal, uptr *registers_frame);
void ReportInvalidFree(StackTrace *stack, uptr addr);
void ReportTailOverwritten(StackTrace *stack, uptr addr, uptr orig_size,
const u8 *expected);
void ReportRegisters(uptr *registers_frame, uptr pc);
void ReportAtExitStatistics();


}

#endif
