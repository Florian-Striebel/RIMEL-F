










#if !defined(MSAN_FLAGS_H)
#define MSAN_FLAGS_H

namespace __msan {

struct Flags {
#define MSAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "msan_flags.inc"
#undef MSAN_FLAG

void SetDefaults();
};

Flags *flags();

}

#endif
