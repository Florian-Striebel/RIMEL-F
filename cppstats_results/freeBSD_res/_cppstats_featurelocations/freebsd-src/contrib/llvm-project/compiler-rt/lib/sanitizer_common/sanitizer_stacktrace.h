










#if !defined(SANITIZER_STACKTRACE_H)
#define SANITIZER_STACKTRACE_H

#include "sanitizer_common.h"
#include "sanitizer_internal_defs.h"
#include "sanitizer_platform.h"

namespace __sanitizer {

struct BufferedStackTrace;

static const u32 kStackTraceMax = 256;

#if SANITIZER_LINUX && defined(__mips__)
#define SANITIZER_CAN_FAST_UNWIND 0
#elif SANITIZER_WINDOWS
#define SANITIZER_CAN_FAST_UNWIND 0
#else
#define SANITIZER_CAN_FAST_UNWIND 1
#endif




#if SANITIZER_MAC
#define SANITIZER_CAN_SLOW_UNWIND 0
#else
#define SANITIZER_CAN_SLOW_UNWIND 1
#endif

struct StackTrace {
const uptr *trace;
u32 size;
u32 tag;

static const int TAG_UNKNOWN = 0;
static const int TAG_ALLOC = 1;
static const int TAG_DEALLOC = 2;
static const int TAG_CUSTOM = 100;

StackTrace() : trace(nullptr), size(0), tag(0) {}
StackTrace(const uptr *trace, u32 size) : trace(trace), size(size), tag(0) {}
StackTrace(const uptr *trace, u32 size, u32 tag)
: trace(trace), size(size), tag(tag) {}


void Print() const;



void PrintTo(InternalScopedString *output) const;





uptr PrintTo(char *out_buf, uptr out_buf_size) const;

static bool WillUseFastUnwind(bool request_fast_unwind) {
if (!SANITIZER_CAN_FAST_UNWIND)
return false;
if (!SANITIZER_CAN_SLOW_UNWIND)
return true;
return request_fast_unwind;
}

static uptr GetCurrentPc();
static inline uptr GetPreviousInstructionPc(uptr pc);
static uptr GetNextInstructionPc(uptr pc);
};


ALWAYS_INLINE
uptr StackTrace::GetPreviousInstructionPc(uptr pc) {
#if defined(__arm__)



return (pc - 3) & (~1);
#elif defined(__powerpc__) || defined(__powerpc64__) || defined(__aarch64__)

return pc - 4;
#elif defined(__sparc__) || defined(__mips__)
return pc - 8;
#elif SANITIZER_RISCV64






return pc - 2;
#else
return pc - 1;
#endif
}


struct BufferedStackTrace : public StackTrace {
uptr trace_buffer[kStackTraceMax];
uptr top_frame_bp;

BufferedStackTrace() : StackTrace(trace_buffer, 0), top_frame_bp(0) {}

void Init(const uptr *pcs, uptr cnt, uptr extra_top_pc = 0);




void Unwind(uptr pc, uptr bp, void *context, bool request_fast,
u32 max_depth = kStackTraceMax) {
top_frame_bp = (max_depth > 0) ? bp : 0;

if (max_depth <= 1) {
if (max_depth == 1)
trace_buffer[0] = pc;
size = max_depth;
return;
}
UnwindImpl(pc, bp, context, request_fast, max_depth);
}

void Unwind(u32 max_depth, uptr pc, uptr bp, void *context, uptr stack_top,
uptr stack_bottom, bool request_fast_unwind);

void Reset() {
*static_cast<StackTrace *>(this) = StackTrace(trace_buffer, 0);
top_frame_bp = 0;
}

private:

void UnwindImpl(uptr pc, uptr bp, void *context, bool request_fast,
u32 max_depth);


void UnwindFast(uptr pc, uptr bp, uptr stack_top, uptr stack_bottom,
u32 max_depth);
void UnwindSlow(uptr pc, u32 max_depth);
void UnwindSlow(uptr pc, void *context, u32 max_depth);

void PopStackFrames(uptr count);
uptr LocatePcInTrace(uptr pc);

BufferedStackTrace(const BufferedStackTrace &) = delete;
void operator=(const BufferedStackTrace &) = delete;

friend class FastUnwindTest;
};

#if defined(__s390x__)
static const uptr kFrameSize = 160;
#elif defined(__s390__)
static const uptr kFrameSize = 96;
#else
static const uptr kFrameSize = 2 * sizeof(uhwptr);
#endif


static inline bool IsValidFrame(uptr frame, uptr stack_top, uptr stack_bottom) {
return frame > stack_bottom && frame < stack_top - kFrameSize;
}

}



#define GET_CALLER_PC_BP uptr bp = GET_CURRENT_FRAME(); uptr pc = GET_CALLER_PC();



#define GET_CALLER_PC_BP_SP GET_CALLER_PC_BP; uptr local_stack; uptr sp = (uptr)&local_stack






#define GET_CURRENT_PC_BP uptr bp = GET_CURRENT_FRAME(); uptr pc = StackTrace::GetCurrentPc()



#define GET_CURRENT_PC_BP_SP GET_CURRENT_PC_BP; uptr local_stack; uptr sp = (uptr)&local_stack















#if defined(__x86_64__)
#define GET_CURRENT_PC() ({ uptr pc; asm("lea 0(%%rip), %0" : "=r"(pc)); pc; })





#else
#define GET_CURRENT_PC() StackTrace::GetCurrentPc()
#endif

#endif
