










#if !defined(HWASAN_FLAGS_H)
#define HWASAN_FLAGS_H

#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __hwasan {

struct Flags {
#define HWASAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "hwasan_flags.inc"
#undef HWASAN_FLAG

void SetDefaults();
};

Flags *flags();

}

#endif
