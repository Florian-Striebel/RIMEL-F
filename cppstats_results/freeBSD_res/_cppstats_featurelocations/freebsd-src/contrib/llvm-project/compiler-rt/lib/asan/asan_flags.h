












#if !defined(ASAN_FLAGS_H)
#define ASAN_FLAGS_H

#include "sanitizer_common/sanitizer_internal_defs.h"
#include "sanitizer_common/sanitizer_flag_parser.h"










namespace __asan {

struct Flags {
#define ASAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "asan_flags.inc"
#undef ASAN_FLAG

void SetDefaults();
};

extern Flags asan_flags_dont_use_directly;
inline Flags *flags() {
return &asan_flags_dont_use_directly;
}

void InitializeFlags();

}

#endif
