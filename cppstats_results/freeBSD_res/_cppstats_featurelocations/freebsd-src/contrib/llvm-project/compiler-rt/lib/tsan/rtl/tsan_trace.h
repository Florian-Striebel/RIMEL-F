










#if !defined(TSAN_TRACE_H)
#define TSAN_TRACE_H

#include "tsan_defs.h"
#include "tsan_stack_trace.h"
#include "tsan_mutexset.h"

namespace __tsan {

const int kTracePartSizeBits = 13;
const int kTracePartSize = 1 << kTracePartSizeBits;
const int kTraceParts = 2 * 1024 * 1024 / kTracePartSize;
const int kTraceSize = kTracePartSize * kTraceParts;


enum EventType {
EventTypeMop,
EventTypeFuncEnter,
EventTypeFuncExit,
EventTypeLock,
EventTypeUnlock,
EventTypeRLock,
EventTypeRUnlock
};




typedef u64 Event;

const uptr kEventPCBits = 61;

struct TraceHeader {
#if !SANITIZER_GO
BufferedStackTrace stack0;
#else
VarSizeStackTrace stack0;
#endif
u64 epoch0;
MutexSet mset0;

TraceHeader() : stack0(), epoch0() {}
};

struct Trace {
Mutex mtx;
#if !SANITIZER_GO


uptr shadow_stack[kShadowStackSize];
#endif


TraceHeader headers[kTraceParts];

Trace() : mtx(MutexTypeTrace) {}
};

}

#endif
