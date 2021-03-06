







#if !defined(SCUDO_FLAGS_H_)
#define SCUDO_FLAGS_H_

#include "internal_defs.h"

namespace scudo {

struct Flags {
#define SCUDO_FLAG(Type, Name, DefaultValue, Description) Type Name;
#include "flags.inc"
#undef SCUDO_FLAG

#if defined(GWP_ASAN_HOOKS)
#define GWP_ASAN_OPTION(Type, Name, DefaultValue, Description) Type GWP_ASAN_##Name;

#include "gwp_asan/options.inc"
#undef GWP_ASAN_OPTION
#endif

void setDefaults();
};

Flags *getFlags();
void initFlags();
class FlagParser;
void registerFlags(FlagParser *Parser, Flags *F);

}

#endif
