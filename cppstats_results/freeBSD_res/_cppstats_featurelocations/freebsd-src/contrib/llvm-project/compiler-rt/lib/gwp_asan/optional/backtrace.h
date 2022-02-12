







#if !defined(GWP_ASAN_OPTIONAL_BACKTRACE_H_)
#define GWP_ASAN_OPTIONAL_BACKTRACE_H_

#include "gwp_asan/optional/printf.h"
#include "gwp_asan/options.h"

namespace gwp_asan {
namespace backtrace {














typedef void (*PrintBacktrace_t)(uintptr_t *TraceBuffer, size_t TraceLength,
Printf_t Print);






typedef size_t (*SegvBacktrace_t)(uintptr_t *TraceBuffer, size_t Size,
void *SignalContext);



options::Backtrace_t getBacktraceFunction();



PrintBacktrace_t getPrintBacktraceFunction();
SegvBacktrace_t getSegvBacktraceFunction();
}
}

#endif
