











#if !defined(SCUDO_FLAGS_H_)
#define SCUDO_FLAGS_H_

namespace __scudo {

struct Flags {
#define SCUDO_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "scudo_flags.inc"
#undef SCUDO_FLAG

void setDefaults();
};

Flags *getFlags();

void initFlags();

}

#endif
