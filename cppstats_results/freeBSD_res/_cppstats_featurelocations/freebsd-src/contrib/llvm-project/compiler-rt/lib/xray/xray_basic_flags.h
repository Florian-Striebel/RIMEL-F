












#if !defined(XRAY_BASIC_FLAGS_H)
#define XRAY_BASIC_FLAGS_H

#include "sanitizer_common/sanitizer_flag_parser.h"
#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __xray {

struct BasicFlags {
#define XRAY_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "xray_basic_flags.inc"
#undef XRAY_FLAG

void setDefaults();
};

extern BasicFlags xray_basic_flags_dont_use_directly;
extern void registerXRayBasicFlags(FlagParser *P, BasicFlags *F);
const char *useCompilerDefinedBasicFlags();
inline BasicFlags *basicFlags() { return &xray_basic_flags_dont_use_directly; }

}

#endif
