












#if !defined(DFSAN_FLAGS_H)
#define DFSAN_FLAGS_H

namespace __dfsan {

struct Flags {
#define DFSAN_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "dfsan_flags.inc"
#undef DFSAN_FLAG

void SetDefaults();
};

extern Flags flags_data;
inline Flags &flags() { return flags_data; }

}

#endif
