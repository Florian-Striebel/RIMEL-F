







#if !defined(GWP_ASAN_OPTIONS_H_)
#define GWP_ASAN_OPTIONS_H_

#include <stddef.h>
#include <stdint.h>

namespace gwp_asan {
namespace options {

















typedef size_t (*Backtrace_t)(uintptr_t *TraceBuffer, size_t Size);

struct Options {
Backtrace_t Backtrace = nullptr;


#define GWP_ASAN_OPTION(Type, Name, DefaultValue, Description) Type Name = DefaultValue;

#include "gwp_asan/options.inc"
#undef GWP_ASAN_OPTION

void setDefaults() {
#define GWP_ASAN_OPTION(Type, Name, DefaultValue, Description) Name = DefaultValue;

#include "gwp_asan/options.inc"
#undef GWP_ASAN_OPTION

Backtrace = nullptr;
}
};
}
}

#endif
