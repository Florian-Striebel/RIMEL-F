











#if !defined(SCUDO_INTERFACE_INTERNAL_H_)
#define SCUDO_INTERFACE_INTERNAL_H_

#include "sanitizer_common/sanitizer_internal_defs.h"

using __sanitizer::uptr;
using __sanitizer::s32;

extern "C" {
SANITIZER_INTERFACE_ATTRIBUTE SANITIZER_WEAK_ATTRIBUTE
const char* __scudo_default_options();

SANITIZER_INTERFACE_ATTRIBUTE
void __scudo_set_rss_limit(uptr LimitMb, s32 HardLimit);

SANITIZER_INTERFACE_ATTRIBUTE
void __scudo_print_stats();
}

#endif
