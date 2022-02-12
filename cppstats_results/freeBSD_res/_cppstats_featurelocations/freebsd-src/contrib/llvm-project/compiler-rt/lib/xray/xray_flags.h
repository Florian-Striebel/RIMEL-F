












#if !defined(XRAY_FLAGS_H)
#define XRAY_FLAGS_H

#include "sanitizer_common/sanitizer_flag_parser.h"
#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __xray {

struct Flags {
#define XRAY_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "xray_flags.inc"
#undef XRAY_FLAG

void setDefaults();
};

extern Flags xray_flags_dont_use_directly;
extern void registerXRayFlags(FlagParser *P, Flags *F);
const char *useCompilerDefinedFlags();
inline Flags *flags() { return &xray_flags_dont_use_directly; }

void initializeFlags();

}

#endif
