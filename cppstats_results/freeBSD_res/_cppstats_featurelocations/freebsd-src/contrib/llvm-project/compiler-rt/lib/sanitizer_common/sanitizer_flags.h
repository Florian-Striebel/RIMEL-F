











#if !defined(SANITIZER_FLAGS_H)
#define SANITIZER_FLAGS_H

#include "sanitizer_internal_defs.h"

namespace __sanitizer {

enum HandleSignalMode {
kHandleSignalNo,
kHandleSignalYes,
kHandleSignalExclusive,
};

struct CommonFlags {
#define COMMON_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "sanitizer_flags.inc"
#undef COMMON_FLAG

void SetDefaults();
void CopyFrom(const CommonFlags &other);
};


extern CommonFlags common_flags_dont_use;
inline const CommonFlags *common_flags() {
return &common_flags_dont_use;
}

inline void SetCommonFlagsDefaults() {
common_flags_dont_use.SetDefaults();
}






inline void OverrideCommonFlags(const CommonFlags &cf) {
common_flags_dont_use.CopyFrom(cf);
}

void SubstituteForFlagValue(const char *s, char *out, uptr out_size);

class FlagParser;
void RegisterCommonFlags(FlagParser *parser,
CommonFlags *cf = &common_flags_dont_use);
void RegisterIncludeFlags(FlagParser *parser, CommonFlags *cf);




void InitializeCommonFlags(CommonFlags *cf = &common_flags_dont_use);


void InitializePlatformCommonFlags(CommonFlags *cf);

}

#endif
