







#if !defined(GWP_ASAN_OPTIONAL_SEGV_HANDLER_H_)
#define GWP_ASAN_OPTIONAL_SEGV_HANDLER_H_

#include "gwp_asan/guarded_pool_allocator.h"
#include "gwp_asan/optional/backtrace.h"
#include "gwp_asan/optional/printf.h"

namespace gwp_asan {
namespace segv_handler {






void installSignalHandlers(gwp_asan::GuardedPoolAllocator *GPA, Printf_t Printf,
gwp_asan::backtrace::PrintBacktrace_t PrintBacktrace,
gwp_asan::backtrace::SegvBacktrace_t SegvBacktrace);


void uninstallSignalHandlers();
}
}

#endif
