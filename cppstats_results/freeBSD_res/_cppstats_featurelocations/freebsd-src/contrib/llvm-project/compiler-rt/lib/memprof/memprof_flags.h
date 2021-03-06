












#if !defined(MEMPROF_FLAGS_H)
#define MEMPROF_FLAGS_H

#include "sanitizer_common/sanitizer_flag_parser.h"
#include "sanitizer_common/sanitizer_internal_defs.h"









namespace __memprof {

struct Flags {
#define MEMPROF_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "memprof_flags.inc"
#undef MEMPROF_FLAG

void SetDefaults();
};

extern Flags memprof_flags_dont_use_directly;
inline Flags *flags() { return &memprof_flags_dont_use_directly; }

void InitializeFlags();

}

#endif
