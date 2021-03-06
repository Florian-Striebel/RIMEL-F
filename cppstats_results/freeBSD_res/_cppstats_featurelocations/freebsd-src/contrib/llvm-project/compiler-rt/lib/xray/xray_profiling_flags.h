












#if !defined(XRAY_PROFILER_FLAGS_H)
#define XRAY_PROFILER_FLAGS_H

#include "sanitizer_common/sanitizer_flag_parser.h"
#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __xray {

struct ProfilerFlags {
#define XRAY_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "xray_profiling_flags.inc"
#undef XRAY_FLAG

void setDefaults();
};

extern ProfilerFlags xray_profiling_flags_dont_use_directly;
inline ProfilerFlags *profilingFlags() {
return &xray_profiling_flags_dont_use_directly;
}
void registerProfilerFlags(FlagParser *P, ProfilerFlags *F);

}

#endif
