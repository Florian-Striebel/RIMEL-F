










#if !defined(UBSAN_FLAGS_H)
#define UBSAN_FLAGS_H

#include "sanitizer_common/sanitizer_internal_defs.h"

namespace __sanitizer {
class FlagParser;
}

namespace __ubsan {

struct Flags {
#define UBSAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "ubsan_flags.inc"
#undef UBSAN_FLAG

void SetDefaults();
};

extern Flags ubsan_flags;
inline Flags *flags() { return &ubsan_flags; }

void InitializeFlags();
void RegisterUbsanFlags(FlagParser *parser, Flags *f);

}

extern "C" {


SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
const char *__ubsan_default_options();
}

#endif
