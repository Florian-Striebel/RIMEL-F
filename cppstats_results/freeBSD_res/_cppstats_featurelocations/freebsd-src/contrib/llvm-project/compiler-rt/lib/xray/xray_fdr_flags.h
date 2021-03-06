












#if !defined(XRAY_FDR_FLAGS_H)
#define XRAY_FDR_FLAGS_H

#include "sanitizer_common/sanitizer_flag_parser.h"
#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __xray {

struct FDRFlags {
#define XRAY_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "xray_fdr_flags.inc"
#undef XRAY_FLAG

void setDefaults();
};

extern FDRFlags xray_fdr_flags_dont_use_directly;
extern void registerXRayFDRFlags(FlagParser *P, FDRFlags *F);
const char *useCompilerDefinedFDRFlags();
inline FDRFlags *fdrFlags() { return &xray_fdr_flags_dont_use_directly; }

}

#endif
