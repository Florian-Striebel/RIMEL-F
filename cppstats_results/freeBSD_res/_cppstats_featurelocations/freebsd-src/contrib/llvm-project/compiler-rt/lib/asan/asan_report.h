












#if !defined(ASAN_REPORT_H)
#define ASAN_REPORT_H

#include "asan_allocator.h"
#include "asan_internal.h"
#include "asan_thread.h"

namespace __asan {

struct StackVarDescr {
uptr beg;
uptr size;
const char *name_pos;
uptr name_len;
uptr line;
};



int GetGlobalsForAddress(uptr addr, __asan_global *globals, u32 *reg_sites,
int max_globals);

const char *MaybeDemangleGlobalName(const char *name);
void PrintGlobalNameIfASCII(InternalScopedString *str, const __asan_global &g);
void PrintGlobalLocation(InternalScopedString *str, const __asan_global &g);

void PrintMemoryByte(InternalScopedString *str, const char *before, u8 byte,
bool in_shadow, const char *after = "\n");



bool ParseFrameDescription(const char *frame_descr,
InternalMmapVector<StackVarDescr> *vars);


void ReportGenericError(uptr pc, uptr bp, uptr sp, uptr addr, bool is_write,
uptr access_size, u32 exp, bool fatal);
void ReportDeadlySignal(const SignalContext &sig);
void ReportNewDeleteTypeMismatch(uptr addr, uptr delete_size,
uptr delete_alignment,
BufferedStackTrace *free_stack);
void ReportDoubleFree(uptr addr, BufferedStackTrace *free_stack);
void ReportFreeNotMalloced(uptr addr, BufferedStackTrace *free_stack);
void ReportAllocTypeMismatch(uptr addr, BufferedStackTrace *free_stack,
AllocType alloc_type,
AllocType dealloc_type);
void ReportMallocUsableSizeNotOwned(uptr addr, BufferedStackTrace *stack);
void ReportSanitizerGetAllocatedSizeNotOwned(uptr addr,
BufferedStackTrace *stack);
void ReportCallocOverflow(uptr count, uptr size, BufferedStackTrace *stack);
void ReportReallocArrayOverflow(uptr count, uptr size,
BufferedStackTrace *stack);
void ReportPvallocOverflow(uptr size, BufferedStackTrace *stack);
void ReportInvalidAllocationAlignment(uptr alignment,
BufferedStackTrace *stack);
void ReportInvalidAlignedAllocAlignment(uptr size, uptr alignment,
BufferedStackTrace *stack);
void ReportInvalidPosixMemalignAlignment(uptr alignment,
BufferedStackTrace *stack);
void ReportAllocationSizeTooBig(uptr user_size, uptr total_size, uptr max_size,
BufferedStackTrace *stack);
void ReportRssLimitExceeded(BufferedStackTrace *stack);
void ReportOutOfMemory(uptr requested_size, BufferedStackTrace *stack);
void ReportStringFunctionMemoryRangesOverlap(const char *function,
const char *offset1, uptr length1,
const char *offset2, uptr length2,
BufferedStackTrace *stack);
void ReportStringFunctionSizeOverflow(uptr offset, uptr size,
BufferedStackTrace *stack);
void ReportBadParamsToAnnotateContiguousContainer(uptr beg, uptr end,
uptr old_mid, uptr new_mid,
BufferedStackTrace *stack);

void ReportODRViolation(const __asan_global *g1, u32 stack_id1,
const __asan_global *g2, u32 stack_id2);


void ReportMacMzReallocUnknown(uptr addr, uptr zone_ptr,
const char *zone_name,
BufferedStackTrace *stack);
void ReportMacCfReallocUnknown(uptr addr, uptr zone_ptr,
const char *zone_name,
BufferedStackTrace *stack);

}
#endif
