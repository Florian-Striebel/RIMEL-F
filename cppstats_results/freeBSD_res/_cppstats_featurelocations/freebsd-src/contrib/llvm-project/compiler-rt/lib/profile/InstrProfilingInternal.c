










#if !defined(__Fuchsia__)

#include "InstrProfilingInternal.h"

static unsigned ProfileDumped = 0;

COMPILER_RT_VISIBILITY unsigned lprofProfileDumped() {
return ProfileDumped;
}

COMPILER_RT_VISIBILITY void lprofSetProfileDumped(unsigned Value) {
ProfileDumped = Value;
}

#endif
